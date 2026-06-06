#ifndef UTILIDADES_H
#define UTILIDADES_H
#include <string>
#include "Trabajo.h"

class Utilidades {

public:
    static std::string obtenerTimestamp();
    static std::string prioridadATexto(
        int prioridad
    );

    static std::string estadoATexto(
        EstadoTrabajo estado
    );
};

#endif // UTILIDADES_H_INCLUDED
