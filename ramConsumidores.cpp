#include <iostream>
#include <thread>
#include <mutex>
#include <semaphore.h>
#include <queue>
#include <vector>
#include <chrono>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <atomic>
#include <condition_variable>
 
using namespace std;
using namespace chrono;
 
// ============================================================
//  ESTRUCTURAS
// ============================================================
 
enum class Prioridad { FREE = 0, PREMIUM = 1 };
 
struct Job {
    int id;
    Prioridad prioridad;
    steady_clock::time_point tiempoCreacion;
};
 
// Comparador para la priority_queue (PREMIUM primero)
struct CompararJob {
    bool operator()(const Job& a, const Job& b) {
        return (int)a.prioridad < (int)b.prioridad;
    }
};
 
// ============================================================
//  VARIABLES GLOBALES COMPARTIDAS
// ============================================================
 
// --- Log ---
mutex mtxLog;
ofstream archivoLog("sistema.log");
 
// --- Contador global de jobs finalizados ---
mutex mtxContador;
atomic<int> jobsFinalizados{0};   // atomic como alternativa segura
 
// --- Pool de VRAM (máx. 5 slots) ---
sem_t semVRAM;          // controla los 5 slots disponibles
mutex mtxAsignar;       // solo 1 asignación simultánea
mutex mtxLiberar;       // solo 1 liberación simultánea
vector<int> poolVRAM;   // IDs de jobs actualmente en VRAM
mutex mtxPoolVec;       // protege el vector poolVRAM
 
// ============================================================
//  ==  STUB DE LA QUEUE  ==
//  Reemplazá esto con la implementación real de tu compañero.
//  La interfaz que necesitás es:
//    - pushJob(Job j)   → inserta un job con prioridad
//    - popJob()         → devuelve el job de mayor prioridad
//    - hayJobs()        → true si la queue no está vacía
// ============================================================
 
priority_queue<Job, vector<Job>, CompararJob> messageQueue;
mutex mtxQueue;
condition_variable cvQueue;
bool produccionTerminada = false;   // señal para que los workers paren
 
void pushJob(Job j) {
    {
        lock_guard<mutex> lock(mtxQueue);
        messageQueue.push(j);
    }
    cvQueue.notify_one();   // avisa a un Worker que hay trabajo
}
 
// Bloquea hasta que haya un job disponible o se termine la producción
// Devuelve false si no hay más jobs y la producción terminó
bool popJob(Job& resultado) {
    unique_lock<mutex> lock(mtxQueue);
    cvQueue.wait(lock, [] {
        return !messageQueue.empty() || produccionTerminada;
    });
    if (messageQueue.empty()) return false;
    resultado = messageQueue.top();
    messageQueue.pop();
    return true;
}
 
// ============================================================
//  LOGGING
// ============================================================
 
string timestamp() {
    auto ahora = system_clock::now();
    auto t = system_clock::to_time_t(ahora);
    auto ms = duration_cast<milliseconds>(ahora.time_since_epoch()) % 1000;
    ostringstream ss;
    ss << put_time(localtime(&t), "%H:%M:%S")
       << "." << setw(3) << setfill('0') << ms.count();
    return ss.str();
}
 
void loguear(int jobId, Prioridad prio, const string& evento) {
    lock_guard<mutex> lock(mtxLog);
    string pStr = (prio == Prioridad::PREMIUM) ? "PREMIUM" : "FREE";
    string linea = "[" + timestamp() + "] - Job " + to_string(jobId)
                 + " - " + pStr + " - " + evento;
    cout << linea << "\n";
    if (archivoLog.is_open()) archivoLog << linea << "\n";
}
 
// ============================================================
//  PRODUCTOR (STUB — genera N jobs para las pruebas)
// ============================================================
 
void productor(int idProductor, int cantJobs, int offsetId) {
    for (int i = 0; i < cantJobs; i++) {
        Job j;
        j.id = offsetId + i;
        // Alterna PREMIUM / FREE para ver el orden de despacho
        j.prioridad = (i % 3 == 0) ? Prioridad::FREE : Prioridad::PREMIUM;
        j.tiempoCreacion = steady_clock::now();
 
        loguear(j.id, j.prioridad, "CREADO");
        this_thread::sleep_for(milliseconds(100));  // retardo de ingreso a Queue
        loguear(j.id, j.prioridad, "EN_COLA");
        pushJob(j);
    }
    cout << "[Productor " << idProductor << "] Terminó de generar " << cantJobs << " jobs.\n";
}
 
