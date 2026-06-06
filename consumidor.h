#ifndef CONSUMIDOR_H
#define CONSUMIDOR_H

#include <thread>
#include <mutex>

#include "ColaMensajes.h"
#include "PoolVRAM.h"
#include "Logger.h"

class Consumidor {

private:
    int idConsumidor;
    ColaMensajes& cola;
    PoolVRAM& pool;
    Logger& logger;

    int& contadorFinalizados;
    std::mutex& mutexContador;

    std::thread hilo;

public:
    Consumidor(
        int id,
        ColaMensajes& cola,
        PoolVRAM& pool,
        Logger& logger,
        int& contador,
        std::mutex& mutexContador
    );

    void iniciar();
    void esperar();

private:
    void ejecutar();
};

#endif
