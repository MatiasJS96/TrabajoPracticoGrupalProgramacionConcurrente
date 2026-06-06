#include "Utilidades.h"
#include <chrono>
#include <iomanip>
#include <sstream>

std::string Utilidades::obtenerTimestamp() {

    using namespace std::chrono;
    auto ahora = system_clock::now();
    auto tiempo = system_clock::to_time_t(ahora);
    auto milisegundos = duration_cast<milliseconds>(ahora.time_since_epoch()) % 1000;

    std::tm tm = *std::localtime(&tiempo);
    std::ostringstream salida;

    salida
        << std::put_time(&tm,"%H:%M:%S")
        << "."
        << std::setw(3)
        << std::setfill('0')
        << milisegundos.count();

    return salida.str();
}

std::string Utilidades::prioridadATexto(int prioridad) {
    if(prioridad == 1){
        return "Premium";
    }
    return "Free";
}

std::string Utilidades::estadoATexto(EstadoTrabajo estado) {
    switch(estado) {
    case CREADO:
        return "CREADO";

    case EN_COLA:
        return "EN_COLA";

    case ASIGNADO_VRAM:
        return "ASIGNADO_VRAM";

    case FINALIZADO:
        return "FINALIZADO";
    }

    return "DESCONOCIDO";
}
