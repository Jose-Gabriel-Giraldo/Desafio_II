#include "UdeABook.h"
#include <fstream>
#include <chrono>

const char *const UdeABook::SUBRUTA = "udeabook";
const char *const UdeABook::DIRECTORIO = "C:/Users/Jose Gabriel Giraldo/OneDrive/Documentos/Universidad/Sexto semestre/Info/Desafio_II/";

// ------------------------------------------------------------------ Ciclo de vida

UdeABook::UdeABook()
    : usuarios(), mensajes(), red(),
    generador(std::chrono::system_clock::now().time_since_epoch().count()),
    sesion(nullptr), cambios(0) {
    Metricas::registrarExterno();
}

UdeABook::UdeABook(const UdeABook &otro)
    : usuarios(), mensajes(), red(), generador(otro.generador), sesion(nullptr), cambios(0) {
    copiar(otro);
}

UdeABook::~UdeABook() { liberar(); }

const UdeABook &UdeABook::operator=(const UdeABook &otro) {
    if (this != &otro) {
        liberar();
        generador = otro.generador;
        copiar(otro);
    }
    return *this;
}

void UdeABook::liberar() {
    for (unsigned int i = 0; i < usuarios.getTam(); ++i) delete usuarios[i];
    Metricas::registrarLiberacion(usuarios.getTam() * sizeof(Usuario));
    Metricas::contar(usuarios.getTam());
    usuarios = Lista<Usuario *>();
    mensajes = Lista<Mensaje *>();
    sesion = nullptr;
    cambios = 0;
}

void UdeABook::copiar(const UdeABook &otro) {
    usuarios.reservar(otro.usuarios.getTam());
    for (unsigned int i = 0; i < otro.usuarios.getTam(); ++i)
        usuarios.agregar(new Usuario(*otro.usuarios[i]));
    Metricas::registrarReserva(usuarios.getTam() * sizeof(Usuario));

    mensajes.reservar(otro.mensajes.getTam());
    for (unsigned int i = 0; i < otro.mensajes.getTam(); ++i) {
        Mensaje *m = new Mensaje(*otro.mensajes[i]);
        usuarios[m->getEmisor()]->agregarEnviado(m);
        usuarios[m->getReceptor()]->agregarRecibido(m);
        mensajes.agregar(m);
    }
    Metricas::contar(otro.usuarios.getTam() + otro.mensajes.getTam());
    red = otro.red;
    sesion = nullptr;
    cambios = 0;
}

Usuario *UdeABook::getSesion() const { return sesion; }
const Lista<Usuario *> &UdeABook::getUsuarios() const { return usuarios; }
const RedAmistad &UdeABook::getRed() const { return red; }
unsigned char UdeABook::getCambios() const { return cambios; }
const char *UdeABook::getSubruta() { return SUBRUTA; }
const char *UdeABook::getDirectorio() { return DIRECTORIO; }

// ------------------------------------------------------------------ I. Carga y guardado

bool UdeABook::cargarDatos() {
    Lista<char> nombreArchivo = ruta("usuarios.txt");
    unsigned long n = 0, a = 0, b = 0, c = 0, d = 0, iteraciones = 0;
    Lista<char> nombre, clave, correo, ciudad, pais;
    Fecha nacimiento, inscripcion, fecha;
    {
        ifstream archivo(&nombreArchivo[0]);
        if (!archivo) return false;
        liberar();
        leer(archivo, n);
        usuarios.reservar(n);
        for (unsigned long i = 0; i < n; ++i) {
            leer(archivo, nombre, '|');
            leer(archivo, clave, '|');
            leer(archivo, correo, '|');
            leer(archivo, ciudad, '|');
            leer(archivo, pais, '|');
            leer(archivo, nacimiento);
            leer(archivo, inscripcion);
            leer(archivo, a);
            leer(archivo, b);
            leer(archivo, c);
            leer(archivo, d);
            Usuario *u = new Usuario(i, nombre, clave, correo, ciudad, pais, nacimiento, inscripcion);
            u->reservar(a, b, c, d);
            usuarios.agregar(u);
        }
        Metricas::registrarReserva(n * sizeof(Usuario));
        Metricas::contar(n);
    }
    red.inicializar(n);

    nombreArchivo = ruta("amistades.txt");
    {
        ifstream archivo(&nombreArchivo[0]);
        c = 0;
        leer(archivo, c);
        for (unsigned long i = 0; i < c; ++i) {
            leer(archivo, a);
            leer(archivo, b);
            red.agregarAmistad(a, b);
        }
        Metricas::contar(c);
    }

    nombreArchivo = ruta("solicitudes.txt");
    {
        ifstream archivo(&nombreArchivo[0]);
        c = 0;
        leer(archivo, c);
        for (unsigned long i = 0; i < c; ++i) {
            leer(archivo, a);
            leer(archivo, b);
            if (a < n && b < n) usuarios[b]->agregarSolicitud(a);
        }
        Metricas::contar(c);
    }

    nombreArchivo = ruta("mensajes.txt");
    {
        ifstream archivo(&nombreArchivo[0]);
        c = 0;
        leer(archivo, c);
        mensajes.reservar(c + c / 8);
        for (unsigned long i = 0; i < c; ++i) {
            leer(archivo, a);
            leer(archivo, b);
            leer(archivo, fecha);
            leer(archivo, d);
            leer(archivo, nombre);
            if (a >= n || b >= n) continue;
            Mensaje *m = new Mensaje(a, b, fecha, nombre, d == 1);
            usuarios[a]->agregarEnviado(m);
            usuarios[b]->agregarRecibido(m);
            mensajes.agregar(m);
        }
        Metricas::contar(c);
    }

    nombreArchivo = ruta("albumes.txt");
    {
        ifstream archivo(&nombreArchivo[0]);
        c = 0;
        leer(archivo, c);
        iteraciones = c;
        for (unsigned long i = 0; i < c; ++i) {
            unsigned long fotos = 0, tamano = 0, likes = 0;
            leer(archivo, a);
            leer(archivo, nombre, '|');
            leer(archivo, b);
            leer(archivo, fotos);
            Album *album = new Album(nombre, b);
            for (unsigned long j = 0; j < fotos; ++j) {
                leer(archivo, nombre, '|');
                leer(archivo, fecha);
                leer(archivo, tamano);
                leer(archivo, likes);
                album->agregarFoto(new Foto(nombre, fecha, tamano, likes));
            }
            iteraciones += fotos;
            if (a < n) usuarios[a]->agregarAlbum(album);
            else delete album;
        }
        Metricas::contar(iteraciones);
    }

    aplicarCambios();
    Metricas::sumarLocal(sizeof(nombreArchivo) + sizeof(ifstream) + 6 * sizeof(n) + 5 * sizeof(nombre) +
                         3 * sizeof(fecha));
    return true;
}

