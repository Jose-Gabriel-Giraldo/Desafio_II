#ifndef MENSAJE_H
#define MENSAJE_H

#include "Lista.h"
#include "Fecha.h"

class Mensaje {
public:
    static const unsigned int LONGITUD_MAXIMA = 512;

private:
    Lista<char> texto;
    unsigned int emisor;
    unsigned int receptor;
    Fecha fecha;
    bool leido;

public:
    Mensaje();
    Mensaje(unsigned int, unsigned int, const Fecha &, const Lista<char> &, bool = false);
    Mensaje(const Mensaje &);
    ~Mensaje();

    const Mensaje &operator=(const Mensaje &);

    unsigned int getEmisor() const;
    unsigned int getReceptor() const;
    const Fecha &getFecha() const;
    const Lista<char> &getTexto() const;
    bool getLeido() const;

    void setEmisor(unsigned int);
    void setReceptor(unsigned int);
    void setFecha(const Fecha &);
    bool setTexto(const Lista<char> &);
    void setLeido(bool);

    static bool textoValido(const Lista<char> &);
};

#endif
