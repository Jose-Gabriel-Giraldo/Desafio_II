#include "mensaje.h"

Mensaje::Mensaje() : emisor(0), receptor(0), fecha(), texto(), leido(false) {}

Mensaje::Mensaje(unsigned int emisor, unsigned int receptor, const Fecha &fecha,
                 const Lista<char> &texto, bool leido)
    : emisor(emisor), receptor(receptor), fecha(fecha), texto(texto), leido(leido) {}

// Copia profunda: Lista<char> ya copia su propio arreglo de caracteres.
Mensaje::Mensaje(const Mensaje &otro)
    : emisor(otro.emisor), receptor(otro.receptor), fecha(otro.fecha),
    texto(otro.texto), leido(otro.leido) {}

// texto es una Lista<char>: su propio destructor libera la memoria.
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

// Un mensaje es "menor" si se envio antes. Se apoya en Fecha::operator<.
bool Mensaje::operator<(const Mensaje &otro) const { return fecha < otro.fecha; }

unsigned int Mensaje::getEmisor() const { return emisor; }
unsigned int Mensaje::getReceptor() const { return receptor; }
const Fecha &Mensaje::getFecha() const { return fecha; }
const Lista<char> &Mensaje::getTexto() const { return texto; }
bool Mensaje::estaLeido() const { return leido; }

void Mensaje::marcarLeido() { leido = true; }

// Busqueda por rango (ambas fechas inclusive). Se compara solo por dia:
// el rango va desde las 00:00 de "desde" hasta las 23:59 de "hasta",
// sin importar la hora que traigan las fechas recibidas.
bool Mensaje::enRango(const Fecha &desde, const Fecha &hasta) const {
    Fecha inicio(desde.getAnio(), desde.getMes(), desde.getDia(), 0, 0);
    Fecha fin(hasta.getAnio(), hasta.getMes(), hasta.getDia(), 23, 59);
    return !(fecha < inicio) && !(fin < fecha);
}

// Formato: [dd/mm/aaaa hh:mm] texto
// El texto se escribe caracter por caracter porque es una Lista<char>.
void Mensaje::mostrar(std::ostream &salida) const {
    salida << '[';
    escribirDosDigitos(salida, fecha.getDia());
    salida << '/';
    escribirDosDigitos(salida, fecha.getMes());
    salida << '/' << fecha.getAnio() << ' ';
    escribirDosDigitos(salida, fecha.getHora());
    salida << ':';
    escribirDosDigitos(salida, fecha.getMinuto());
    salida << "] ";

    unsigned int n = texto.getTam();
    for (unsigned int i = 0; i < n; ++i) salida << texto[i];
    Metricas::contar(n);
    salida << '\n';
}

// unsigned char se imprimiria como caracter, por eso se pasa como unsigned int.
void Mensaje::escribirDosDigitos(std::ostream &salida, unsigned int valor) {
    if (valor < 10) salida << '0';
    salida << valor;
}

// Reglas del enunciado: no vacio y maximo 512 caracteres.
// Ademas se rechaza un texto hecho solo de espacios, tabs o saltos de linea.
// La longitud se revisa primero (O(1)) y el recorrido termina apenas encuentra
// un caracter visible, asi un texto normal cuesta 1 o 2 iteraciones.
bool Mensaje::textoValido(const Lista<char> &t) {
    unsigned int n = t.getTam();
    if (n == 0 || n > LONGITUD_MAXIMA) return false;

    unsigned int i = 0;
    while (i < n && (t[i] == ' ' || t[i] == '\t' || t[i] == '\n' || t[i] == '\r')) ++i;
    Metricas::contar(i + 1);
    return i < n;
}
