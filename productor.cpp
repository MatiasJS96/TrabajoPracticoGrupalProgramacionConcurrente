#include "Productor.h"
#include "Utilidades.h"
#include <iostream>
#include <memory>
#include <chrono>
#include <thread>
#include <cstdlib>

Productor::Productor(
    int id,
    int cantidad,
    ColaMensajes& cola,
    Logger& logger
)
:
idProductor(id),
cantidadTrabajos(cantidad),
cola(cola),
logger(logger)
{}

void Productor::iniciar() {
    hilo = std::thread( &Productor::ejecutar, this);
}

void Productor::esperar() {
    if(hilo.joinable()) {
        hilo.join();
    }
}

void Productor::ejecutar() {
    for(int i = 0; i < cantidadTrabajos; i++){
        std::this_thread::sleep_for(
            std::chrono::milliseconds(100)
        );

        /*70% Premium 30% Free*/

        int prioridad = (rand() % 100 < 70)? 1: 0;
        int idTrabajo = idProductor * 100000 + i;
        auto trabajo = std::make_shared<Trabajo>(idTrabajo, prioridad);

        logger.registrar(trabajo->obtenerId(),Utilidades::prioridadATexto(prioridad),
            "CREADO"
        );

        trabajo->establecerEstado(EN_COLA);

        logger.registrar(trabajo->obtenerId(),Utilidades::prioridadATexto(prioridad),
            "EN_COLA"
        );

        cola.insertarTrabajo(trabajo);

        std::cout
            << "[PRODUCTOR "
            << idProductor
            << "] Job "
            << trabajo->obtenerId()
            << " agregado"
            << std::endl;
    }
}
