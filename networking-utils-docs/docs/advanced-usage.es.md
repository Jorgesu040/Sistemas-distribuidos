# Uso avanzado de la librería de redes

Técnicas para asincronía, multihilo, sincronización y rendimiento.

## 1) Manejo asíncrono de mensajes

Lanza hilos receptores para mantener el hilo principal libre:

```cpp
connection_t conn = clientList[clientID];
std::thread t(recvMSGAsync, conn);
t.detach();
```

**Cuidado**: `recvMSGAsync` recibe `conn` por valor, el `buffer` es compartido pero `alive` es una copia.

## 2) Multihilo y sincronización

### Patrón servidor multihilo

```cpp
#include <thread>
#include <mutex>

std::mutex clientes_mutex; // Para proteger acceso a estructuras compartidas

void atenderCliente(int clientID) {
    // Cada cliente en su propio hilo
    while (clientList[clientID].alive) {
        // Procesar mensajes del cliente
        if (checkPendingMessages(clientID)) {
            std::vector<unsigned char> pkt;
            getMSG(clientID, pkt);
            // procesar pkt
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

int main() {
    int server_fd = initServer(8080);
    
    while (true) {
        if (checkClient()) {
            int clientID = getLastClientID();
            
            // Lanzar hilo para este cliente
            std::thread clientThread(atenderCliente, clientID);
            clientThread.detach(); // Dejar que corra independientemente
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}
```

### Gestión segura de hilos

```cpp
#include <vector>
#include <thread>

class ServidorSeguro {
private:
    std::vector<std::thread> hilosClientes;
    std::mutex estado_mutex;
    bool ejecutando = true;
    
public:
    void pararServidor() {
        std::lock_guard<std::mutex> lock(estado_mutex);
        ejecutando = false;
        
        // Esperar a que terminen todos los hilos
        for (auto& hilo : hilosClientes) {
            if (hilo.joinable()) {
                hilo.join();
            }
        }
    }
    
    void nuevoCliente(int clientID) {
        std::lock_guard<std::mutex> lock(estado_mutex);
        if (ejecutando) {
            hilosClientes.emplace_back(atenderCliente, clientID);
        }
    }
};
```

### Protección de datos compartidos

La librería usa `contador_mutex` internamente, pero si añades tus propias estructuras:

```cpp
#include <mutex>
#include <map>

std::mutex usuarios_mutex;
std::map<int, std::string> nombresUsuarios;

void registrarUsuario(int clientID, const std::string& nombre) {
    std::lock_guard<std::mutex> lock(usuarios_mutex);
    nombresUsuarios[clientID] = nombre;
}

std::string obtenerNombre(int clientID) {
    std::lock_guard<std::mutex> lock(usuarios_mutex);
    auto it = nombresUsuarios.find(clientID);
    return (it != nombresUsuarios.end()) ? it->second : "Desconocido";
}
```

## 4) Mejores prácticas de threading

### Gestión de hilos con RAII

```cpp
class GestorHilos {
private:
    std::vector<std::thread> hilos;
    std::mutex hilos_mutex;
    
public:
    void agregarHilo(std::thread&& hilo) {
        std::lock_guard<std::mutex> lock(hilos_mutex);
        hilos.push_back(std::move(hilo));
    }
    
    ~GestorHilos() {
        // Esperar a que terminen todos los hilos
        for (auto& hilo : hilos) {
            if (hilo.joinable()) {
                hilo.join();
            }
        }
    }
};
```

### Evitar condiciones de carrera

```cpp
// ❌ Peligroso: acceso sin protección
if (clientList.find(clientID) != clientList.end()) {
    // El cliente podría desconectarse aquí
    sendMSG(clientID, data); // ¡Puede fallar!
}

// ✓ Mejor: verificar dentro de try-catch o usar mutex
try {
    if (clientList.find(clientID) != clientList.end()) {
        sendMSG(clientID, data);
    }
} catch (...) {
    std::cerr << "Cliente " << clientID << " desconectado" << std::endl;
}
```

### Comunicación entre hilos

```cpp
#include <queue>
#include <condition_variable>

template<typename T>
class ColaSegura {
private:
    std::queue<T> cola;
    std::mutex mutex;
    std::condition_variable condicion;
    
public:
    void push(T item) {
        std::lock_guard<std::mutex> lock(mutex);
        cola.push(item);
        condicion.notify_one();
    }
    
    T pop() {
        std::unique_lock<std::mutex> lock(mutex);
        condicion.wait(lock, [this] { return !cola.empty(); });
        T item = cola.front();
        cola.pop();
        return item;
    }
};
```

## 5) Optimización de rendimiento

- **Buffers**: reutiliza `std::vector<unsigned char>` para evitar realocaciones.
- **Batching**: agrupa mensajes pequeños en uno mayor cuando tenga sentido.
- **Copias**: evita copias innecesarias; trabaja por referencia donde sea seguro.
- **Errores**: gestiona errores de socket (retornos negativos, `read`=0) para limpiar rápido y no dejar hilos colgados.

## 6) Cierre ordenado

Secuencia recomendada:

1. Deja de aceptar nuevos mensajes del cliente (marca una condición de parada propia).
2. Drena el buffer si necesitas procesar lo pendiente.
3. `closeConnection(clientID)` para cerrar el socket y liberar memoria.

## 7) Limitaciones y consideraciones

- `recvMSGAsync` comprueba `alive` en una copia; si necesitas parada estricta, crea tu propio hilo receptor que revise `clientList[id].alive` o un `std::atomic<bool>` compartido.
- `sendMSG/recvMSG` no serializan tipos complejos automáticamente; usa `pack/unpack` para estructuras.
