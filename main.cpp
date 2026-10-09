#include "Foto.h"
#include <iostream>

using namespace std;

unsigned int aprobadas = 0;
unsigned int fallidas = 0;

void verificar(bool condicion, const char *descripcion) {
    if (condicion) {
        ++aprobadas;
        cout << "[OK]    " << descripcion << '\n';
    } else {
        ++fallidas;
        cout << "[FALLO] " << descripcion << '\n';
    }
}

void reporte(const char *titulo) {
    cout << "\n>> Metricas - " << titulo;
    Metricas::reportar();
    cout << '\n';
}

bool mismoTexto(const Lista<char> &a, const char *b, unsigned int n) {
    if (a.getTam() != n) return false;
    for (unsigned int i = 0; i < n; ++i)
        if (a[i] != b[i]) return false;
    return true;
}

bool mismaFecha(const Fecha &a, const Fecha &b) { return !(a < b) && !(b < a); }

void pruebasConstruccion() {
    Metricas::reiniciar();
    char texto[] = {'M', 'a', 'l', 'e', 'c', 'o', 'n'};
    Lista<char> nombre;
    nombre.asignar(texto, 7);

    Foto vacia;
    verificar(vacia.getNombre().getTam() == 0 && vacia.getTamano() == 0 && vacia.getLikes() == 0 &&
                  vacia.getCaptura().esValida(), "constructor por defecto");

    Foto foto(nombre, Fecha(2026, 3, 15, 18, 40), 2048);
    verificar(mismoTexto(foto.getNombre(), "Malecon", 7) && mismaFecha(foto.getCaptura(), Fecha(2026, 3, 15, 18, 40)) &&
                  foto.getTamano() == 2048 && foto.getLikes() == 0, "constructor con datos y likes por defecto en 0");

    Foto conLikes(nombre, Fecha(2026, 3, 15), 100, 7);
    verificar(conLikes.getLikes() == 7, "constructor con likes iniciales");

    nombre[0] = 'X';
    verificar(foto.getNombre()[0] == 'M', "la foto guarda su propia copia del nombre");

    Metricas::sumarLocal(sizeof(texto) + sizeof(nombre) + sizeof(vacia) + sizeof(foto) + sizeof(conLikes) +
                         sizeof(Fecha));
    reporte("construccion");
}

void pruebasCopia() {
    Metricas::reiniciar();
    char texto[] = {'B', 'o', 'c', 'a'};
    Lista<char> nombre;
    nombre.asignar(texto, 4);
    Foto original(nombre, Fecha(2025, 12, 24), 500, 3);

    Foto copia(original);
    copia.darLike();
    char otro[] = {'C', 'a', 'r', 't', 'a', 'g', 'e', 'n', 'a'};
    Lista<char> nuevoNombre;
    nuevoNombre.asignar(otro, 9);
    copia.setNombre(nuevoNombre);
    verificar(original.getLikes() == 3 && mismoTexto(original.getNombre(), "Boca", 4) &&
                  copia.getLikes() == 4 && mismoTexto(copia.getNombre(), "Cartagena", 9),
              "constructor de copia profundo");

    Foto asignada;
    asignada = original;
    asignada.setTamano(999);
    verificar(asignada.getTamano() == 999 && original.getTamano() == 500 &&
                  mismoTexto(asignada.getNombre(), "Boca", 4), "operator= profundo");

    asignada = asignada;
    verificar(asignada.getTamano() == 999 && mismoTexto(asignada.getNombre(), "Boca", 4),
              "autoasignacion no altera la foto");

    Foto encadenada;
    encadenada = asignada = original;
    verificar(encadenada.getTamano() == 500 && asignada.getTamano() == 500, "asignacion encadenada");

    Metricas::sumarLocal(sizeof(texto) + sizeof(nombre) + sizeof(original) + sizeof(copia) + sizeof(otro) +
                         sizeof(nuevoNombre) + sizeof(asignada) + sizeof(encadenada) + sizeof(Fecha));
    reporte("copia y asignacion");
}

void pruebasSettersYLikes() {
    Metricas::reiniciar();
    Foto foto;
    char texto[] = {'P', 'l', 'a', 'y', 'a'};
    Lista<char> nombre;
    nombre.asignar(texto, 5);
    foto.setNombre(nombre);
    foto.setCaptura(Fecha(2024, 2, 29, 7, 0));
    foto.setTamano(4096);
    foto.setLikes(10);
    verificar(mismoTexto(foto.getNombre(), "Playa", 5) && mismaFecha(foto.getCaptura(), Fecha(2024, 2, 29, 7, 0)) &&
                  foto.getTamano() == 4096 && foto.getLikes() == 10, "setters modifican cada atributo");

    foto.darLike();
    foto.darLike();
    verificar(foto.getLikes() == 12, "darLike suma");
    foto.quitarLike();
    verificar(foto.getLikes() == 11, "quitarLike resta");

    foto.setLikes(0);
    foto.quitarLike();
    verificar(foto.getLikes() == 0, "quitarLike no baja de 0");

    Metricas::sumarLocal(sizeof(foto) + sizeof(texto) + sizeof(nombre) + sizeof(Fecha));
    reporte("setters y likes");
}

void pruebaMemoria() {
    Metricas::reiniciar();
    unsigned long antes = Metricas::getMemoriaDinamica();
    {
        char texto[] = {'A', 'l', 'b', 'u', 'm', '0', '1'};
        Lista<char> nombre;
        nombre.asignar(texto, 7);
        Lista<Foto *> fotos;
        for (unsigned int i = 0; i < 5; ++i) {
            fotos.agregar(new Foto(nombre, Fecha(2026, 1, i + 1), 100 * (i + 1)));
            Metricas::registrarReserva(sizeof(Foto));
        }
        Metricas::contar(5);
        verificar(fotos.getTam() == 5 && fotos[4]->getTamano() == 500, "fotos creadas con new en una Lista<Foto *>");
        fotos[2]->darLike();
        verificar(fotos[2]->getLikes() == 1 && fotos[1]->getLikes() == 0, "like en una foto no afecta a otra");

        Metricas::sumarLocal(sizeof(antes) + sizeof(texto) + sizeof(nombre) + sizeof(fotos) + sizeof(unsigned int));
        reporte("5 fotos dinamicas vivas");

        for (unsigned int i = 0; i < fotos.getTam(); ++i) {
            delete fotos[i];
            Metricas::registrarLiberacion(sizeof(Foto));
        }
    }
    verificar(Metricas::getMemoriaDinamica() == antes, "al liberar las fotos la memoria vuelve a su valor inicial");
}

int main() {
    pruebasConstruccion();
    pruebasCopia();
    pruebasSettersYLikes();
    pruebaMemoria();
    cout << "\nAprobadas: " << aprobadas << "  Fallidas: " << fallidas << '\n';
    cout << "sizeof(Foto) = " << sizeof(Foto) << " bytes\n";
    return fallidas == 0 ? 0 : 1;
}
