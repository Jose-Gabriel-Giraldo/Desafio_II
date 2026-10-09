#ifndef LISTA_H
#define LISTA_H

#include "Metricas.h"

template <typename T>
class Lista {
private:
    static const unsigned int CAPACIDAD_INICIAL = 4;

    T *datos;
    unsigned int tam;
    unsigned int capacidad;

    void redimensionar(unsigned int);

public:
    Lista();
    Lista(const Lista<T> &);
    ~Lista();

    const Lista<T> &operator=(const Lista<T> &);
    T &operator[](unsigned int);
    const T &operator[](unsigned int) const;

    unsigned int getTam() const;
    unsigned int getCapacidad() const;

    void reservar(unsigned int);
    void asignar(const T *, unsigned int);
    void agregar(const T &);
    void eliminar(unsigned int);
    bool contiene(const T &) const;
};

template <typename T>
Lista<T>::Lista() : datos(nullptr), tam(0), capacidad(0) {}

template <typename T>
Lista<T>::Lista(const Lista<T> &otra) : datos(nullptr), tam(0), capacidad(0) {
    asignar(otra.datos, otra.tam);
}

template <typename T>
Lista<T>::~Lista() {
    delete[] datos;
    Metricas::registrarLiberacion(capacidad * sizeof(T));
}

template <typename T>
const Lista<T> &Lista<T>::operator=(const Lista<T> &otra) {
    if (this != &otra) asignar(otra.datos, otra.tam);
    return *this;
}

template <typename T>
T &Lista<T>::operator[](unsigned int i) { return datos[i]; }

template <typename T>
const T &Lista<T>::operator[](unsigned int i) const { return datos[i]; }

template <typename T>
unsigned int Lista<T>::getTam() const { return tam; }

template <typename T>
unsigned int Lista<T>::getCapacidad() const { return capacidad; }

template <typename T>
void Lista<T>::redimensionar(unsigned int nueva) {
    T *nuevo = new T[nueva];
    for (unsigned int i = 0; i < tam; ++i) nuevo[i] = datos[i];
    Metricas::contar(tam);
    delete[] datos;
    Metricas::registrarLiberacion(capacidad * sizeof(T));
    Metricas::registrarReserva(nueva * sizeof(T));
    datos = nuevo;
    capacidad = nueva;
}

template <typename T>
void Lista<T>::reservar(unsigned int n) {
    if (n > capacidad) redimensionar(n);
}

template <typename T>
void Lista<T>::asignar(const T *origen, unsigned int n) {
    if (n > capacidad) {
        tam = 0;
        redimensionar(n);
    }
    for (unsigned int i = 0; i < n; ++i) datos[i] = origen[i];
    Metricas::contar(n);
    tam = n;
}

template <typename T>
void Lista<T>::agregar(const T &elemento) {
    if (tam < capacidad) {
        datos[tam++] = elemento;
        return;
    }
    T copia = elemento;
    redimensionar(capacidad == 0 ? CAPACIDAD_INICIAL : capacidad * 2);
    datos[tam++] = copia;
}

template <typename T>
void Lista<T>::eliminar(unsigned int i) {
    if (i >= tam) return;
    datos[i] = datos[--tam];
}

template <typename T>
bool Lista<T>::contiene(const T &elemento) const {
    unsigned int i = 0;
    while (i < tam && !(datos[i] == elemento)) ++i;
    Metricas::contar(i);
    return i < tam;
}

#endif
