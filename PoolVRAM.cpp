#include "PoolVRAM.h"
#include <chrono>
#include <thread>

PoolVRAM::PoolVRAM() {
    slotsDisponibles = 5;
}

void PoolVRAM::asignarSlot() {
    std::unique_lock<std::mutex>lock(mutexPool);

    condicionPool.wait(lock,[this] {
            return slotsDisponibles > 0;
        }
    );

    /*Restriccion: Debe existir una demora de 450 ms entre asignaciones.*/

    std::this_thread::sleep_for( std::chrono::milliseconds(450));
    slotsDisponibles--;
}

void PoolVRAM::liberarSlot() {
    std::unique_lock<std::mutex>
        lock(mutexPool);

    /*Restriccion: Debe existir una demorade 250 ms entre liberaciones.*/

    std::this_thread::sleep_for(
        std::chrono::milliseconds(250)
    );

    slotsDisponibles++;
    lock.unlock();
    condicionPool.notify_one();
}

int PoolVRAM::obtenerDisponibles(){
    std::lock_guard<std::mutex>
        lock(mutexPool);

    return slotsDisponibles;
}
