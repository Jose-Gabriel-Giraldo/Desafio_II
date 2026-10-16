#include "Album.h"

Album::Album() : nombre(), privacidad(PUBLICO), fotos() {}

Album::Album(const Lista<char> &nombre, unsigned char privacidad)
    : nombre(nombre), privacidad(privacidad <= AMIGOS ? privacidad : PRIVADO), fotos() {}

Album::Album(const Album &otro)
    : nombre(otro.nombre), privacidad(otro.privacidad), fotos() {
    for (unsigned int i = 0; i < otro.fotos.getTam(); ++i)
        agregarFoto(new Foto(*otro.fotos[i]));
    Metricas::contar(otro.fotos.getTam());
}

Album::~Album() {
    for (unsigned int i = 0; i < fotos.getTam(); ++i) {
        delete fotos[i];
        Metricas::registrarLiberacion(sizeof(Foto));
    }
    Metricas::contar(fotos.getTam());
}

const Album &Album::operator=(const Album &otro) {
    if (this != &otro) {
        for (unsigned int i = 0; i < fotos.getTam(); ++i) {
            delete fotos[i];
            Metricas::registrarLiberacion(sizeof(Foto));
        }
        Metricas::contar(fotos.getTam());
        while (fotos.getTam() > 0) fotos.eliminar(fotos.getTam() - 1);

        nombre = otro.nombre;
        privacidad = otro.privacidad;
        for (unsigned int i = 0; i < otro.fotos.getTam(); ++i)
            agregarFoto(new Foto(*otro.fotos[i]));
        Metricas::contar(otro.fotos.getTam());
    }
    return *this;
}

const Lista<char> &Album::getNombre() const { return nombre; }
unsigned char Album::getPrivacidad() const { return privacidad; }
const Lista<Foto *> &Album::getFotos() const { return fotos; }

void Album::setNombre(const Lista<char> &valor) { nombre = valor; }
void Album::setPrivacidad(unsigned char valor) {
    if (valor <= AMIGOS) privacidad = valor;
}

void Album::agregarFoto(Foto *foto) {
    fotos.agregar(foto);
    Metricas::registrarReserva(sizeof(Foto));
}
