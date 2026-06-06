#ifndef POOLVRAM_H
#define POOLVRAM_H
#include <mutex>
#include <condition_variable>

class PoolVRAM {
private:
    int slotsDisponibles;
    std::mutex mutexPool;
    std::condition_variable condicionPool;

public:
    PoolVRAM();
    void asignarSlot();
    void liberarSlot();
    int obtenerDisponibles();
};

#endif // POOLVRAM_H_INCLUDED
