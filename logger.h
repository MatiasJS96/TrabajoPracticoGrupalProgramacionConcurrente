#ifndef LOGGER_H
#define LOGGER_H

#include <fstream>
#include <mutex>
#include <string>

class Logger {

private:
    std::ofstream archivo;
    std::mutex mutexLogger;

public:
    Logger();
    ~Logger();

    void registrar(
        int idTrabajo,
        const std::string& prioridad,
        const std::string& evento
    );
};

#endif // LOGGER_H_INCLUDED
