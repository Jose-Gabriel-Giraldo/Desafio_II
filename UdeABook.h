#ifndef UDEABOOK_H
#define UDEABOOK_H

#include <iostream>
#include <random>
#include "Lista.h"
#include "Usuario.h"
#include "RedAmistad.h"

using namespace std;

class UdeABook {
private:
    typedef std::mt19937 random;

    Lista<Usuario *> usuarios;
    Lista<Mensaje *> mensajes;
    RedAmistad red;
    random generador;
    Usuario *sesion;
    unsigned char cambios;

    static const char *const SUBRUTA;
    static const char *const DIRECTORIO;

public:
    static const unsigned char AMISTADES = 1;
    static const unsigned char SOLICITUDES = 2;
    static const unsigned char MENSAJES = 4;
    static const unsigned char ALBUMES = 8;

    UdeABook();
    UdeABook(const UdeABook &);
    ~UdeABook();

    const UdeABook &operator=(const UdeABook &);

    Usuario *getSesion() const;
    const Lista<Usuario *> &getUsuarios() const;
    const RedAmistad &getRed() const;
    unsigned char getCambios() const;
    static const char *getSubruta();
    static const char *getDirectorio();

    bool cargarDatos();
    bool guardarDatos();
    void ejecutar();

private:
    void liberar();
    void copiar(const UdeABook &);

    bool iniciarSesion(const Lista<char> &, const Lista<char> &);
    void cerrarSesion();
    void mostrarFotosAmigos();
    void menuAmistades();
    void mostrarAmigos() const;
    void visitarPerfil(const Lista<char> &);
    void verSugerencias(unsigned int);
    void menuMensajes();
    void enviarMensaje(const Lista<char> &, const Lista<char> &);
    void leerMensajes();
    void buscarMensajes(const Fecha &, const Fecha &);

    Usuario *buscarUsuario(const Lista<char> &) const;
    bool enviarSolicitud(Usuario &, Usuario &);
    void aceptarSolicitud(unsigned int);
    unsigned int elegirPonderado(Lista<unsigned int> &, Lista<unsigned int> &, unsigned int &);
    void carrusel(Usuario &, Album &, Lista<unsigned int> &);
    unsigned int cotaInferior(const Lista<Mensaje *> &, const Fecha &) const;
    void imprimirRuta(const Usuario &) const;
    void imprimirRuta(const Usuario &, const Album &, const Foto &) const;
    void imprimirMensaje(const Mensaje &) const;

    void anotarMensaje(const Mensaje &);
    void anotarLectura(unsigned int);
    void anotarPendientes(const Lista<unsigned int> &);
    void aplicarCambios();

    static Lista<char> ruta(const char *);
    static int comparar(const Lista<char> &, const Lista<char> &);
    static bool leer(istream &, Lista<char> &, char = '\n');
    static bool leer(istream &, unsigned long &);
    static bool leer(istream &, Fecha &);
    static unsigned long leerOpcion();
    static void escribir(ostream &, unsigned long, unsigned char = 0);
    static void escribir(ostream &, const Lista<char> &);
    static void escribir(ostream &, const Fecha &);
};

#endif
