#ifndef PRODUCTOR_H
#define PRODUCTOR_H
#include <thread>
#include <atomic>
#include "ColaMensajes.h"
#include "Logger.h"

class Productor{
private:

    int idProductor;
    int cantidadTrabajos;
    ColaMensajes& cola;
    Logger& logger;
    std::thread hilo;

public:

    Productor(
        int id,
        int cantidad,
        ColaMensajes& cola,
        Logger& logger
    );

    void iniciar();
    void esperar();

private:
    void ejecutar();
};

#endif // PRODUCTOR_H_INCLUDED
