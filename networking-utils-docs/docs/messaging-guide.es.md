# Guía de mensajería en Networking Utils

Cómo enviar/recibir mensajes de forma segura y eficiente con TCP.

## Modelo de datos

Las funciones genéricas trabajan con vectores de tipos triviales:
 
- `sendMSG<T>(clientID, std::vector<T>&)`
- `recvMSG<T>(clientID, std::vector<T>&)`

El envío hace:
 
1) calcular `dataLen` (en bytes), 2) enviar `int` con el tamaño, 3) enviar el buffer.

La recepción hace:
 
1) leer `int` tamaño, 2) redimensionar vector, 3) leer exactamente `size` bytes.

## Tipos soportados directamente

Tipos triviales (POD): `char/unsigned char`, `int32_t/uint32_t`, `float/double`, etc. Evita `std::string`, `std::vector<T>` anidados, o clases con punteros.

## Manejo de strings

Los `std::string` no se pueden enviar directamente con `sendMSG` porque no son tipos triviales. Usa el patrón **tamaño + datos**:

```cpp
// Envío
std::vector<unsigned char> buffer;
pack(buffer, mensaje.size());              // tamaño
packv(buffer, mensaje.data(), mensaje.size()); // datos
sendMSG(clientID, buffer);

// Recepción
std::vector<unsigned char> buffer;
recvMSG(clientID, buffer);
size_t tamaño = unpack<size_t>(buffer);    // tamaño
mensaje.resize(tamaño);                     // reservar
unpackv(buffer, (char*)mensaje.data(), tamaño); // datos
```

**Importante**: El emisor y receptor deben usar el mismo tipo para el tamaño (`size_t`, `int`, etc.).

## Empaquetado de datos compuestos

Usa los helpers de `utils.h`:
 
- `pack(packet, dato)` añade un dato trivial a `std::vector<unsigned char>`.
- `packv(packet, ptr, n)` añade `n` elementos desde `ptr`.
- `unpack<T>(packet)` extrae el primer elemento y desplaza el buffer.
- `unpackv<T>(packet, ptr, n)` extrae `n` elementos.

Ejemplo:

```cpp
// Emisor
std::vector<unsigned char> pkt;
int32_t id = 7; float valor = 2.5f;
pack(pkt, id);
pack(pkt, valor);
sendMSG<unsigned char>(clientID, pkt);

// Receptor
std::vector<unsigned char> pkt;
recvMSG(clientID, pkt);
int32_t id2 = unpack<int32_t>(pkt);
float valor2 = unpack<float>(pkt);
```

## Recepción asíncrona y buffer

- `recvMSGAsync(connection_t)` lee en bucle y encola cada mensaje en `connection.buffer` como `msg_t*`.
- Consulta si hay pendientes con `checkPendingMessages(clientID)`.
- Obtén el siguiente mensaje (no bloqueante) con `getMSG<T>(clientID, data)`.

Nota: La función asíncrona usa una copia de `connection_t` para el chequeo de `alive`; planifica la parada del hilo cuando cierres la conexión.

## Integridad y errores

- La librería intenta leer el tamaño y luego exactamente ese número de bytes. Si se lee `0` en el tamaño, la conexión probablemente se cerró lado remoto.
- Verifica las longitudes y coincide el tipo `T` entre emisor y receptor.
- Ten en cuenta el endianness si intercambias con otras plataformas: para enteros de 16/32 bits puedes usar `htons/htonl` al empacar y `ntohs/ntohl` al desempacar.
