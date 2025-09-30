# Conceptos básicos de redes

Comprender los conceptos fundamentales de red es clave para usar la librería con soltura. Aquí verás IPs, puertos, protocolos y sockets, con notas prácticas para C/C++.

## 1) Direcciones IP

Una **dirección IP** identifica de forma única a un dispositivo en la red.

- **IPv4**: cuatro números decimales (0–255) separados por puntos, ej. `192.168.1.10`.
- **IPv6**: ocho grupos hexadecimales, ej. `2001:0db8:85a3::8a2e:0370:7334`.

En esta librería se usa IPv4: `inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)`.

## 2) Puertos

Un **puerto** (0–65535) distingue servicios en un mismo host. Ejemplos conocidos: HTTP 80, HTTPS 443. Cliente y servidor deben coincidir en puerto.

## 3) Protocolos

- **TCP (Transmission Control Protocol)**: orientado a conexión, fiable (entrega y orden garantizados). Requiere conexión previa (three-way handshake) y usa control de congestión.
- **UDP (User Datagram Protocol)**: no orientado a conexión, más rápido pero sin garantías. Útil para latencia baja y tolerancia a pérdida.

Esta librería usa TCP (sockets de flujo/stream).

## 4) Sockets

Un **socket** es un extremo de comunicación (IP + puerto). En C se trabaja con un descriptor de archivo (int) que se usa con `read`/`write` o `recv`/`send`.

Tipos:

- **Stream (TCP)**: fiable, orientado a conexión.
- **Datagram (UDP)**: sin conexión, posible pérdida/reorden.

## 5) Endianness y tamaño de datos

Entre máquinas diferentes, el orden de bytes puede variar (little/big endian). En esta librería, los mensajes se mandan como bytes crudos. Si defines tus propios tipos/estructuras, considera:

- Normalizar a un endianness (por ejemplo, big-endian) o usar funciones de conversión (`htons/htonl` y `ntohs/ntohl` para 16/32 bits).
- Empaquetar datos como bytes con `pack`/`unpack` si envías tipos compuestos.

## 5.1) Tipos de datos y serialización

**Tipos triviales**: `int`, `float`, `char`, etc. Se pueden copiar bit a bit sin problemas.

**Tipos no triviales**: `std::string`, `std::vector`, clases con punteros. Requieren serialización manual.

Para `std::string`, el patrón estándar es:

1. Enviar el tamaño (como `size_t`)
2. Enviar los datos (como array de `char`)

```cpp
// Envío
pack(buffer, mensaje.size());           // tamaño
packv(buffer, mensaje.data(), mensaje.size()); // datos

// Recepción  
size_t tamaño = unpack<size_t>(buffer); // tamaño
mensaje.resize(tamaño);                 // reservar espacio
unpackv(buffer, mensaje.data(), tamaño); // datos
```

## 6) Conexión cliente-servidor

Flujo típico TCP:

1. Servidor crea socket, `bind` a puerto, `listen`.  
2. Cliente crea socket y `connect` a IP:puerto.
3. Servidor acepta con `accept` y obtiene un nuevo descriptor para ese cliente.
4. Intercambio de datos con `read`/`write`.
5. Cierre con `close`.

## 7) Concurrencia: threads y mutex

Las aplicaciones de red modernas requieren concurrencia para manejar múltiples clientes simultáneamente.

### Threads (`std::thread`)

Un hilo es una unidad de ejecución independiente que permite que tu programa haga varias cosas a la vez:

```cpp
#include <thread>

// Crear y lanzar un hilo
std::thread miHilo(función, parámetros...);

// Opciones para gestionar el hilo:
miHilo.join();   // Esperar a que termine
miHilo.detach(); // Dejarlo correr independientemente
```

### Mutex (`std::mutex`)

Un mutex (mutual exclusion) protege datos compartidos entre hilos para evitar condiciones de carrera:

```cpp
#include <mutex>

std::mutex miMutex;
int datoCompartido = 0;

void funcionSegura() {
    miMutex.lock();
    datoCompartido++;  // Solo un hilo puede ejecutar esto a la vez
    miMutex.unlock();
}

// Mejor: usar lock_guard para liberación automática
void funcionMejor() {
    std::lock_guard<std::mutex> lock(miMutex);
    datoCompartido++;  // Se libera automáticamente al salir del scope
}
```

### En esta librería

- `contador_mutex`: protege el contador de IDs entre hilos
- Cada cliente puede tener su propio hilo receptor con `recvMSGAsync`
- `clientList` se accede desde múltiples hilos, pero la librería maneja la sincronización básica

## 8) Bloqueo y asincronía

Las llamadas de socket por defecto son bloqueantes (se quedan esperando). Para no bloquear el hilo principal puedes:

- Usar hilos (`std::thread`) para recepción asíncrona (la librería ofrece `recvMSGAsync`).
- Diseñar bucles de evento/polling si lo prefieres.

Con estos conceptos podrás entender mejor cómo funciona la librería y depurar problemas de red.
