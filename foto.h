#ifndef FOTO_H
#define FOTO_H

#include "Lista.h"
#include "Fecha.h"

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

    void setNombre(const Lista<char> &);
    void setCaptura(const Fecha &);
    void setTamano(unsigned int);
    void setLikes(unsigned int);

    void darLike();
    void quitarLike();
};

#endif