// ============================================================
//  CONSUMIDOR (TU PARTE)
// ============================================================
 
void consumidor(int idWorker) {
    while (true) {
        Job job;
        if (!popJob(job)) break;  // no hay más jobs y producción terminó
 
        // 1. Esperar slot libre en VRAM
        sem_wait(&semVRAM);
 
        // 2. Asignación exclusiva (solo 1 a la vez + 450ms de retardo)
        {
            lock_guard<mutex> lock(mtxAsignar);
 
            {
                lock_guard<mutex> lv(mtxPoolVec);
                poolVRAM.push_back(job.id);
            }
 
            loguear(job.id, job.prioridad, "ASIGNADO_VRAM");
            cout << "  [Worker " << idWorker << "] Pool ocupado: "
                 << poolVRAM.size() << "/5\n";
 
            this_thread::sleep_for(milliseconds(450));
        }
 
        // 3. Tiempo mínimo de procesamiento en VRAM
        this_thread::sleep_for(milliseconds(600));
 
        // 4. Liberación exclusiva (solo 1 a la vez + 250ms de retardo)
        {
            lock_guard<mutex> lock(mtxLiberar);
 
            {
                lock_guard<mutex> lv(mtxPoolVec);
                auto it = find(poolVRAM.begin(), poolVRAM.end(), job.id);
                if (it != poolVRAM.end()) poolVRAM.erase(it);
            }
 
            loguear(job.id, job.prioridad, "FINALIZADO");
 
            this_thread::sleep_for(milliseconds(250));
        }
 
        // 5. Liberar slot
        sem_post(&semVRAM);
 
        // 6. Incrementar contador global (protegido)
        {
            lock_guard<mutex> lock(mtxContador);
            jobsFinalizados++;
        }
    }
    cout << "[Worker " << idWorker << "] Sin más jobs, terminando.\n";
}
 
// ============================================================
//  MAIN — configurá acá el escenario de prueba
// ============================================================
 
int main() {
    // ---- Parámetros del escenario ----
    // Cambiá estos valores para probar las distintas configuraciones:
    //   Config A: productores=1, consumidores=2
    //   Config B: productores=3, consumidores=1
    //   Config C: productores=3, consumidores=3
    int numProductores  = 3;
    int numConsumidores = 3;
    int jobsPorProductor = 5;   // ponelo en 500 para 1500 jobs totales con 3 productores
 
    cout << "=== Sistema de Renderizado — TEST Pool VRAM ===\n";
    cout << "Productores: " << numProductores
         << "  |  Consumidores: " << numConsumidores
         << "  |  Jobs totales: " << numProductores * jobsPorProductor << "\n\n";
 
    // Inicializar semáforo con 5 slots
    sem_init(&semVRAM, 0, 5);
 
    // Lanzar productores
    vector<thread> hilosProductores;
    for (int i = 0; i < numProductores; i++) {
        hilosProductores.emplace_back(productor, i + 1,
                                      jobsPorProductor,
                                      i * jobsPorProductor);
    }
 
    // Lanzar consumidores
    vector<thread> hilosConsumidores;
    for (int i = 0; i < numConsumidores; i++) {
        hilosConsumidores.emplace_back(consumidor, i + 1);
    }
 
    // Esperar que los productores terminen
    for (auto& t : hilosProductores) t.join();
 
    // Señalar a los consumidores que no vendrán más jobs
    {
        lock_guard<mutex> lock(mtxQueue);
        produccionTerminada = true;
    }
    cvQueue.notify_all();
 
    // Esperar que los consumidores terminen
    for (auto& t : hilosConsumidores) t.join();
 
    sem_destroy(&semVRAM);
 
    cout << "\n=== FIN ===\n";
    cout << "Jobs finalizados: " << jobsFinalizados << "\n";
    cout << "Log guardado en sistema.log\n";
 
    return 0;
}
 
// ============================================================
//  COMPILAR CON:
//    g++ -std=c++20 test_pool_vram.cpp -o test_pool_vram -lpthread
//  EJECUTAR:
//    ./test_pool_vram
// ============================================================
 
