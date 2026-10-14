#include "RedAmistad.h"

RedAmistad::RedAmistad() : bits(nullptr), n(0), palabrasFila(0) {}

RedAmistad::RedAmistad(unsigned int usuarios) : bits(nullptr), n(0), palabrasFila(0) {
    inicializar(usuarios);
}

RedAmistad::RedAmistad(const RedAmistad &otra) : bits(nullptr), n(0), palabrasFila(0) {
    *this = otra;
}

RedAmistad::~RedAmistad() {
    delete[] bits;
    Metricas::registrarLiberacion((unsigned long)n * palabrasFila * sizeof(unsigned long long));
}

const RedAmistad &RedAmistad::operator=(const RedAmistad &otra) {
    if (this != &otra) {
        inicializar(otra.n);
        unsigned long total = (unsigned long)n * palabrasFila;
        for (unsigned long i = 0; i < total; ++i) bits[i] = otra.bits[i];
        Metricas::contar(total);
    }
    return *this;
}

unsigned int RedAmistad::getN() const { return n; }
unsigned int RedAmistad::getPalabrasFila() const { return palabrasFila; }

void RedAmistad::inicializar(unsigned int usuarios) {
    delete[] bits;
    Metricas::registrarLiberacion((unsigned long)n * palabrasFila * sizeof(unsigned long long));

    n = usuarios;
    palabrasFila = (n + 63) / 64;
    unsigned long total = (unsigned long)n * palabrasFila;
    bits = total > 0 ? new unsigned long long[total] : nullptr;
    for (unsigned long i = 0; i < total; ++i) bits[i] = 0;
    Metricas::registrarReserva(total * sizeof(unsigned long long));
    Metricas::contar(total);
}

bool RedAmistad::sonAmigos(unsigned int a, unsigned int b) const {
    if (a >= n || b >= n) return false;
    return (bits[(unsigned long)a * palabrasFila + b / 64] >> (b % 64)) & 1ULL;
}

void RedAmistad::agregarAmistad(unsigned int a, unsigned int b) {
    if (a >= n || b >= n || a == b) return;
    bits[(unsigned long)a * palabrasFila + b / 64] |= 1ULL << (b % 64);
    bits[(unsigned long)b * palabrasFila + a / 64] |= 1ULL << (a % 64);
}

unsigned int RedAmistad::numAmigos(unsigned int a) const {
    if (a >= n) return 0;
    const unsigned long long *fila = bits + (unsigned long)a * palabrasFila;
    unsigned int total = 0;
    for (unsigned int w = 0; w < palabrasFila; ++w) total += contarBits(fila[w]);
    Metricas::contar(palabrasFila);
    return total;
}

unsigned int RedAmistad::amigosEnComun(unsigned int a, unsigned int b) const {
    if (a >= n || b >= n) return 0;
    const unsigned long long *filaA = bits + (unsigned long)a * palabrasFila;
    const unsigned long long *filaB = bits + (unsigned long)b * palabrasFila;
    unsigned int total = 0;
    for (unsigned int w = 0; w < palabrasFila; ++w) total += contarBits(filaA[w] & filaB[w]);
    Metricas::contar(palabrasFila);
    return total;
}

void RedAmistad::amigosDe(unsigned int a, Lista<unsigned int> &salida) const {
    salida = Lista<unsigned int>();
    if (a >= n) return;
    const unsigned long long *fila = bits + (unsigned long)a * palabrasFila;
    unsigned long iteraciones = palabrasFila;
    for (unsigned int w = 0; w < palabrasFila; ++w) {
        unsigned long long palabra = fila[w];
        while (palabra != 0) {
            salida.agregar(w * 64 + bitMenor(palabra));
            palabra &= palabra - 1;
            ++iteraciones;
        }
    }
    Metricas::contar(iteraciones);
}

void RedAmistad::sugerencias(unsigned int a, unsigned int k, Lista<unsigned int> &ids,
                             Lista<unsigned int> &comunes) const {
    ids = Lista<unsigned int>();
    comunes = Lista<unsigned int>();
    if (a >= n || k == 0) return;

    const unsigned long long *filaA = bits + (unsigned long)a * palabrasFila;
    unsigned long long *candidatos = new unsigned long long[palabrasFila];
    for (unsigned int w = 0; w < palabrasFila; ++w) candidatos[w] = 0;
    unsigned long iteraciones = palabrasFila;

    for (unsigned int w = 0; w < palabrasFila; ++w) {
        unsigned long long palabra = filaA[w];
        while (palabra != 0) {
            unsigned int amigo = w * 64 + bitMenor(palabra);
            const unsigned long long *filaAmigo = bits + (unsigned long)amigo * palabrasFila;
            for (unsigned int x = 0; x < palabrasFila; ++x) candidatos[x] |= filaAmigo[x];
            iteraciones += palabrasFila;
            palabra &= palabra - 1;
        }
    }
    for (unsigned int w = 0; w < palabrasFila; ++w) candidatos[w] &= ~filaA[w];
    candidatos[a / 64] &= ~(1ULL << (a % 64));
    iteraciones += palabrasFila;

    ids.reservar(k);
    comunes.reservar(k);
    for (unsigned int w = 0; w < palabrasFila; ++w) {
        unsigned long long palabra = candidatos[w];
        while (palabra != 0) {
            unsigned int c = w * 64 + bitMenor(palabra);
            palabra &= palabra - 1;
            unsigned int enComun = amigosEnComun(a, c);

            unsigned int pos = ids.getTam();
            while (pos > 0 && comunes[pos - 1] < enComun) --pos;
            iteraciones += ids.getTam() - pos + 1;
            if (pos >= k) continue;

            if (ids.getTam() < k) {
                ids.agregar(0);
                comunes.agregar(0);
            }
            for (unsigned int j = ids.getTam() - 1; j > pos; --j) {
                ids[j] = ids[j - 1];
                comunes[j] = comunes[j - 1];
            }
            iteraciones += ids.getTam() - 1 - pos;
            ids[pos] = c;
            comunes[pos] = enComun;
        }
    }

    delete[] candidatos;
    Metricas::sumarLocal(palabrasFila * sizeof(unsigned long long));
    Metricas::contar(iteraciones);
}

unsigned int RedAmistad::contarBits(unsigned long long x) {
    x = x - ((x >> 1) & 0x5555555555555555ULL);
    x = (x & 0x3333333333333333ULL) + ((x >> 2) & 0x3333333333333333ULL);
    x = (x + (x >> 4)) & 0x0F0F0F0F0F0F0F0FULL;
    return (x * 0x0101010101010101ULL) >> 56;
}

unsigned int RedAmistad::bitMenor(unsigned long long x) {
    return contarBits((x & (~x + 1)) - 1);
}
