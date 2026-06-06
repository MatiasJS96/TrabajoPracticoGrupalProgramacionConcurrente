#ifndef CONSUMIDOR_H
#define CONSUMIDOR_H
#include <thread>
#include <atomic>
#include "ColaMensajes.h"
#include "PoolVRAM.h"
#include "Logger.h"

class Consumidor {

private:
    int idConsumidor;
    ColaMensajes& cola;
    PoolVRAM& pool;
    Logger& logger;
    std::atomic<int>& contadorFinalizados;
    std::thread hilo;

public:
    Consumidor(int id, ColaMensajes& cola, PoolVRAM& pool, Logger& logger, std::atomic<int>& contador);
    void iniciar();
    void esperar();

private:
    void ejecutar();
};

#endif // CONSUMIDOR_H_INCLUDED
