#include "Consumidor.h"
#include "Utilidades.h"
#include <iostream>
#include <chrono>
#include <thread>

Consumidor::Consumidor(
    int id,
    ColaMensajes& cola,
    PoolVRAM& pool,
    Logger& logger,
    std::atomic<int>& contador
)
:
idConsumidor(id),
cola(cola),
pool(pool),
logger(logger),
contadorFinalizados(contador)
{
}

void Consumidor::iniciar() {
    hilo = std::thread(&Consumidor::ejecutar, this);
}

void Consumidor::esperar() {
    if(hilo.joinable()) {
        hilo.join();
    }
}

void Consumidor::ejecutar() {
    while(true) {
        auto trabajo = cola.obtenerTrabajo();
        if(trabajo == nullptr){
            if(cola.estaFinalizada()) {
                break;
            }
            continue;
        }
        trabajo->establecerEstado(ASIGNADO_VRAM);

        logger.registrar(trabajo->obtenerId(),Utilidades::prioridadATexto(trabajo->obtenerPrioridad()),
            "ASIGNADO_VRAM"
        );

        std::cout
            << "[CONSUMIDOR "
            << idConsumidor
            << "] Procesando Job "
            << trabajo->obtenerId()
            << std::endl;

            pool.asignarSlot();
            std::this_thread::sleep_for(std::chrono::milliseconds(600));
            pool.liberarSlot();

            trabajo->establecerEstado(FINALIZADO);

            logger.registrar(trabajo->obtenerId(), Utilidades::prioridadATexto(trabajo->obtenerPrioridad()),
            "FINALIZADO"
            ); contadorFinalizados++;

        std::cout
            << "[CONSUMIDOR "
            << idConsumidor
            << "] Job "
            << trabajo->obtenerId()
            << " finalizado"
            << std::endl;
    }
}
