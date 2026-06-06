#ifndef TRABAJO_H
#define TRABAJO_H
#include <string>
#include <chrono>

enum EstadoTrabajo {
    CREADO,
    EN_COLA,
    ASIGNADO_VRAM,
    FINALIZADO
};

class Trabajo {
private:

    int id;
    int prioridad;
    std::chrono::system_clock::time_point timestampCreacion;
    EstadoTrabajo estado;

public:

    Trabajo(int id, int prioridad);
    int obtenerId() const;
    int obtenerPrioridad() const;
    EstadoTrabajo obtenerEstado() const;
    void establecerEstado(EstadoTrabajo nuevoEstado);
    std::chrono::system_clock::time_point obtenerTimestamp() const;
};

#endif // TRABAJO_H_INCLUDED
