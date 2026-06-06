#include "ColaMensajes.h"

#include <chrono>

ColaMensajes::ColaMensajes(){
    finalizada = false;

    ultimoDespachoFree = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count();
}

bool ColaMensajes::ComparadorPrioridad::operator()(const std::shared_ptr<Trabajo>& a,const std::shared_ptr<Trabajo>& b) {
    return a->obtenerPrioridad()
        < b->obtenerPrioridad();
}

void ColaMensajes::insertarTrabajo(std::shared_ptr<Trabajo> trabajo) {
    {
        std::lock_guard<std::mutex>lock(mutexCola);
        cola.push(trabajo);
    }

    condicion.notify_one();
}

std::shared_ptr<Trabajo>ColaMensajes::obtenerTrabajo() {

    std::unique_lock<std::mutex>lock(mutexCola);

    condicion.wait(lock,[this] {

            return!cola.empty()|| finalizada;
        }
    );

    if(cola.empty()) {
        return nullptr;
    }

    auto ahora = std::chrono::duration_cast <std::chrono::milliseconds>
    (std::chrono::steady_clock::now().time_since_epoch()).count();

    long long diferencia = ahora - ultimoDespachoFree;

    /* Anti-Starvation

    Si pasan mas de 5000 ms sin despachar un trabajo Free, se busca uno manualmente.*/

    if(diferencia >= 5000) {
            std::vector<std::shared_ptr<Trabajo>> temporal;

        std::shared_ptr<Trabajo>trabajoFree = nullptr;

        while(!cola.empty()){
            auto actual = cola.top();
            cola.pop();

            if(actual->obtenerPrioridad() == 0 && trabajoFree == nullptr) {
                trabajoFree = actual;
            }
            else{
                temporal.push_back(actual);
            }
        }

        for(auto& trabajo : temporal){
            cola.push(trabajo);
        }

        if(trabajoFree != nullptr){
            ultimoDespachoFree = ahora;

            return trabajoFree;
        }
    }

    auto trabajo = cola.top();
    cola.pop();

    if(trabajo->obtenerPrioridad() == 0){
        ultimoDespachoFree = ahora;
    }

    return trabajo;
}

bool ColaMensajes::estaVacia() {
    std::lock_guard<std::mutex>lock(mutexCola);

    return cola.empty();
}

void ColaMensajes::finalizar() {
    {
        std::lock_guard<std::mutex> lock(mutexCola);
        finalizada = true;
    }
    condicion.notify_all();
}

bool ColaMensajes::estaFinalizada(){
    std::lock_guard<std::mutex>lock(mutexCola);

    return finalizada;
}
