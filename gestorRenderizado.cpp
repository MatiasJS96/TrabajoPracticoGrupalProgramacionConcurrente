#include "GestorRenderizado.h"
#include <iostream>
#include <vector>
#include <memory>

GestorRenderizado::GestorRenderizado()
    : jobsFinalizados(0)
{
}

void GestorRenderizado::ejecutar(
    int productores,
    int consumidores,
    int trabajos
){
    ejecutarEscenario(
        productores,
        consumidores,
        trabajos
    );
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

    int cantidad = trabajosPorProductor;

    // El último productor genera los trabajos sobrantes
    if(i == productores - 1) {
        cantidad += trabajos % productores;
    }

    listaProductores.push_back(
        std::make_unique<Productor>(i + 1,cantidad,cola,logger));
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

    /*Avisar a la cola que no se produciran mas trabajos.*/
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

