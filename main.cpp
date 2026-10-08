#include "Metricas.h"
#include <iostream>

using namespace std;

int main() {
    string corta = "ana";
    string larga = "este texto es lo bastante largo para salir del buffer interno";

    Metricas::reiniciar();
    Metricas::registrarReserva(Metricas::bytesCadena(corta));
    Metricas::registrarReserva(Metricas::bytesCadena(larga));
    Metricas::registrarReserva(100);
    Metricas::registrarLiberacion(40);

    unsigned long local = 0;
    for (unsigned int i = 0; i < 10; ++i) ++local;
    Metricas::contar(local);
    Metricas::contar();
    Metricas::registrarExterno();
    Metricas::sumarLocal(sizeof(local) + sizeof(corta) + sizeof(larga));

    cout << "Bytes cadena corta: " << Metricas::bytesCadena(corta) << '\n';
    cout << "Bytes cadena larga: " << Metricas::bytesCadena(larga) << '\n';
    Metricas::reportar();

    Metricas::reiniciar();
    Metricas::reportar();
    return 0;
}
