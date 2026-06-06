#include "GestorRenderizado.h"
#include <iostream>
#include <vector>
#include <memory>

GestorRenderizado::GestorRenderizado()
    : jobsFinalizados(0)
{
}

void GestorRenderizado::ejecutarEscenario(int productores,int consumidores,int trabajos) {
    jobsFinalizados = 0;

    std::vector<std::unique_ptr<Productor>> listaProductores;
    std::vector<std::unique_ptr<Consumidor>> listaConsumidores;

    int trabajosPorProductor = 0;

    if(productores > 0){
        trabajosPorProductor = trabajos / productores;
    }

    std::cout << "\n=====================================\n";
    std::cout << "INICIANDO ESCENARIO\n";
    std::cout << "Productores : " << productores << "\n";
    std::cout << "Consumidores: " << consumidores << "\n";
    std::cout << "Trabajos    : " << trabajos << "\n";
    std::cout << "=====================================\n";

    /*Crear productores*/
    for(int i = 0; i < productores; i++) {
        listaProductores.push_back(std::make_unique<Productor>(i + 1,trabajosPorProductor,cola,logger));
    }

    /*Crear consumidores*/
    for(int i = 0; i < consumidores; i++) {
    listaConsumidores.push_back(
        std::make_unique<Consumidor>(i + 1,cola,pool,logger,jobsFinalizados,mutexContador));
}

    
    for(auto& consumidor : listaConsumidores){
        consumidor->iniciar();
    }

    /* Iniciar productores*/
    for(auto& productor : listaProductores) {
        productor->iniciar();
    }

    /*
        Esperar productores
    */
    for(auto& productor : listaProductores)
    {
        productor->esperar();
    }

    /*Avisar a la cola que no se producir�n m�s trabajos.*/
    cola.finalizar();

    /*Esperar consumidores*/
    for(auto& consumidor : listaConsumidores){
        consumidor->esperar();
    }

    std::cout << "\n=====================================\n";
    std::cout << "ESCENARIO FINALIZADO\n";
    std::cout << "Jobs finalizados: "
              << jobsFinalizados
              << "\n";
    std::cout << "=====================================\n";
}

void GestorRenderizado::ejecutarConfiguracionA(){

    std::cout
        << "\n\n========== CONFIGURACION A ==========\n"
        << "1 Productor\n"
        << "2 Consumidores\n"
        << "100 Jobs\n"
        << "=====================================\n";

    ejecutarEscenario(
        1,
        2,
        100
    );
}

void GestorRenderizado::ejecutarConfiguracionB() {

    std::cout
        << "\n\n========== CONFIGURACION B ==========\n"
        << "3 Productores\n"
        << "1 Consumidor\n"
        << "100 Jobs\n"
        << "=====================================\n";

    ejecutarEscenario(
        3,
        1,
        100
    );
}

void GestorRenderizado::ejecutarConfiguracionC() {
    std::cout
        << "\n\n========== CONFIGURACION C ==========\n"
        << "3 Productores\n"
        << "3 Consumidores\n"
        << "100 Jobs\n"
        << "=====================================\n";

    ejecutarEscenario(
        3,
        3,
        100
    );
}

void GestorRenderizado::pruebaCargaMasiva() {
    std::cout
        << "\n\n======= PRUEBA DE CARGA MASIVA =======\n"
        << "1500 Jobs\n"
        << "3 Productores\n"
        << "3 Consumidores\n"
        << "======================================\n";

    ejecutarEscenario(
        3,
        3,
        1500
    );
}

void GestorRenderizado::pruebaVacuidad()
{
    std::cout
        << "\n\n========= PRUEBA DE VACUIDAD =========\n"
        << "0 Jobs\n"
        << "======================================\n";

    ejecutarEscenario(
        1,
        1,
        0
    );
}

void GestorRenderizado::pruebaSaturacion() {
    std::cout
        << "\n\n======== PRUEBA DE SATURACION ========\n"
        << "8 Jobs\n"
        << "3 Consumidores\n"
        << "Pool VRAM = 5 Slots\n"
        << "======================================\n";

    ejecutarEscenario(
        1,
        3,
        8
    );
}

void GestorRenderizado::pruebaEquidad() {
    std::cout
        << "\n\n========== PRUEBA EQUIDAD ============\n"
        << "Verificacion de Anti-Starvation\n"
        << "======================================\n";

    ejecutarEscenario(
        3,
        3,
        200
    );
}
