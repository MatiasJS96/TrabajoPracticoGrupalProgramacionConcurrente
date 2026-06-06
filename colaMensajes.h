#ifndef COLAMENSAJES_H
#define COLAMENSAJES_H
#include <queue>
#include <mutex>
#include <condition_variable>
#include <memory>
#include "Trabajo.h"

class ColaMensajes {

private:
    struct ComparadorPrioridad
    {
        bool operator()(const std::shared_ptr<Trabajo>& a, const std::shared_ptr<Trabajo>& b);
    };

    std::priority_queue
    <
        std::shared_ptr<Trabajo>,
        std::vector<std::shared_ptr<Trabajo>>,
        ComparadorPrioridad
    > cola;

    std::mutex mutexCola;
    std::condition_variable condicion;
    bool finalizada;
    long long ultimoDespachoFree;

public:
    ColaMensajes();
    void insertarTrabajo(std::shared_ptr<Trabajo> trabajo);
    std::shared_ptr<Trabajo> obtenerTrabajo();
    bool estaVacia();
    void finalizar();
    bool estaFinalizada();
};

#endif // COLAMENSAJES_H_INCLUDED
