#ifndef METRICAS_H
#define METRICAS_H

class Metricas {
private:
    static unsigned long iteraciones;
    static unsigned long memoriaDinamica;
    static unsigned long memoriaLocal;
    static unsigned int usosExternos;

public:
    Metricas();
    Metricas(const Metricas &);
    ~Metricas();

    static unsigned long getIteraciones();
    static unsigned long getMemoriaDinamica();
    static unsigned long getMemoriaLocal();
    static unsigned int getUsosExternos();

    static void setIteraciones(unsigned long);
    static void setMemoriaDinamica(unsigned long);
    static void setMemoriaLocal(unsigned long);
    static void setUsosExternos(unsigned int);

    static void reiniciar();
    static void contar();
    static void contar(unsigned long);
    static void registrarReserva(unsigned long);
    static void registrarLiberacion(unsigned long);
    static void sumarLocal(unsigned long);
    static void registrarExterno();
    static void reportar();
};

#endif
