#include "GestorRenderizado.h"
#include <iostream>
using namespace std;

int main(){
    int cfg,prueba;
    cout<<"===== SISTEMA DE GESTION DE GRANJA DE RENDER =====\n";
    cout<<"Configuracion:\n";
    cout<<"1 - A (1P - 2C)\n";
    cout<<"2 - B (3P - 1C)\n";
    cout<<"3 - C (3P - 3C)\n";
    cout<<"Opcion: ";
    cin>>cfg;

    cout<<"\nEscenario:\n";
    cout<<"1 - Carga Masiva (1500 jobs)\n";
    cout<<"2 - Vacuidad (0 jobs)\n";
    cout<<"3 - Saturacion (8 jobs)\n";
    cout<<"4 - Equidad (200 jobs)\n";
    cout<<"Opcion: ";
    cin>>prueba;

    int productores=1,consumidores=2,trabajos=0;

    if(cfg==1){productores=1; consumidores=2;}
    if(cfg==2){productores=3; consumidores=1;}
    if(cfg==3){productores=3; consumidores=3;}

    if(prueba==1) trabajos=1500;
    if(prueba==2) trabajos=0;
    if(prueba==3) trabajos=8;
    if(prueba==4) trabajos=200;

    GestorRenderizado gestor;
    gestor.ejecutar(productores,consumidores,trabajos);

    return 0;
}
