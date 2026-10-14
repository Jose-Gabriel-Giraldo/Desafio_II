#include "Usuario.h"

Usuario::Usuario()
    : nombre(), clave(), correo(), ciudad(), pais(), albumes(), enviados(), recibidos(),
    solicitudes(), id(0), primerNoLeido(0), nacimiento(), inscripcion() {}

Usuario::Usuario(unsigned int id, const Lista<char> &nombre, const Lista<char> &clave,
                 const Lista<char> &correo, const Lista<char> &ciudad, const Lista<char> &pais,
                 const Fecha &nacimiento, const Fecha &inscripcion)
    : nombre(nombre), clave(clave), correo(correo), ciudad(ciudad), pais(pais), albumes(),
    enviados(), recibidos(), solicitudes(), id(id), primerNoLeido(0),
    nacimiento(nacimiento), inscripcion(inscripcion) {}

Usuario::Usuario(const Usuario &otro)
    : nombre(otro.nombre), clave(otro.clave), correo(otro.correo), ciudad(otro.ciudad),
    pais(otro.pais), albumes(otro.albumes), enviados(), recibidos(), solicitudes(otro.solicitudes),
    id(otro.id), primerNoLeido(0), nacimiento(otro.nacimiento), inscripcion(otro.inscripcion) {
    for (unsigned int i = 0; i < albumes.getTam(); ++i) albumes[i] = new Album(*albumes[i]);
    Metricas::registrarReserva(albumes.getTam() * sizeof(Album));
    Metricas::contar(albumes.getTam());
}

Usuario::~Usuario() {
    for (unsigned int i = 0; i < albumes.getTam(); ++i) delete albumes[i];
    for (unsigned int i = 0; i < recibidos.getTam(); ++i) delete recibidos[i];
    Metricas::registrarLiberacion(albumes.getTam() * sizeof(Album) + recibidos.getTam() * sizeof(Mensaje));
    Metricas::contar(albumes.getTam() + recibidos.getTam());
}

const Usuario &Usuario::operator=(const Usuario &otro) {
    if (this != &otro) {
        for (unsigned int i = 0; i < albumes.getTam(); ++i) delete albumes[i];
        for (unsigned int i = 0; i < recibidos.getTam(); ++i) delete recibidos[i];
        Metricas::registrarLiberacion(albumes.getTam() * sizeof(Album) + recibidos.getTam() * sizeof(Mensaje));
        Metricas::contar(albumes.getTam() + recibidos.getTam());
        recibidos = Lista<Mensaje *>();
        enviados = Lista<Mensaje *>();

        nombre = otro.nombre;
        clave = otro.clave;
        correo = otro.correo;
        ciudad = otro.ciudad;
        pais = otro.pais;
        solicitudes = otro.solicitudes;
        id = otro.id;
        primerNoLeido = 0;
        nacimiento = otro.nacimiento;
        inscripcion = otro.inscripcion;
        albumes = otro.albumes;
        for (unsigned int i = 0; i < albumes.getTam(); ++i) albumes[i] = new Album(*albumes[i]);
        Metricas::registrarReserva(albumes.getTam() * sizeof(Album));
        Metricas::contar(albumes.getTam());
    }
    return *this;
}

unsigned int Usuario::getId() const { return id; }
unsigned int Usuario::getPrimerNoLeido() const { return primerNoLeido; }
const Lista<char> &Usuario::getNombre() const { return nombre; }
const Lista<char> &Usuario::getCorreo() const { return correo; }
const Lista<char> &Usuario::getCiudad() const { return ciudad; }
const Lista<char> &Usuario::getPais() const { return pais; }
const Fecha &Usuario::getNacimiento() const { return nacimiento; }
const Fecha &Usuario::getInscripcion() const { return inscripcion; }
const Lista<Album *> &Usuario::getAlbumes() const { return albumes; }
const Lista<Mensaje *> &Usuario::getEnviados() const { return enviados; }
const Lista<Mensaje *> &Usuario::getRecibidos() const { return recibidos; }
const Lista<unsigned int> &Usuario::getSolicitudes() const { return solicitudes; }

void Usuario::setId(unsigned int valor) { id = valor; }
void Usuario::setNombre(const Lista<char> &valor) { nombre = valor; }
void Usuario::setClave(const Lista<char> &valor) { clave = valor; }
void Usuario::setCorreo(const Lista<char> &valor) { correo = valor; }
void Usuario::setCiudad(const Lista<char> &valor) { ciudad = valor; }
void Usuario::setPais(const Lista<char> &valor) { pais = valor; }
void Usuario::setNacimiento(const Fecha &valor) { nacimiento = valor; }
void Usuario::setInscripcion(const Fecha &valor) { inscripcion = valor; }

bool Usuario::verificarClave(const Lista<char> &intento) const {
    if (clave.getTam() != intento.getTam()) return false;
    unsigned int i = 0;
    while (i < clave.getTam() && clave[i] == intento[i]) ++i;
    Metricas::contar(i);
    return i == clave.getTam();
}

void Usuario::agregarAlbum(Album *album) {
    albumes.agregar(album);
    Metricas::registrarReserva(sizeof(Album));
}

void Usuario::agregarEnviado(Mensaje *mensaje) { enviados.agregar(mensaje); }

void Usuario::agregarRecibido(Mensaje *mensaje) {
    recibidos.agregar(mensaje);
    Metricas::registrarReserva(sizeof(Mensaje));
    if (mensaje->getLeido()) primerNoLeido = recibidos.getTam();
}

void Usuario::marcarLeidos() {
    for (unsigned int i = primerNoLeido; i < recibidos.getTam(); ++i) recibidos[i]->setLeido(true);
    Metricas::contar(recibidos.getTam() - primerNoLeido);
    primerNoLeido = recibidos.getTam();
}

void Usuario::agregarSolicitud(unsigned int idOrigen) { solicitudes.agregar(idOrigen); }

void Usuario::eliminarSolicitud(unsigned int posicion) { solicitudes.eliminar(posicion); }
