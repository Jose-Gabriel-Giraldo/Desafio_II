#include "Metricas.h"
#include <iostream>

using namespace std;

unsigned long Metricas::iteraciones = 0;
unsigned long Metricas::memoriaDinamica = 0;
unsigned long Metricas::memoriaLocal = 0;
unsigned int Metricas::usosExternos = 0;

Metricas::Metricas() {}

Metricas::Metricas(const Metricas &) {}

Metricas::~Metricas() {}

unsigned long Metricas::getIteraciones() { return iteraciones; }
unsigned long Metricas::getMemoriaDinamica() { return memoriaDinamica; }
unsigned long Metricas::getMemoriaLocal() { return memoriaLocal; }
unsigned int Metricas::getUsosExternos() { return usosExternos; }

void Metricas::setIteraciones(unsigned long valor) { iteraciones = valor; }
void Metricas::setMemoriaDinamica(unsigned long valor) { memoriaDinamica = valor; }
void Metricas::setMemoriaLocal(unsigned long valor) { memoriaLocal = valor; }
void Metricas::setUsosExternos(unsigned int valor) { usosExternos = valor; }

void Metricas::reiniciar() {
    iteraciones = 0;
    memoriaLocal = 0;
    usosExternos = 0;
}

void Metricas::contar() { ++iteraciones; }

void Metricas::contar(unsigned long cantidad) { iteraciones += cantidad; }

void Metricas::registrarReserva(unsigned long bytes) { memoriaDinamica += bytes; }

void Metricas::registrarLiberacion(unsigned long bytes) {
    memoriaDinamica = bytes > memoriaDinamica ? 0 : memoriaDinamica - bytes;
}

void Metricas::sumarLocal(unsigned long bytes) { memoriaLocal += bytes; }

void Metricas::registrarExterno() { ++usosExternos; }

void Metricas::reportar() {
    unsigned long propia = 3 * sizeof(unsigned long) + sizeof(unsigned int);
    unsigned long total = memoriaDinamica + memoriaLocal + propia;

    cout << "\n------ Consumo de recursos ------\n";
    cout << "Iteraciones: " << iteraciones << '\n';
    cout << "Componentes externos invocados: " << usosExternos << '\n';
    cout << "Memoria total: " << total << " bytes\n";
    cout << "  Estructuras y objetos: " << memoriaDinamica + propia << " bytes\n";
    cout << "  Variables locales y parametros: " << memoriaLocal << " bytes\n";
    cout << "---------------------------------\n";
}
