#ifndef ALBUM_H
#define ALBUM_H

#include "Lista.h"
#include "Foto.h"

class Album {
public:
    static const unsigned char PUBLICO = 0;
    static const unsigned char PRIVADO = 1;
    static const unsigned char AMIGOS = 2;

private:
    Lista<char> nombre;
    unsigned char privacidad;
    Lista<Foto *> fotos;

public:
    Album();
    Album(const Lista<char> &, unsigned char);
    Album(const Album &);
    ~Album();

    const Album &operator=(const Album &);

    const Lista<char> &getNombre() const;
    unsigned char getPrivacidad() const;
    const Lista<Foto *> &getFotos() const;

    void setNombre(const Lista<char> &);
    void setPrivacidad(unsigned char);

    void agregarFoto(Foto *);
};

#endif
