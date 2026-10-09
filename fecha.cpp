#include "fecha.h"
#include "metricas.h"
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
    long long segundos = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    segundos += ZONA_HORARIA * 3600LL;

    long long dias = segundos / 86400;
    long long resto = segundos % 86400;
    if (resto < 0) {
        resto += 86400;
        --dias;
    }

    dias += 719468;
    long long era = (dias >= 0 ? dias : dias - 146096) / 146097;
    long long diaEra = dias - era * 146097;
    long long anioEra = (diaEra - diaEra / 1460 + diaEra / 36524 - diaEra / 146096) / 365;
    long long diaAnio = diaEra - (365 * anioEra + anioEra / 4 - anioEra / 100);
    long long mesMarzo = (5 * diaAnio + 2) / 153;
    long long d = diaAnio - (153 * mesMarzo + 2) / 5 + 1;
    long long m = mesMarzo < 10 ? mesMarzo + 3 : mesMarzo - 9;
    long long a = anioEra + era * 400 + (m <= 2 ? 1 : 0);

    return Fecha(static_cast<unsigned short>(a), static_cast<unsigned char>(m),
                 static_cast<unsigned char>(d),
                 static_cast<unsigned char>(resto / 3600),
                 static_cast<unsigned char>((resto % 3600) / 60));
}