void UdeABook::aplicarCambios() {
    Lista<char> nombreArchivo = ruta("cambios.txt");
    ifstream archivo(&nombreArchivo[0]);
    if (!archivo) return;

    Lista<char> tipo, texto;
    Fecha fecha;
    unsigned long a = 0, b = 0, c = 0, d = 0, lineas = 0;
    unsigned int n = usuarios.getTam();
    while (leer(archivo, tipo, '|')) {
        ++lineas;
        if (tipo.getTam() == 0) continue;
        if (tipo[0] == 'M') {
            leer(archivo, a);
            leer(archivo, b);
            leer(archivo, fecha);
            leer(archivo, texto);
            if (a >= n || b >= n) continue;
            Mensaje *m = new Mensaje(a, b, fecha, texto);
            usuarios[a]->agregarEnviado(m);
            usuarios[b]->agregarRecibido(m);
            mensajes.agregar(m);
            cambios |= MENSAJES;
        } else if (tipo[0] == 'L') {
            leer(archivo, a);
            if (a < n) usuarios[a]->marcarLeidos();
            cambios |= MENSAJES;
        } else if (tipo[0] == 'S') {
            leer(archivo, a);
            leer(archivo, b);
            if (a < n && b < n) usuarios[b]->agregarSolicitud(a);
            cambios |= SOLICITUDES;
        } else if (tipo[0] == 'A') {
            leer(archivo, a);
            leer(archivo, b);
            if (a >= n || b >= n) continue;
            const Lista<unsigned int> &pendientes = usuarios[b]->getSolicitudes();
            unsigned int i = 0;
            while (i < pendientes.getTam() && pendientes[i] != a) ++i;
            Metricas::contar(i);
            usuarios[b]->eliminarSolicitud(i);
            red.agregarAmistad(a, b);
            cambios |= AMISTADES | SOLICITUDES;
        } else if (tipo[0] == 'K') {
            leer(archivo, a);
            leer(archivo, b);
            leer(archivo, c);
            leer(archivo, d);
            if (a < n && b < usuarios[a]->getAlbumes().getTam() &&
                c < usuarios[a]->getAlbumes()[b]->getFotos().getTam())
                usuarios[a]->getAlbumes()[b]->getFotos()[c]->setLikes(d);
            cambios |= ALBUMES;
        }
    }
    Metricas::contar(lineas);
    Metricas::sumarLocal(sizeof(nombreArchivo) + sizeof(archivo) + 2 * sizeof(tipo) + sizeof(fecha) +
                         5 * sizeof(a) + sizeof(n));
}

