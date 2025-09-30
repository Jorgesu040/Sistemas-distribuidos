# Estructuras de datos en Networking Utils

A continuación se describen las estructuras clave definidas en `utils.h`.

## `msg_t`

Representa un mensaje en bruto:
 
- `int size`: tamaño del payload en bytes.
- `unsigned char* data`: puntero a los bytes del mensaje.

Se usa para almacenar mensajes recibidos de forma asíncrona (cola/buffer) o para liberar memoria tras procesarlos.

## `connection_t`

Representa una conexión cliente-servidor:
 
- `unsigned int id`: identificador local del cliente (en clientes se inicializa a `-1`).
- `unsigned int serverId`: identificador asignado por el servidor y clave en `clientList`.
- `int socket`: descriptor del socket.
- `std::list<msg_t*>* buffer`: cola de mensajes pendientes.
- `bool alive`: marca de conexión activa.

Notas importantes:
 
- En el cliente, tras `initClient`, debes usar `connection.serverId` como ID para llamar a `sendMSG/recvMSG/getMSG` porque es el índice real en `clientList`.
- La librería mantiene un estado global: `std::map<unsigned int, connection_t> clientList`, un contador, y una lista `waitingClients` con IDs recién aceptados.

## Estado global y sincronización

La librería mantiene estado global protegido por mutex:
 
- `clientList`: mapa de `serverId` a `connection_t` (acceso desde múltiples hilos).
- `contador`: contador atómico protegido por `contador_mutex` para asignar IDs únicos.
- `waitingClients`: lista de IDs de nuevas conexiones aceptadas.

### Funciones thread-safe

 
- `checkClient()`: ¿hay clientes nuevos en espera?
- `getLastClientID()`: toma el último ID en espera.
- `checkPendingMessages(int clientID)`: ¿hay mensajes sin leer en el buffer?

### Consideraciones de concurrencia

- `clientList` se modifica desde el hilo de aceptación y se lee desde hilos de procesamiento.
- Los `buffer` de cada conexión son accedidos por el hilo receptor (`recvMSGAsync`) y el hilo que consume (`getMSG`).
- Si añades tus propias estructuras compartidas, protégelas con `std::mutex`.

Estas estructuras y utilidades habilitan un modelo sencillo de mensajería sincrónica/asíncrona sobre TCP.
