#include "Mensaje.h"

Mensaje::Mensaje() : texto(), emisor(0), receptor(0), fecha(), leido(false) {}

Mensaje::Mensaje(unsigned int emisor, unsigned int receptor, const Fecha &fecha,
                 const Lista<char> &texto, bool leido)
    : texto(texto), emisor(emisor), receptor(receptor), fecha(fecha), leido(leido) {}

Mensaje::Mensaje(const Mensaje &otro)
    : texto(otro.texto), emisor(otro.emisor), receptor(otro.receptor),
    fecha(otro.fecha), leido(otro.leido) {}

Mensaje::~Mensaje() {}

const Mensaje &Mensaje::operator=(const Mensaje &otro) {
    if (this != &otro) {
        emisor = otro.emisor;
        receptor = otro.receptor;
        fecha = otro.fecha;
        texto = otro.texto;
        leido = otro.leido;
    }
    return *this;
}

unsigned int Mensaje::getEmisor() const { return emisor; }
unsigned int Mensaje::getReceptor() const { return receptor; }
const Fecha &Mensaje::getFecha() const { return fecha; }
const Lista<char> &Mensaje::getTexto() const { return texto; }
bool Mensaje::getLeido() const { return leido; }

void Mensaje::setEmisor(unsigned int valor) { emisor = valor; }
void Mensaje::setReceptor(unsigned int valor) { receptor = valor; }
void Mensaje::setFecha(const Fecha &valor) { fecha = valor; }
void Mensaje::setLeido(bool valor) { leido = valor; }

bool Mensaje::setTexto(const Lista<char> &valor) {
    if (!textoValido(valor)) return false;
    texto = valor;
    return true;
}

bool Mensaje::textoValido(const Lista<char> &t) {
    unsigned int n = t.getTam();
    if (n == 0 || n > LONGITUD_MAXIMA) return false;

    unsigned int i = 0;
    while (i < n && (t[i] == ' ' || t[i] == '\t' || t[i] == '\n' || t[i] == '\r')) ++i;
    Metricas::contar(i < n ? i + 1 : i);
    return i < n;
}