bool UdeABook::guardarDatos() {
    if (cambios == 0) return true;
    Lista<char> nombreArchivo;
    unsigned long total = 0, iteraciones = 0;
    unsigned int n = usuarios.getTam();

    if (cambios & AMISTADES) {
        nombreArchivo = ruta("amistades.txt");
        ofstream archivo(&nombreArchivo[0]);
        if (!archivo) return false;
        escribir(archivo, red.getAmistades());
        archivo << '\n';
        Lista<unsigned int> amigos;
        for (unsigned int i = 0; i < n; ++i) {
            red.amigosDe(i, amigos);
            for (unsigned int j = 0; j < amigos.getTam(); ++j) {
                if (amigos[j] < i) continue;
                escribir(archivo, i);
                archivo << '|';
                escribir(archivo, amigos[j]);
                archivo << '\n';
            }
            iteraciones += amigos.getTam();
        }
    }

    if (cambios & SOLICITUDES) {
        nombreArchivo = ruta("solicitudes.txt");
        ofstream archivo(&nombreArchivo[0]);
        if (!archivo) return false;
        total = 0;
        for (unsigned int i = 0; i < n; ++i) total += usuarios[i]->getSolicitudes().getTam();
        escribir(archivo, total);
        archivo << '\n';
        for (unsigned int i = 0; i < n; ++i) {
            const Lista<unsigned int> &pendientes = usuarios[i]->getSolicitudes();
            for (unsigned int j = 0; j < pendientes.getTam(); ++j) {
                escribir(archivo, pendientes[j]);
                archivo << '|';
                escribir(archivo, i);
                archivo << '\n';
            }
        }
        iteraciones += 2 * n + total;
    }

    if (cambios & MENSAJES) {
        nombreArchivo = ruta("mensajes.txt");
        ofstream archivo(&nombreArchivo[0]);
        if (!archivo) return false;
        escribir(archivo, mensajes.getTam());
        archivo << '\n';
        for (unsigned int i = 0; i < mensajes.getTam(); ++i) {
            const Mensaje &m = *mensajes[i];
            escribir(archivo, m.getEmisor());
            archivo << '|';
            escribir(archivo, m.getReceptor());
            archivo << '|';
            escribir(archivo, m.getFecha());
            archivo << '|';
            archivo << (m.getLeido() ? '1' : '0');
            archivo << '|';
            escribir(archivo, m.getTexto());
            archivo << '\n';
        }
        iteraciones += mensajes.getTam();
    }

    if (cambios & ALBUMES) {
        nombreArchivo = ruta("albumes.txt");
        ofstream archivo(&nombreArchivo[0]);
        if (!archivo) return false;
        total = 0;
        for (unsigned int i = 0; i < n; ++i) total += usuarios[i]->getAlbumes().getTam();
        escribir(archivo, total);
        archivo << '\n';
        for (unsigned int i = 0; i < n; ++i) {
            const Lista<Album *> &albumes = usuarios[i]->getAlbumes();
            for (unsigned int j = 0; j < albumes.getTam(); ++j) {
                const Lista<Foto *> &fotos = albumes[j]->getFotos();
                escribir(archivo, i);
                archivo << '|';
                escribir(archivo, albumes[j]->getNombre());
                archivo << '|';
                escribir(archivo, albumes[j]->getPrivacidad());
                archivo << '|';
                escribir(archivo, fotos.getTam());
                archivo << '\n';
                for (unsigned int k = 0; k < fotos.getTam(); ++k) {
                    escribir(archivo, fotos[k]->getNombre());
                    archivo << '|';
                    escribir(archivo, fotos[k]->getCaptura());
                    archivo << '|';
                    escribir(archivo, fotos[k]->getTamano());
                    archivo << '|';
                    escribir(archivo, fotos[k]->getLikes());
                    archivo << '\n';
                }
                iteraciones += fotos.getTam();
            }
        }
        iteraciones += 2 * n + total;
    }

    nombreArchivo = ruta("cambios.txt");
    {
        ofstream vaciar(&nombreArchivo[0]);
    }
    cambios = 0;

    Metricas::contar(iteraciones);
    Metricas::sumarLocal(sizeof(nombreArchivo) + sizeof(ofstream) + 2 * sizeof(total) + sizeof(n));
    return true;
}

// ------------------------------------------------------------------ Menu principal

void UdeABook::ejecutar() {
    Metricas::reiniciar();
    bool cargado = cargarDatos();
    cout << "\n=== I. Carga de datos ===\n";
    if (!cargado) {
        cout << "No se encontro " << DIRECTORIO << "usuarios.txt\n";
        return;
    }
    cout << "Usuarios: ";
    escribir(cout, usuarios.getTam());
    cout << "  Mensajes: ";
    escribir(cout, mensajes.getTam());
    cout << '\n';
    Metricas::sumarLocal(sizeof(cargado));
    Metricas::reportar();

    Lista<char> nombre, clave;
    unsigned long opcion = 1;
    while (true) {
        cout << "\n===== Ude@Book =====\n1. Iniciar sesion\n0. Salir\nOpcion: ";
        opcion = leerOpcion();
        if (opcion == 0) break;
        if (opcion != 1) {
            cout << "Opcion no valida\n";
            continue;
        }

        cout << "Usuario: ";
        leer(cin, nombre);
        cout << "Clave: ";
        leer(cin, clave);
        Metricas::reiniciar();
        cout << "\n=== II. Inicio de sesion ===\n";
        if (iniciarSesion(nombre, clave)) mostrarFotosAmigos();
        else cout << "Usuario o clave incorrectos\n";
        Metricas::sumarLocal(sizeof(nombre) + sizeof(clave) + sizeof(opcion));
        Metricas::reportar();

        while (sesion != nullptr) {
            cout << "\n===== ";
            escribir(cout, sesion->getNombre());
            cout << " =====\n1. Amistades\n2. Visitar perfil de un amigo\n3. Sugerencias de amistad\n"
                    "4. Mensajes\n0. Cerrar sesion\nOpcion: ";
            opcion = leerOpcion();
            if (opcion == 0) {
                cerrarSesion();
            } else if (opcion == 1) {
                menuAmistades();
            } else if (opcion == 2) {
                Metricas::reiniciar();
                cout << "\n=== IV. Perfil de amigo ===\n";
                mostrarAmigos();
                cout << "Nombre del amigo: ";
                leer(cin, nombre);
                visitarPerfil(nombre);
                Metricas::sumarLocal(sizeof(nombre) + sizeof(opcion));
                Metricas::reportar();
            } else if (opcion == 3) {
                cout << "Cantidad de sugerencias (1 a 20): ";
                unsigned long k = leerOpcion();
                Metricas::reiniciar();
                cout << "\n=== V. Sugerencias ===\n";
                verSugerencias(k);
                Metricas::sumarLocal(sizeof(k) + sizeof(opcion));
                Metricas::reportar();
            } else if (opcion == 4) {
                menuMensajes();
            } else {
                cout << "Opcion no valida\n";
            }
        }
    }

    Metricas::reiniciar();
    cout << "\n=== I. Consolidacion de datos ===\n";
    if (guardarDatos()) cout << "Datos guardados\n";
    else cout << "No se pudieron guardar los datos\n";
    Metricas::reportar();
}

