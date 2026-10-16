#ifndef FECHA_H
#define FECHA_H

class Fecha {
private:
    unsigned short anio;
    unsigned char mes;
    unsigned char dia;
    unsigned char hora;
    unsigned char minuto;

    static const int ZONA_HORARIA = -5;

    static bool esBisiesto(unsigned short);
    static unsigned char diasDelMes(unsigned short, unsigned char);

public:
    Fecha();
    Fecha(unsigned short, unsigned char, unsigned char,
          unsigned char = 0, unsigned char = 0);
    Fecha(const Fecha &);
    ~Fecha();

    const Fecha &operator=(const Fecha &);

    unsigned short getAnio() const;
    unsigned char getMes() const;
    unsigned char getDia() const;
    unsigned char getHora() const;
    unsigned char getMinuto() const;

    void setAnio(unsigned short);
    void setMes(unsigned char);
    void setDia(unsigned char);
    void setHora(unsigned char);
    void setMinuto(unsigned char);

    static Fecha ahora();
    bool esValida() const;
    bool operator<(const Fecha &) const;
};

#endif
