# Guía de servidor de Networking Utils

Aprende a levantar un servidor TCP, aceptar clientes y manejar varios a la vez.

## 1) Inicialización

```cpp
#include "utils.h"

int main() {
    int server_fd = initServer(8080);
    if (server_fd < 0) return 1;
    // A partir de aquí el socket está en listen y un hilo espera conexiones.
}
```

`initServer` crea el socket, configura `SO_REUSEPORT|SO_REUSEADDR`, hace `bind`, `listen(5)` y lanza un hilo `waitForConnectionsAsync(server_fd)` que acepta conexiones en bucle.

## 2) Aceptar clientes

Tienes dos opciones:

- Bloqueante (aceptar uno):

    ```cpp
    int clientSock = waitForConnections(server_fd); // devuelve descriptor del socket aceptado
    // El ID para usar en send/recv está en waitingClients -> usa getLastClientID()
    int clientID = getLastClientID();
    ```

- Asíncrono (recomendado):

    ```cpp
    // initServer ya lanzó el hilo de aceptación. Solo polling de nuevos IDs:
    if (checkClient()) {
            int clientID = getLastClientID();
            // ya puedes comunicarte con ese cliente
    }
    ```

## 3) Comunicación con clientes

Usa `clientID` (no el descriptor de socket) con las utilidades de mensajería:

```cpp
std::vector<int> datos = {10,20,30};
sendMSG<int>(clientID, datos);

std::vector<int> recibidos;
recvMSG<int>(clientID, recibidos);
```

### Manejo de strings en el servidor

Ver [Guía de mensajería](messaging-guide.es.md#manejo-de-strings) para el patrón completo. Ejemplo básico:

```cpp
void procesarMensajeCliente(int clientID) {
    std::vector<unsigned char> buffer;
    recvMSG(clientID, buffer);
    
    // Desempaquetar string
    std::string mensaje;
    mensaje.resize(unpack<size_t>(buffer));
    unpackv(buffer, (char*)mensaje.data(), mensaje.size());
    
    std::cout << "Mensaje recibido: " << mensaje << std::endl;
    
    // Enviar ACK
    buffer.clear();
    int ack = 1;
    pack(buffer, ack);
    sendMSG(clientID, buffer);
}
```

Para consumo no bloqueante de lo que llega por hilos receptores, usa `getMSG`:

```cpp
std::vector<unsigned char> pkt;
if (checkPendingMessages(clientID)) {
    getMSG<unsigned char>(clientID, pkt);
}
```

## 4) Múltiples clientes con threading

### Patrón básico: un hilo por cliente

```cpp
#include <thread>

void atenderCliente(int clientID) {
    // Lógica específica del cliente
    while (clientList[clientID].alive) {
        if (checkPendingMessages(clientID)) {
            std::vector<unsigned char> pkt;
            getMSG(clientID, pkt);
            // procesar pkt
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    std::cout << "Cliente " << clientID << " desconectado" << std::endl;
}

// En el bucle principal del servidor
while (true) {
    if (checkClient()) {
        int clientID = getLastClientID();
        std::thread clientThread(atenderCliente, clientID);
        clientThread.detach(); // Dejar que corra independientemente
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}
```

### Usando recvMSGAsync

Alternativamente, puedes usar el hilo receptor integrado:

```cpp
if (checkClient()) {
    int clientID = getLastClientID();
    connection_t conn = clientList[clientID];
    std::thread(recvMSGAsync, conn).detach();
}
```

**Ventajas del primer patrón**: Control total sobre el ciclo de vida del hilo.
**Ventajas del segundo**: Menos código, usa la funcionalidad integrada de la librería.

## 5) Cierre y limpieza

```cpp
closeConnection(clientID);
```

Esto cierra el socket, marca la conexión como no viva y limpia el buffer si quedaban mensajes.

## 6) Errores habituales

- Leer/escribir con el descriptor de socket en vez del `clientID` en las utilidades.
- No consultar `checkClient()` y perder nuevos clientes (si usas el modo asíncrono).
- Mezclar tipos al enviar/recibir (el tipo de `sendMSG<T>` debe coincidir con `recvMSG<T>`).
