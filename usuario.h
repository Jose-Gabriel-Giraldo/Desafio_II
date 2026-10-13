#ifndef USUARIO_H
#define USUARIO_H

#include "Lista.h"
#include "Fecha.h"
#include "Album.h"
#include "Mensaje.h"

class Usuario {
private:
    Lista<char> nombre;
    Lista<char> clave;
    Lista<char> correo;
    Lista<char> ciudad;
    Lista<char> pais;
    Lista<Album *> albumes;
    Lista<Mensaje *> enviados;
    Lista<Mensaje *> recibidos;
    Lista<unsigned int> solicitudes;
    unsigned int id;
    unsigned int primerNoLeido;
    Fecha nacimiento;
    Fecha inscripcion;

public:
    Usuario();
    Usuario(unsigned int, const Lista<char> &, const Lista<char> &, const Lista<char> &,
            const Lista<char> &, const Lista<char> &, const Fecha &, const Fecha &);
    Usuario(const Usuario &);
    ~Usuario();

    const Usuario &operator=(const Usuario &);

    unsigned int getId() const;
    unsigned int getPrimerNoLeido() const;
    const Lista<char> &getNombre() const;
    const Lista<char> &getCorreo() const;
    const Lista<char> &getCiudad() const;
    const Lista<char> &getPais() const;
    const Fecha &getNacimiento() const;
    const Fecha &getInscripcion() const;
    const Lista<Album *> &getAlbumes() const;
    const Lista<Mensaje *> &getEnviados() const;
    const Lista<Mensaje *> &getRecibidos() const;
    const Lista<unsigned int> &getSolicitudes() const;

    void setId(unsigned int);
    void setNombre(const Lista<char> &);
    void setClave(const Lista<char> &);
    void setCorreo(const Lista<char> &);
    void setCiudad(const Lista<char> &);
    void setPais(const Lista<char> &);
    void setNacimiento(const Fecha &);
    void setInscripcion(const Fecha &);

    bool verificarClave(const Lista<char> &) const;
    void agregarAlbum(Album *);
    void agregarEnviado(Mensaje *);
    void agregarRecibido(Mensaje *);
    void marcarLeidos();
    void agregarSolicitud(unsigned int);
    void eliminarSolicitud(unsigned int);
};

#endif
