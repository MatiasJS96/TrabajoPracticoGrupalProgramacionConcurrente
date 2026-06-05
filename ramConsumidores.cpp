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
#include <condition_variable>
#include <algorithm>

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
int jobsFinalizados = 0;

// --- Pool de VRAM (máx. 5 slots) ---
sem_t semVRAM;
mutex mtxAsignar;
mutex mtxLiberar;
vector<int> poolVRAM;
mutex mtxPoolVec;

//  MESSAGE QUEUE


priority_queue<Job, vector<Job>, CompararJob> messageQueue;
mutex mtxQueue;
condition_variable cvQueue;
bool produccionTerminada = false;

void pushJob(Job j) {
    {
        lock_guard<mutex> lock(mtxQueue);
        messageQueue.push(j);
    }
    cvQueue.notify_one();
}

// Devuelve false cuando ya no quedan trabajos
bool popJob(Job& resultado) {
    unique_lock<mutex> lock(mtxQueue);

    cvQueue.wait(lock, [] {
        return !messageQueue.empty() || produccionTerminada;
    });

    if (messageQueue.empty())
        return false;

    resultado = messageQueue.top();
    messageQueue.pop();

    return true;
}


//  LOGGING


string timestamp() {
    auto ahora = system_clock::now();
    auto t = system_clock::to_time_t(ahora);
    auto ms = duration_cast<milliseconds>(
        ahora.time_since_epoch()) % 1000;

    ostringstream ss;
    ss << put_time(localtime(&t), "%H:%M:%S")
       << "." << setw(3)
       << setfill('0')
       << ms.count();

    return ss.str();
}

void loguear(int jobId,
             Prioridad prio,
             const string& evento) {

    lock_guard<mutex> lock(mtxLog);

    string pStr =
        (prio == Prioridad::PREMIUM)
        ? "PREMIUM"
        : "FREE";

    string linea =
        "[" + timestamp() + "] - Job "
        + to_string(jobId)
        + " - "
        + pStr
        + " - "
        + evento;

    cout << linea << "\n";

    if (archivoLog.is_open())
        archivoLog << linea << "\n";
}


//  PRODUCTOR


void productor(int idProductor,
               int cantJobs,
               int offsetId) {

    for (int i = 0; i < cantJobs; i++) {

        Job j;

        j.id = offsetId + i;
        j.prioridad =
            (i % 3 == 0)
            ? Prioridad::FREE
            : Prioridad::PREMIUM;

        j.tiempoCreacion =
            steady_clock::now();

        loguear(j.id,
                j.prioridad,
                "CREADO");

        this_thread::sleep_for(
            milliseconds(100));

        loguear(j.id,
                j.prioridad,
                "EN_COLA");

        pushJob(j);
    }

    cout << "[Productor "
         << idProductor
         << "] Terminó de generar "
         << cantJobs
         << " jobs.\n";
}


//  CONSUMIDOR


void consumidor(int idWorker) {

    while (true) {

        Job job;

        if (!popJob(job))
            break;

        // Esperar slot libre
        sem_wait(&semVRAM);

        // Asignación exclusiva
        {
            lock_guard<mutex> lock(
                mtxAsignar);

            {
                lock_guard<mutex> lv(
                    mtxPoolVec);

                poolVRAM.push_back(
                    job.id);
            }

            loguear(job.id,
                    job.prioridad,
                    "ASIGNADO_VRAM");

            cout << "  [Worker "
                 << idWorker
                 << "] Pool ocupado: "
                 << poolVRAM.size()
                 << "/5\n";

            this_thread::sleep_for(
                milliseconds(450));
        }

        // Procesamiento
        this_thread::sleep_for(
            milliseconds(600));

        // Liberación exclusiva
        {
            lock_guard<mutex> lock(
                mtxLiberar);

            {
                lock_guard<mutex> lv(
                    mtxPoolVec);

                auto it =
                    find(poolVRAM.begin(),
                         poolVRAM.end(),
                         job.id);

                if (it != poolVRAM.end())
                    poolVRAM.erase(it);
            }

            loguear(job.id,
                    job.prioridad,
                    "FINALIZADO");

            this_thread::sleep_for(
                milliseconds(250));
        }

        // Liberar slot
        sem_post(&semVRAM);

        // Incrementar contador
        {
            lock_guard<mutex> lock(
                mtxContador);

            jobsFinalizados++;
        }
    }

    cout << "[Worker "
         << idWorker
         << "] Sin más jobs, terminando.\n";
}


//  MAIN


int main() {

    int numProductores = 3;
    int numConsumidores = 3;
    int jobsPorProductor = 5;

    cout << "=== Sistema de Renderizado ===\n";
    cout << "Productores: "
         << numProductores
         << " | Consumidores: "
         << numConsumidores
         << " | Jobs totales: "
         << numProductores
            * jobsPorProductor
         << "\n\n";

    sem_init(&semVRAM, 0, 5);

    vector<thread> hilosProductores;

    for (int i = 0;
         i < numProductores;
         i++) {

        hilosProductores.emplace_back(
            productor,
            i + 1,
            jobsPorProductor,
            i * jobsPorProductor);
    }

    vector<thread> hilosConsumidores;

    for (int i = 0;
         i < numConsumidores;
         i++) {

        hilosConsumidores.emplace_back(
            consumidor,
            i + 1);
    }

    // Esperar productores
    for (auto& t : hilosProductores)
        t.join();

    // Avisar fin de producción
    {
        lock_guard<mutex> lock(
            mtxQueue);

        produccionTerminada = true;
    }

    cvQueue.notify_all();

    // Esperar consumidores
    for (auto& t : hilosConsumidores)
        t.join();

    sem_destroy(&semVRAM);

    cout << "\n=== FIN ===\n";
    cout << "Jobs finalizados: "
         << jobsFinalizados
         << "\n";

    cout << "Log guardado en sistema.log\n";

    return 0;
}
