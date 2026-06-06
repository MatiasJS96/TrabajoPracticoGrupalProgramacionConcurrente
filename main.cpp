#include "GestorRenderizado.h"

#include <iostream>
#include <ctime>
#include <cstdlib>

using namespace std;

void mostrarMenu()
{
    cout << "\n========================================\n";
    cout << " SISTEMA DE GESTION DE GRANJA DE RENDER\n";
    cout << "========================================\n";
    cout << "1 - Configuracion A (1P - 2C)\n";
    cout << "2 - Configuracion B (3P - 1C)\n";
    cout << "3 - Configuracion C (3P - 3C)\n";
    cout << "4 - Prueba de Carga Masiva\n";
    cout << "5 - Prueba de Vacuidad\n";
    cout << "6 - Prueba de Saturacion\n";
    cout << "7 - Prueba de Equidad\n";
    cout << "0 - Salir\n";
    cout << "========================================\n";
    cout << "Seleccione una opcion: ";
}

int main(){
    srand(static_cast<unsigned>(time(nullptr)));
    int opcion;

    do {
        mostrarMenu();
        cin >> opcion;

        if(cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');

            cout << "\nOpcion invalida.\n";
            continue;
        }

        switch(opcion) {
        case 1: {
            cout << "\nEjecutando Configuracion A...\n";
            GestorRenderizado gestor;
            gestor.ejecutarConfiguracionA();
            break;
        }

        case 2: {
            cout << "\nEjecutando Configuracion B...\n";
            GestorRenderizado gestor;
            gestor.ejecutarConfiguracionB();
            break;
        }

        case 3: {
            cout << "\nEjecutando Configuracion C...\n";
            GestorRenderizado gestor;
            gestor.ejecutarConfiguracionC();
            break;
        }

        case 4: {
            cout << "\nEjecutando Prueba de Carga Masiva...\n";
            GestorRenderizado gestor;
            gestor.pruebaCargaMasiva();
            break;
        }

        case 5: {
            cout << "\nEjecutando Prueba de Vacuidad...\n";
            GestorRenderizado gestor;
            gestor.pruebaVacuidad();
            break;
        }

        case 6: {
            cout << "\nEjecutando Prueba de Saturacion...\n";
            GestorRenderizado gestor;
            gestor.pruebaSaturacion();
            break;
        }

        case 7: {
            cout << "\nEjecutando Prueba de Equidad...\n";
            GestorRenderizado gestor;
            gestor.pruebaEquidad();
            break;
        }

        case 0: {
            cout << "\nFinalizando programa...\n";
            break;
        }

        default:{
            cout << "\nOpcion inexistente.\n";
            break;
        }
        }

    }
    while(opcion != 0);
    return 0;
}
