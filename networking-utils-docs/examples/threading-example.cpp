/**
 * Ejemplo de servidor multihilo con gestión segura de threads y mutex
 * Demuestra conceptos de concurrencia aplicados a networking
 */

#include "../utils.h"
#include <thread>
#include <mutex>
#include <map>
#include <atomic>
#include <iostream>
#include <chrono>

// Datos compartidos protegidos por mutex
std::mutex usuarios_mutex;
std::map<int, std::string> nombresUsuarios;
std::atomic<int> contadorMensajes{0};

/**
 * Registra un usuario de forma thread-safe
 */
void registrarUsuario(int clientID, const std::string& nombre) {
    std::lock_guard<std::mutex> lock(usuarios_mutex);
    nombresUsuarios[clientID] = nombre;
    std::cout << "Usuario registrado: " << nombre << " (ID: " << clientID << ")" << std::endl;
}

/**
 * Obtiene el nombre de un usuario de forma thread-safe
 */
std::string obtenerNombre(int clientID) {
    std::lock_guard<std::mutex> lock(usuarios_mutex);
    auto it = nombresUsuarios.find(clientID);
    return (it != nombresUsuarios.end()) ? it->second : "Desconocido";
}

/**
 * Remueve un usuario de forma thread-safe
 */
void removerUsuario(int clientID) {
    std::lock_guard<std::mutex> lock(usuarios_mutex);
    auto it = nombresUsuarios.find(clientID);
    if (it != nombresUsuarios.end()) {
        std::cout << "Usuario desconectado: " << it->second << std::endl;
        nombresUsuarios.erase(it);
    }
}

/**
 * Función que atiende a un cliente en su propio hilo
 */
void atenderCliente(int clientID) {
    std::cout << "Hilo iniciado para cliente " << clientID << std::endl;
    
    try {
        // 1. Recibir nombre de usuario
        std::vector<unsigned char> buffer;
        recvMSG(clientID, buffer);
        
        std::string nombre;
        nombre.resize(unpack<size_t>(buffer));
        unpackv(buffer, (char*)nombre.data(), nombre.size());
        
        registrarUsuario(clientID, nombre);
        
        // 2. Enviar confirmación
        buffer.clear();
        std::string confirmacion = "Bienvenido " + nombre;
        pack(buffer, confirmacion.size());
        packv(buffer, confirmacion.data(), confirmacion.size());
        sendMSG(clientID, buffer);
        
        // 3. Procesar mensajes del cliente
        while (clientList[clientID].alive) {
            buffer.clear();
            recvMSG(clientID, buffer);
            
            std::string mensaje;
            mensaje.resize(unpack<size_t>(buffer));
            unpackv(buffer, (char*)mensaje.data(), mensaje.size());
            
            if (mensaje == "salir") {
                break;
            }
            
            // Incrementar contador de forma atómica
            int numMensaje = ++contadorMensajes;
            
            std::cout << "[Mensaje #" << numMensaje << "] " 
                     << obtenerNombre(clientID) << ": " << mensaje << std::endl;
            
            // Responder al cliente
            buffer.clear();
            std::string respuesta = "Mensaje #" + std::to_string(numMensaje) + " recibido";
            pack(buffer, respuesta.size());
            packv(buffer, respuesta.data(), respuesta.size());
            sendMSG(clientID, buffer);
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error en cliente " << clientID << ": " << e.what() << std::endl;
    }
    
    // Limpiar y cerrar
    removerUsuario(clientID);
    closeConnection(clientID);
    std::cout << "Hilo terminado para cliente " << clientID << std::endl;
}

/**
 * Clase para gestionar hilos de forma segura
 */
class GestorHilos {
private:
    std::vector<std::thread> hilos;
    std::mutex hilos_mutex;
    
public:
    void agregarHilo(std::thread&& hilo) {
        std::lock_guard<std::mutex> lock(hilos_mutex);
        hilos.push_back(std::move(hilo));
    }
    
    size_t contarHilos() {
        std::lock_guard<std::mutex> lock(hilos_mutex);
        return hilos.size();
    }
    
    ~GestorHilos() {
        std::cout << "Esperando a que terminen " << hilos.size() << " hilos..." << std::endl;
        for (auto& hilo : hilos) {
            if (hilo.joinable()) {
                hilo.join();
            }
        }
        std::cout << "Todos los hilos han terminado." << std::endl;
    }
};

int main() {
    std::cout << "=== Servidor Multihilo con Mutex ===\n" << std::endl;
    
    // Inicializar servidor
    int server_fd = initServer(8080);
    if (server_fd < 0) {
        std::cerr << "Error al inicializar servidor" << std::endl;
        return 1;
    }
    
    std::cout << "Servidor iniciado en puerto 8080" << std::endl;
    std::cout << "Presiona Ctrl+C para terminar\n" << std::endl;
    
    GestorHilos gestor;
    
    // Bucle principal
    while (true) {
        // Verificar nuevos clientes
        if (checkClient()) {
            int clientID = getLastClientID();
            std::cout << "Nuevo cliente conectado: ID " << clientID << std::endl;
            
            // Crear hilo para atender al cliente
            std::thread clientThread(atenderCliente, clientID);
            gestor.agregarHilo(std::move(clientThread));
            
            std::cout << "Hilos activos: " << gestor.contarHilos() << std::endl;
        }
        
        // Dormir brevemente para no saturar CPU
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    return 0;
}

/*
CONCEPTOS DEMOSTRADOS:

1. std::mutex - Protege datos compartidos (nombresUsuarios)
2. std::lock_guard - RAII para bloqueos automáticos
3. std::atomic - Contador thread-safe sin mutex
4. std::thread - Crear hilos para cada cliente
5. RAII - Gestión automática de recursos en destructor
6. try-catch - Manejo de errores en contexto multihilo

PARA COMPILAR:
g++ -pthread threading-example.cpp ../utils.cpp -o threading-example

PARA PROBAR:
1. Ejecutar el servidor: ./threading-example
2. Conectar múltiples clientes con telnet o un cliente personalizado
3. Observar cómo se manejan múltiples conexiones simultáneamente
*/