// ------------------------------------------------------------------ II. Inicio de sesion

bool UdeABook::iniciarSesion(const Lista<char> &nombre, const Lista<char> &clave) {
    Usuario *u = buscarUsuario(nombre);
    if (u == nullptr || !u->verificarClave(clave)) return false;
    sesion = u;
    cout << "Bienvenido, ";
    escribir(cout, u->getNombre());
    cout << '\n';
    Metricas::sumarLocal(sizeof(u));
    return true;
}

void UdeABook::cerrarSesion() { sesion = nullptr; }

void UdeABook::mostrarFotosAmigos() {
    Lista<unsigned int> amigos, pesos;
    red.amigosDe(sesion->getId(), amigos);
    if (amigos.getTam() == 0) {
        cout << "Aun no tienes amigos\n";
        return;
    }

    pesos.reservar(amigos.getTam());
    for (unsigned int i = 0; i < amigos.getTam(); ++i) {
        unsigned int comunes = red.amigosEnComun(sesion->getId(), amigos[i]);
        pesos.agregar(comunes <= 10 ? 2 : comunes <= 20 ? 5 : 7);
    }
    Metricas::contar(amigos.getTam());

    Lista<unsigned int> candidatos, pesosRonda;
    bool actualizado = false;
    while (true) {
        candidatos = amigos;
        pesosRonda = pesos;
        unsigned int total = 0;
        for (unsigned int i = 0; i < pesos.getTam(); ++i) total += pesos[i];
        unsigned int mostrar = amigos.getTam() < 3 ? amigos.getTam() : 3;
        Metricas::contar(pesos.getTam() + mostrar);

        cout << "Fotos de perfil de tus amigos:\n";
        for (unsigned int i = 0; i < mostrar; ++i) {
            unsigned int id = elegirPonderado(candidatos, pesosRonda, total);
            cout << "  ";
            escribir(cout, usuarios[id]->getNombre());
            cout << " (";
            escribir(cout, red.amigosEnComun(sesion->getId(), id));
            cout << " en comun): ";
            imprimirRuta(*usuarios[id]);
            cout << '\n';
        }

        if (actualizado) break;
        cout << "Actualizar fotos? (1 si / 0 no): ";
        if (leerOpcion() != 1) break;
        actualizado = true;
    }
    Metricas::sumarLocal(4 * sizeof(amigos) + sizeof(actualizado) +
                         (amigos.getCapacidad() + pesos.getCapacidad() + candidatos.getCapacidad() +
                          pesosRonda.getCapacidad()) * sizeof(unsigned int));
}

unsigned int UdeABook::elegirPonderado(Lista<unsigned int> &ids, Lista<unsigned int> &pesos,
                                       unsigned int &total) {
    unsigned int r = generador() % total;
    Metricas::registrarExterno();
    unsigned int i = 0;
    while (r >= pesos[i]) {
        r -= pesos[i];
        ++i;
    }
    Metricas::contar(i + 1);
    unsigned int elegido = ids[i];
    total -= pesos[i];
    ids.eliminar(i);
    pesos.eliminar(i);
    Metricas::sumarLocal(sizeof(r) + sizeof(i) + sizeof(elegido));
    return elegido;
}

// ------------------------------------------------------------------ III. Amistades

void UdeABook::menuAmistades() {
    Lista<unsigned int> pendientes;
    Lista<char> nombre;
    while (true) {
        const Lista<unsigned int> &solicitudes = sesion->getSolicitudes();
        cout << "\n=== III. Amistades ===\nSolicitudes recibidas: ";
        escribir(cout, solicitudes.getTam());
        cout << '\n';
        for (unsigned int i = 0; i < solicitudes.getTam(); ++i) {
            cout << "  ";
            escribir(cout, i + 1);
            cout << ". ";
            escribir(cout, usuarios[solicitudes[i]]->getNombre());
            cout << '\n';
        }
        cout << "1. Aceptar solicitud\n2. Enviar solicitud\n0. Volver\nOpcion: ";
        unsigned long opcion = leerOpcion();
        if (opcion == 0) break;

        if (opcion == 1) {
            cout << "Numero de la solicitud: ";
            unsigned long numero = leerOpcion();
            Metricas::reiniciar();
            if (numero >= 1 && numero <= solicitudes.getTam()) {
                unsigned int origen = solicitudes[numero - 1];
                aceptarSolicitud(numero - 1);
                pendientes.agregar('A');
                pendientes.agregar(origen);
                pendientes.agregar(sesion->getId());
                cout << "Ahora eres amigo de ";
                escribir(cout, usuarios[origen]->getNombre());
                cout << '\n';
            } else {
                cout << "Numero no valido\n";
            }
            Metricas::sumarLocal(sizeof(numero) + sizeof(opcion) + sizeof(pendientes) +
                                 pendientes.getCapacidad() * sizeof(unsigned int));
            Metricas::reportar();
        } else if (opcion == 2) {
            cout << "Nombre del destinatario: ";
            leer(cin, nombre);
            Metricas::reiniciar();
            Usuario *destino = buscarUsuario(nombre);
            if (destino == nullptr) {
                cout << "El usuario no existe\n";
            } else if (enviarSolicitud(*sesion, *destino)) {
                pendientes.agregar('S');
                pendientes.agregar(sesion->getId());
                pendientes.agregar(destino->getId());
            }
            Metricas::sumarLocal(sizeof(destino) + sizeof(nombre) + sizeof(opcion) + sizeof(pendientes) +
                                 pendientes.getCapacidad() * sizeof(unsigned int));
            Metricas::reportar();
        } else {
            cout << "Opcion no valida\n";
        }
    }
    anotarPendientes(pendientes);
}

