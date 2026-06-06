#include "Trabajo.h"

Trabajo::Trabajo(int id, int prioridad) {

    this->id = id;
    this->prioridad = prioridad;
    timestampCreacion = std::chrono::system_clock::now();
    estado = CREADO;
}

int Trabajo::obtenerId() const {
    return id;
}

int Trabajo::obtenerPrioridad() const {
    return prioridad;
}

EstadoTrabajo Trabajo::obtenerEstado() const {
    return estado;
}

void Trabajo::establecerEstado(EstadoTrabajo nuevoEstado) {
    estado = nuevoEstado;
}

std::chrono::system_clock::time_point
Trabajo::obtenerTimestamp() const {
    return timestampCreacion;
}
