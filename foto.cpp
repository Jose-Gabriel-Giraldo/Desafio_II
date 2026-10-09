#include "Foto.h"

Foto::Foto() : nombre(), captura(), tamano(0), likes(0) {}

Foto::Foto(const Lista<char> &nombre, const Fecha &captura,
           unsigned int tamano, unsigned int likes)
    : nombre(nombre), captura(captura), tamano(tamano), likes(likes) {}

Foto::Foto(const Foto &otra)
    : nombre(otra.nombre), captura(otra.captura),
    tamano(otra.tamano), likes(otra.likes) {}

Foto::~Foto() {}

const Foto &Foto::operator=(const Foto &otra) {
    if (this != &otra) {
        nombre = otra.nombre;
        captura = otra.captura;
        tamano = otra.tamano;
        likes = otra.likes;
    }
    return *this;
}

const Lista<char> &Foto::getNombre() const { return nombre; }
const Fecha &Foto::getCaptura() const { return captura; }
unsigned int Foto::getTamano() const { return tamano; }
unsigned int Foto::getLikes() const { return likes; }

void Foto::setNombre(const Lista<char> &valor) { nombre = valor; }
void Foto::setCaptura(const Fecha &valor) { captura = valor; }
void Foto::setTamano(unsigned int valor) { tamano = valor; }
void Foto::setLikes(unsigned int valor) { likes = valor; }

void Foto::darLike() { ++likes; }

void Foto::quitarLike() {
    if (likes > 0) --likes;
}