bool UdeABook::enviarSolicitud(Usuario &origen, Usuario &destino) {
    if (origen.getId() == destino.getId()) {
        cout << "No puedes enviarte una solicitud a ti mismo\n";
        return false;
    }
    if (red.sonAmigos(origen.getId(), destino.getId())) {
        cout << "Ya son amigos\n";
        return false;
    }
    if (destino.getSolicitudes().contiene(origen.getId())) {
        cout << "Ya le enviaste una solicitud\n";
        return false;
    }
    if (origen.getSolicitudes().contiene(destino.getId())) {
        cout << "Ese usuario ya te envio una solicitud; aceptala en Amistades\n";
        return false;
    }
    destino.agregarSolicitud(origen.getId());
    cambios |= SOLICITUDES;
    cout << "Solicitud enviada a ";
    escribir(cout, destino.getNombre());
    cout << '\n';
    return true;
}

void UdeABook::aceptarSolicitud(unsigned int posicion) {
    unsigned int origen = sesion->getSolicitudes()[posicion];
    sesion->eliminarSolicitud(posicion);
    red.agregarAmistad(origen, sesion->getId());
    cambios |= AMISTADES | SOLICITUDES;
    Metricas::sumarLocal(sizeof(origen) + sizeof(posicion));
}

// ------------------------------------------------------------------ IV. Perfil de un amigo

void UdeABook::mostrarAmigos() const {
    Lista<unsigned int> amigos;
    red.amigosDe(sesion->getId(), amigos);
    if (amigos.getTam() == 0) {
        cout << "Aun no tienes amigos\n";
        return;
    }
    cout << "Tus amigos (";
    escribir(cout, amigos.getTam());
    cout << "):\n";
    for (unsigned int i = 0; i < amigos.getTam(); ++i) {
        cout << "  ";
        escribir(cout, usuarios[amigos[i]]->getNombre());
        cout << '\n';
    }
    Metricas::contar(amigos.getTam());
    Metricas::sumarLocal(sizeof(amigos) + amigos.getCapacidad() * sizeof(unsigned int));
}

void UdeABook::visitarPerfil(const Lista<char> &nombre) {
    Usuario *buscado = buscarUsuario(nombre);
    if (buscado == nullptr || !red.sonAmigos(sesion->getId(), buscado->getId())) {
        cout << "Solo puedes visitar el perfil de tus amigos\n";
        return;
    }
    Usuario &amigo = *buscado;
    Lista<unsigned int> visibles, pendientes;
    const Lista<Album *> &albumes = amigo.getAlbumes();
    for (unsigned int i = 0; i < albumes.getTam(); ++i)
        if (albumes[i]->getPrivacidad() != Album::PRIVADO) visibles.agregar(i);
    Metricas::contar(albumes.getTam());

    while (true) {
        cout << "\nPerfil de ";
        escribir(cout, amigo.getNombre());
        cout << " - ";
        imprimirRuta(amigo);
        cout << '\n';
        if (visibles.getTam() == 0) {
            cout << "No tiene albumes visibles para ti\n";
            break;
        }
        for (unsigned int i = 0; i < visibles.getTam(); ++i) {
            const Album &album = *albumes[visibles[i]];
            cout << "  ";
            escribir(cout, i + 1);
            cout << ". ";
            escribir(cout, album.getNombre());
            cout << (album.getPrivacidad() == Album::PUBLICO ? " (publico, " : " (amigos, ");
            escribir(cout, album.getFotos().getTam());
            cout << " fotos)\n";
        }
        cout << "Album a ver (0 para salir): ";
        unsigned long opcion = leerOpcion();
        if (opcion == 0) break;
        if (opcion > visibles.getTam()) {
            cout << "Opcion no valida\n";
            continue;
        }
        carrusel(amigo, *albumes[visibles[opcion - 1]], pendientes);
    }
    anotarPendientes(pendientes);
    Metricas::sumarLocal(sizeof(Usuario *) + 2 * sizeof(visibles) +
                         (visibles.getCapacidad() + pendientes.getCapacidad()) * sizeof(unsigned int));
}

