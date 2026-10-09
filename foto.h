#ifndef FOTO_H
#define FOTO_H

#include "lista.h"
#include "fecha.h"

class Foto {
private:
    Lista<char> nombre;
    Fecha captura;
    unsigned int tamano;
    unsigned int likes;

public:
    Foto();
    Foto(const Lista<char> &, const Fecha &, unsigned int, unsigned int = 0);
    Foto(const Foto &);
    ~Foto();

    const Foto &operator=(const Foto &);

    const Lista<char> &getNombre() const;
    const Fecha &getCaptura() const;
    unsigned int getTamano() const;
    unsigned int getLikes() const;

    void darLike();
    void quitarLike();
};

#endif
