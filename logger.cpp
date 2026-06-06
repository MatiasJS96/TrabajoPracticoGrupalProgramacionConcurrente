#include "Logger.h"
#include "Utilidades.h"

Logger::Logger(){

    archivo.open("sistema.log", std::ios::out);
}

Logger::~Logger() {

    if(archivo.is_open()) {
        archivo.close();
    }
}

void Logger::registrar(int idTrabajo, const std::string& prioridad,const std::string& evento) {
    std::lock_guard<std::mutex>lock(mutexLogger);

    archivo
        << "["
        << Utilidades::obtenerTimestamp()
        << "] - "
        << idTrabajo
        << " - "
        << prioridad
        << " - "
        << evento
        << std::endl;
}
