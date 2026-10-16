#ifndef REDAMISTAD_H
#define REDAMISTAD_H

#include "Lista.h"

class RedAmistad {
private:
    unsigned long long *bits;
    unsigned int n;
    unsigned int palabrasFila;
    unsigned int amistades;

    static unsigned int contarBits(unsigned long long);
    static unsigned int bitMenor(unsigned long long);

public:
    RedAmistad();
    RedAmistad(unsigned int);
    RedAmistad(const RedAmistad &);
    ~RedAmistad();

    const RedAmistad &operator=(const RedAmistad &);

    unsigned int getN() const;
    unsigned int getPalabrasFila() const;
    unsigned int getAmistades() const;

    void inicializar(unsigned int);
    bool sonAmigos(unsigned int, unsigned int) const;
    void agregarAmistad(unsigned int, unsigned int);
    unsigned int numAmigos(unsigned int) const;
    unsigned int amigosEnComun(unsigned int, unsigned int) const;
    void amigosDe(unsigned int, Lista<unsigned int> &) const;
    void sugerencias(unsigned int, unsigned int, Lista<unsigned int> &, Lista<unsigned int> &) const;
};

#endif
