#ifndef MENSAJE_H
#define MENSAJE_H

#include "lista.h"
#include "fecha.h"
#include <iostream>

class Mensaje {
public:
    static const unsigned int LONGITUD_MAXIMA = 512;

private:
    unsigned int emisor;     // id del usuario que lo envia
    unsigned int receptor;   // id del usuario que lo recibe (dueno del mensaje)
    Fecha fecha;             // fecha de envio
    Lista<char> texto;
    bool leido;

    static void escribirDosDigitos(std::ostream &, unsigned int);

public:
    Mensaje();
    Mensaje(unsigned int, unsigned int, const Fecha &, const Lista<char> &, bool = false);
    Mensaje(const Mensaje &);
    ~Mensaje();

    const Mensaje &operator=(const Mensaje &);
    bool operator<(const Mensaje &) const;

    unsigned int getEmisor() const;
    unsigned int getReceptor() const;
    const Fecha &getFecha() const;
    const Lista<char> &getTexto() const;
    bool estaLeido() const;

    void marcarLeido();
    bool enRango(const Fecha &, const Fecha &) const;
    void mostrar(std::ostream &) const;

    static bool textoValido(const Lista<char> &);
};

#endif