void UdeABook::carrusel(Usuario &amigo, Album &album, Lista<unsigned int> &pendientes) {
    const Lista<Foto *> &fotos = album.getFotos();
    unsigned int n = fotos.getTam();
    if (n == 0) {
        cout << "El album no tiene fotos\n";
        return;
    }
    unsigned int indiceAlbum = 0;
    while (amigo.getAlbumes()[indiceAlbum] != &album) ++indiceAlbum;
    Metricas::contar(indiceAlbum + 1);

    unsigned int i = 0;
    unsigned long opcion = 1, vistas = 0;
    while (true) {
        ++vistas;
        Foto &foto = *fotos[i];
        cout << "\n[Foto ";
        escribir(cout, i + 1);
        cout << " de ";
        escribir(cout, n);
        cout << "] ";
        imprimirRuta(amigo, album, foto);
        cout << "\nCaptura: ";
        escribir(cout, foto.getCaptura());
        cout << "  Likes: ";
        escribir(cout, foto.getLikes());
        cout << "\n1. Siguiente  2. Previa  3. Dar like  4. Quitar like  0. Salir\nOpcion: ";
        opcion = leerOpcion();
        if (opcion == 0) break;
        if (opcion == 1) {
            i = (i + 1) % n;
        } else if (opcion == 2) {
            i = (i + n - 1) % n;
        } else if (opcion == 3 || opcion == 4) {
            if (opcion == 3) foto.darLike();
            else foto.quitarLike();
            pendientes.agregar('K');
            pendientes.agregar(amigo.getId());
            pendientes.agregar(indiceAlbum);
            pendientes.agregar(i);
            pendientes.agregar(foto.getLikes());
            cambios |= ALBUMES;
        } else {
            cout << "Opcion no valida\n";
        }
    }
    Metricas::contar(vistas);
    Metricas::sumarLocal(sizeof(fotos) + sizeof(n) + sizeof(indiceAlbum) + sizeof(i) + sizeof(opcion) +
                         sizeof(vistas));
}

// ------------------------------------------------------------------ V. Sugerencias

void UdeABook::verSugerencias(unsigned int k) {
    if (k < 1 || k > 20) {
        cout << "La cantidad debe estar entre 1 y 20\n";
        return;
    }
    Lista<unsigned int> ids, comunes, pendientes;
    red.sugerencias(sesion->getId(), k, ids, comunes);
    if (ids.getTam() == 0) {
        cout << "No hay sugerencias\n";
        return;
    }
    for (unsigned int i = 0; i < ids.getTam(); ++i) {
        cout << "  ";
        escribir(cout, i + 1);
        cout << ". ";
        escribir(cout, usuarios[ids[i]]->getNombre());
        cout << " (";
        escribir(cout, comunes[i]);
        cout << " amigos en comun)\n";
    }
    Metricas::contar(ids.getTam());

    cout << "Enviar solicitud a (numero, 0 ninguno): ";
    unsigned long opcion = leerOpcion();
    if (opcion >= 1 && opcion <= ids.getTam() && enviarSolicitud(*sesion, *usuarios[ids[opcion - 1]])) {
        pendientes.agregar('S');
        pendientes.agregar(sesion->getId());
        pendientes.agregar(ids[opcion - 1]);
        anotarPendientes(pendientes);
    }
    Metricas::sumarLocal(3 * sizeof(ids) + sizeof(opcion) + sizeof(k) +
                         (ids.getCapacidad() + comunes.getCapacidad() + pendientes.getCapacidad()) *
                             sizeof(unsigned int));
}

// ------------------------------------------------------------------ VI. Mensajes

void UdeABook::menuMensajes() {
    Lista<char> nombre, texto;
    Fecha desde, hasta;
    while (true) {
        cout << "\n=== VI. Mensajes ===\nNo leidos: ";
        escribir(cout, sesion->getRecibidos().getTam() - sesion->getPrimerNoLeido());
        cout << "\n1. Enviar mensaje\n2. Leer mensajes no leidos\n3. Buscar por rango de fechas\n"
                "0. Volver\nOpcion: ";
        unsigned long opcion = leerOpcion();
        if (opcion == 0) break;

        if (opcion == 1) {
            Metricas::reiniciar();
            mostrarAmigos();
            cout << "Destinatario: ";
            leer(cin, nombre);
            cout << "Mensaje (maximo 512 caracteres): ";
            leer(cin, texto);
            enviarMensaje(nombre, texto);
        } else if (opcion == 2) {
            Metricas::reiniciar();
            leerMensajes();
        } else if (opcion == 3) {
            cout << "Desde (AAAA-MM-DD): ";
            bool valido = leer(cin, desde);
            cout << "Hasta (AAAA-MM-DD): ";
            valido = leer(cin, hasta) && valido;
            Metricas::reiniciar();
            if (valido) buscarMensajes(desde, hasta);
            else cout << "Fecha no valida\n";
        } else {
            cout << "Opcion no valida\n";
            continue;
        }
        Metricas::sumarLocal(2 * sizeof(nombre) + 2 * sizeof(desde) + sizeof(opcion));
        Metricas::reportar();
    }
}

