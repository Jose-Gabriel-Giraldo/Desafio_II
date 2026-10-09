#include "Fecha.h"
#include "Metricas.h"
#include <chrono>

Fecha::Fecha() : anio(1), mes(1), dia(1), hora(0), minuto(0) {}

Fecha::Fecha(unsigned short anio, unsigned char mes, unsigned char dia,
             unsigned char hora, unsigned char minuto)
    : anio(anio), mes(mes), dia(dia), hora(hora), minuto(minuto) {}

Fecha::Fecha(const Fecha &otra)
    : anio(otra.anio), mes(otra.mes), dia(otra.dia),
    hora(otra.hora), minuto(otra.minuto) {}

Fecha::~Fecha() {}

const Fecha &Fecha::operator=(const Fecha &otra) {
    if (this != &otra) {
        anio = otra.anio;
        mes = otra.mes;
        dia = otra.dia;
        hora = otra.hora;
        minuto = otra.minuto;
    }
    return *this;
}

unsigned short Fecha::getAnio() const { return anio; }
unsigned char Fecha::getMes() const { return mes; }
unsigned char Fecha::getDia() const { return dia; }
unsigned char Fecha::getHora() const { return hora; }
unsigned char Fecha::getMinuto() const { return minuto; }

void Fecha::setAnio(unsigned short valor) { anio = valor; }
void Fecha::setMes(unsigned char valor) { mes = valor; }
void Fecha::setDia(unsigned char valor) { dia = valor; }
void Fecha::setHora(unsigned char valor) { hora = valor; }
void Fecha::setMinuto(unsigned char valor) { minuto = valor; }

bool Fecha::esBisiesto(unsigned short a) {
    return (a % 4 == 0 && a % 100 != 0) || a % 400 == 0;
}

unsigned char Fecha::diasDelMes(unsigned short a, unsigned char m) {
    if (m == 2) return esBisiesto(a) ? 29 : 28;
    if (m == 4 || m == 6 || m == 9 || m == 11) return 30;
    return 31;
}

bool Fecha::esValida() const {
    if (anio < 1 || mes < 1 || mes > 12) return false;
    if (dia < 1 || dia > diasDelMes(anio, mes)) return false;
    return hora < 24 && minuto < 60;
}

bool Fecha::operator<(const Fecha &otra) const {
    if (anio != otra.anio) return anio < otra.anio;
    if (mes != otra.mes) return mes < otra.mes;
    if (dia != otra.dia) return dia < otra.dia;
    if (hora != otra.hora) return hora < otra.hora;
    return minuto < otra.minuto;
}

Fecha Fecha::ahora() {
    Metricas::registrarExterno();
    long long ticks = std::chrono::system_clock::now().time_since_epoch().count();
    long long segundos = ticks * std::chrono::system_clock::period::num
                         / std::chrono::system_clock::period::den;

    segundos += ZONA_HORARIA * 3600LL;
    long long dias = segundos / 86400;
    long long resto = segundos % 86400;

    unsigned short a = 1970;
    unsigned char m = 1;
    unsigned long iteraciones = 0;
    while (dias >= (esBisiesto(a) ? 366 : 365)) {
        dias -= esBisiesto(a) ? 366 : 365;
        ++a;
        ++iteraciones;
    }
    while (dias >= diasDelMes(a, m)) {
        dias -= diasDelMes(a, m);
        ++m;
        ++iteraciones;
    }
    Metricas::contar(iteraciones);

    return Fecha(a, m, dias + 1, resto / 3600, resto % 3600 / 60);
}
