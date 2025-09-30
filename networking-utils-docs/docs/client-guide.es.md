# Guía de cliente de Networking Utils

Pensada para quien viene de C/Java y empieza con C++. Verás cómo conectarte a un servidor, enviar/recibir datos y manejar asincronía.

## 1) Inicializar un cliente

```cpp
#include "utils.h"

int main() {
    connection_t conn = initClient("127.0.0.1", 8080);
    if (conn.socket == -1) {
        std::cerr << "No se pudo conectar" << std::endl;
        return 1;
    }
    int clientID = conn.serverId; // IMPORTANTE: usa serverId como ID real
    // ...
}
```

## 2) Enviar mensajes (tipos triviales)

`sendMSG` funciona con tipos triviales (POD) que puedan copiarse como bytes: `int`, `float`, `unsigned char`, etc. No uses `std::string` ni objetos complejos directamente.

```cpp
std::vector<int> numeros = {1, 2, 3, 4};
sendMSG<int>(clientID, numeros);
```

Para bytes crudos:

```cpp
std::vector<unsigned char> bytes = {'H','o','l','a'};
sendMSG<unsigned char>(clientID, bytes);
```

### Envío de strings

Para `std::string`, usa el patrón tamaño + datos (ver [Guía de mensajería](messaging-guide.es.md#manejo-de-strings) para detalles completos):

```cpp
std::vector<unsigned char> buffer;
pack(buffer, mensaje.size());
packv(buffer, mensaje.data(), mensaje.size());
sendMSG(clientID, buffer);
```

## 3) Recibir mensajes (sincrónico)

`recvMSG` bloquea hasta recibir un mensaje completo (primero lee el tamaño, luego el payload).

```cpp
std::vector<int> recibidos;
recvMSG<int>(clientID, recibidos);
for (int v : recibidos) std::cout << v << "\n";
```

### Recepción de strings

Para `std::string`, desempaqueta siguiendo el mismo patrón:

```cpp
std::vector<unsigned char> buffer;
recvMSG(clientID, buffer);
size_t tamaño = unpack<size_t>(buffer);
mensaje.resize(tamaño);
unpackv(buffer, (char*)mensaje.data(), tamaño);
```

## 4) Recepción asíncrona (threading)

Para no bloquear el hilo principal, usa `recvMSGAsync` en un hilo separado:

```cpp
#include <thread>

// Lanzar hilo receptor
std::thread t(recvMSGAsync, conn);
t.detach(); // Dejar que corra independientemente

// Consumir mensajes del buffer (no bloqueante)
std::vector<unsigned char> msg;
if (checkPendingMessages(clientID)) {
    getMSG(clientID, msg);
    // procesar msg
}
```

**Importante**: `recvMSGAsync` recibe una copia de `connection_t`. El `buffer` es compartido pero el flag `alive` no se actualiza automáticamente.

## 5) Empaquetado de datos compuestos

Para estructuras o combinaciones de tipos, usa los helpers:

- `pack(packet, dato)` y `packv(packet, ptr, n)` para construir un `std::vector<unsigned char>`.
- `unpack<T>(packet)` y `unpackv<T>(packet, ptr, n)` para extraer en el receptor.

Emisor:

```cpp
std::vector<unsigned char> pkt;
int a = 42; float b = 3.14f;
pack(pkt, a);
pack(pkt, b);
sendMSG<unsigned char>(clientID, pkt);
```

Receptor:

```cpp
std::vector<unsigned char> pkt;
recvMSG<unsigned char>(clientID, pkt);
float b;
int a = unpack<int>(pkt);
b = unpack<float>(pkt);
```

## 6) Cerrar la conexión

```cpp
closeConnection(clientID); // libera socket, marca no vivo y limpia mensajes pendientes
```

Si hay mensajes sin leer, se informa por consola y se liberan.

## 7) Errores comunes

- Usar `conn.id` en lugar de `conn.serverId` para enviar/recibir.
- Intentar enviar `std::string` directamente con `sendMSG` (usa `pack`/`unpack`).
- Olvidar que `recvMSG` bloquea (diseña la concurrencia en consecuencia).
- Desempaqueter en orden diferente al empaquetado (el orden importa).
- No redimensionar el string antes de `unpackv` (causará corrupción de memoria).