void UdeABook::enviarMensaje(const Lista<char> &nombre, const Lista<char> &texto) {
    Usuario *buscado = buscarUsuario(nombre);
    if (buscado == nullptr || !red.sonAmigos(sesion->getId(), buscado->getId())) {
        cout << "Solo puedes escribirle a tus amigos\n";
        return;
    }
    Usuario &destino = *buscado;
    if (!Mensaje::textoValido(texto)) {
        cout << "El mensaje debe tener entre 1 y 512 caracteres\n";
        return;
    }
    Mensaje *m = new Mensaje(sesion->getId(), destino.getId(), Fecha::ahora(), texto);
    sesion->agregarEnviado(m);
    destino.agregarRecibido(m);
    mensajes.agregar(m);
    cambios |= MENSAJES;
    anotarMensaje(*m);
    cout << "Mensaje enviado\n";
    Metricas::sumarLocal(sizeof(Usuario *) + sizeof(m));
}

void UdeABook::leerMensajes() {
    const Lista<Mensaje *> &recibidos = sesion->getRecibidos();
    unsigned int inicio = sesion->getPrimerNoLeido();
    if (inicio == recibidos.getTam()) {
        cout << "No tienes mensajes sin leer\n";
        return;
    }
    for (unsigned int i = inicio; i < recibidos.getTam(); ++i) imprimirMensaje(*recibidos[i]);
    Metricas::contar(recibidos.getTam() - inicio);
    sesion->marcarLeidos();
    cambios |= MENSAJES;
    anotarLectura(sesion->getId());
    Metricas::sumarLocal(sizeof(inicio));
}

void UdeABook::buscarMensajes(const Fecha &desde, const Fecha &hasta) {
    Fecha inicio(desde.getAnio(), desde.getMes(), desde.getDia(), 0, 0);
    Fecha fin(hasta.getAnio(), hasta.getMes(), hasta.getDia(), 23, 59);
    if (fin < inicio) {
        cout << "La fecha inicial es posterior a la final\n";
        return;
    }
    unsigned long encontrados = 0;
    for (unsigned int lista = 0; lista < 2; ++lista) {
        const Lista<Mensaje *> &mensajesLista = lista == 0 ? sesion->getEnviados() : sesion->getRecibidos();
        cout << (lista == 0 ? "--- Enviados ---\n" : "--- Recibidos ---\n");
        unsigned int i = cotaInferior(mensajesLista, inicio);
        unsigned int primero = i;
        while (i < mensajesLista.getTam() && !(fin < mensajesLista[i]->getFecha())) {
            imprimirMensaje(*mensajesLista[i]);
            ++i;
        }
        Metricas::contar(i - primero + 1);
        encontrados += i - primero;
    }
    cout << "Mensajes encontrados: ";
    escribir(cout, encontrados);
    cout << '\n';
    Metricas::sumarLocal(2 * sizeof(inicio) + sizeof(encontrados) + 3 * sizeof(unsigned int));
}

unsigned int UdeABook::cotaInferior(const Lista<Mensaje *> &lista, const Fecha &fecha) const {
    unsigned int izquierda = 0, derecha = lista.getTam(), pasos = 0;
    while (izquierda < derecha) {
        unsigned int medio = izquierda + (derecha - izquierda) / 2;
        if (lista[medio]->getFecha() < fecha) izquierda = medio + 1;
        else derecha = medio;
        ++pasos;
    }
    Metricas::contar(pasos);
    Metricas::sumarLocal(4 * sizeof(unsigned int));
    return izquierda;
}

void UdeABook::imprimirMensaje(const Mensaje &m) const {
    cout << "  [";
    escribir(cout, m.getFecha());
    cout << "] ";
    escribir(cout, usuarios[m.getEmisor()]->getNombre());
    cout << " -> ";
    escribir(cout, usuarios[m.getReceptor()]->getNombre());
    cout << ": ";
    escribir(cout, m.getTexto());
    cout << '\n';
}

// ------------------------------------------------------------------ Bitacora de cambios

void UdeABook::anotarMensaje(const Mensaje &m) {
    Lista<char> nombreArchivo = ruta("cambios.txt");
    ofstream archivo(&nombreArchivo[0], ios::app);
    archivo << 'M';
    archivo << '|';
    escribir(archivo, m.getEmisor());
    archivo << '|';
    escribir(archivo, m.getReceptor());
    archivo << '|';
    escribir(archivo, m.getFecha());
    archivo << '|';
    escribir(archivo, m.getTexto());
    archivo << '\n';
}

void UdeABook::anotarLectura(unsigned int usuario) {
    Lista<char> nombreArchivo = ruta("cambios.txt");
    ofstream archivo(&nombreArchivo[0], ios::app);
    archivo << 'L';
    archivo << '|';
    escribir(archivo, usuario);
    archivo << '\n';
}

void UdeABook::anotarPendientes(const Lista<unsigned int> &pendientes) {
    if (pendientes.getTam() == 0) return;
    Lista<char> nombreArchivo = ruta("cambios.txt");
    ofstream archivo(&nombreArchivo[0], ios::app);
    unsigned int i = 0;
    while (i < pendientes.getTam()) {
        char tipo = pendientes[i++];
        unsigned int campos = tipo == 'K' ? 4 : 2;
        archivo << tipo;
        for (unsigned int j = 0; j < campos; ++j) {
            archivo << '|';
            escribir(archivo, pendientes[i++]);
        }
        archivo << '\n';
    }
    Metricas::contar(pendientes.getTam());
}

// ------------------------------------------------------------------ Auxiliares

