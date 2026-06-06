#ifndef GESTORRENDERIZADO_H
#define GESTORRENDERIZADO_H

#include <vector>
#include <memory>
#include <mutex>
#include "Productor.h"
#include "Consumidor.h"
#include "PoolVRAM.h"
#include "Logger.h"

class GestorRenderizado {

private:

    ColaMensajes cola;
    PoolVRAM pool;
    Logger logger;

    int jobsFinalizados;
    std::mutex mutexContador;

public:

    GestorRenderizado();

    void ejecutarConfiguracionA();
    void ejecutarConfiguracionB();
    void ejecutarConfiguracionC();

    void pruebaCargaMasiva();
    void pruebaVacuidad();
    void pruebaSaturacion();
    void pruebaEquidad();

private:

    void ejecutarEscenario(
        int productores,
        int consumidores,
        int trabajos
    );
};

#endif