Usuario *UdeABook::buscarUsuario(const Lista<char> &nombre) const {
    unsigned int izquierda = 0, derecha = usuarios.getTam(), pasos = 0;
    Usuario *encontrado = nullptr;
    while (izquierda < derecha) {
        unsigned int medio = izquierda + (derecha - izquierda) / 2;
        int orden = comparar(usuarios[medio]->getNombre(), nombre);
        ++pasos;
        if (orden == 0) {
            encontrado = usuarios[medio];
            break;
        }
        if (orden < 0) izquierda = medio + 1;
        else derecha = medio;
    }
    Metricas::contar(pasos);
    Metricas::sumarLocal(4 * sizeof(unsigned int) + sizeof(encontrado) + sizeof(int));
    return encontrado;
}

void UdeABook::imprimirRuta(const Usuario &usuario) const {
    cout << "/users/" << SUBRUTA << '/';
    escribir(cout, usuario.getNombre());
    cout << "/albums/profile.png";
}

void UdeABook::imprimirRuta(const Usuario &usuario, const Album &album, const Foto &foto) const {
    cout << "/users/" << SUBRUTA << '/';
    escribir(cout, usuario.getNombre());
    cout << "/albums/";
    escribir(cout, album.getNombre());
    cout << '/';
    escribir(cout, foto.getNombre());
    cout << ".png";
}

Lista<char> UdeABook::ruta(const char *archivo) {
    Lista<char> resultado;
    unsigned int i = 0;
    for (i = 0; DIRECTORIO[i] != '\0'; ++i) resultado.agregar(DIRECTORIO[i]);
    for (unsigned int j = 0; archivo[j] != '\0'; ++j, ++i) resultado.agregar(archivo[j]);
    resultado.agregar('\0');
    Metricas::contar(i + 1);
    return resultado;
}

int UdeABook::comparar(const Lista<char> &a, const Lista<char> &b) {
    unsigned int i = 0;
    while (i < a.getTam() && i < b.getTam() && a[i] == b[i]) ++i;
    Metricas::contar(i);
    if (i < a.getTam() && i < b.getTam())
        return a[i] < b[i] ? -1 : 1;
    if (a.getTam() == b.getTam()) return 0;
    return a.getTam() < b.getTam() ? -1 : 1;
}

bool UdeABook::leer(istream &entrada, Lista<char> &salida, char separador) {
    salida = Lista<char>();
    char c;
    bool leyo = false;
    unsigned long n = 0;
    while (entrada >> noskipws >> c) {
        leyo = true;
        if (c == separador || c == '\n') break;
        if (c != '\r') salida.agregar(c);
        ++n;
    }
    Metricas::contar(n);
    return leyo;
}

bool UdeABook::leer(istream &entrada, unsigned long &numero) {
    numero = 0;
    char c;
    bool valido = true, leyo = false;
    unsigned long digitos = 0;
    while (entrada >> noskipws >> c) {
        leyo = true;
        if (c == '|' || c == '\n') break;
        if (c == '\r' || (c == ' ' && digitos == 0)) continue;
        if (c >= '0' && c <= '9' && digitos < 18) {
            numero = numero * 10 + (c - '0');
            ++digitos;
        } else {
            valido = false;
        }
    }
    Metricas::contar(digitos);
    return leyo && valido && digitos > 0;
}

bool UdeABook::leer(istream &entrada, Fecha &fecha) {
    unsigned long campos[5] = {0, 0, 0, 0, 0};
    unsigned int actual = 0, iteraciones = 0;
    bool enNumero = false;
    char c;
    while (entrada >> noskipws >> c) {
        ++iteraciones;
        if (c == '|' || c == '\n') break;
        if (c >= '0' && c <= '9') {
            if (actual < 5 && campos[actual] < 100000) campos[actual] = campos[actual] * 10 + (c - '0');
            enNumero = true;
        } else if (enNumero) {
            enNumero = false;
            ++actual;
        }
    }
    if (enNumero) ++actual;
    Metricas::contar(iteraciones);
    if (actual < 3 || campos[0] > 9999 || campos[1] > 12 || campos[2] > 31 || campos[3] > 23 ||
        campos[4] > 59)
        return false;
    fecha = Fecha(campos[0], campos[1], campos[2], campos[3], campos[4]);
    return fecha.esValida();
}

unsigned long UdeABook::leerOpcion() {
    unsigned long valor = 0;
    if (leer(cin, valor)) return valor;
    if (!cin) return 0;
    return 1000000;
}

void UdeABook::escribir(ostream &salida, unsigned long numero, unsigned char ancho) {
    char digitos[20];
    unsigned int n = 0;
    do {
        digitos[n++] = '0' + numero % 10;
        numero /= 10;
    } while (numero > 0);
    while (n < ancho && n < 20) digitos[n++] = '0';
    Metricas::contar(n);
    while (n > 0) salida << digitos[--n];
}

void UdeABook::escribir(ostream &salida, const Lista<char> &texto) {
    for (unsigned int i = 0; i < texto.getTam(); ++i) salida << texto[i];
    Metricas::contar(texto.getTam());
}

void UdeABook::escribir(ostream &salida, const Fecha &fecha) {
    escribir(salida, fecha.getAnio(), 4);
    salida << '-';
    escribir(salida, fecha.getMes(), 2);
    salida << '-';
    escribir(salida, fecha.getDia(), 2);
    salida << ' ';
    escribir(salida, fecha.getHora(), 2);
    salida << ':';
    escribir(salida, fecha.getMinuto(), 2);
}
