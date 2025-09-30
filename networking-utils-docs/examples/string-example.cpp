/**
 * Ejemplo de manejo de strings con la librería Networking Utils
 * Basado en client.cpp y server.cpp del proyecto
 */

#include "../utils.h"
#include <string>
#include <iostream>

using namespace std;

/**
 * Envía un string al servidor/cliente especificado
 * Patrón: tamaño (size_t) + datos (char[])
 */
void enviarString(int clientID, const string& mensaje) {
    vector<unsigned char> buffer;
    
    // 1. Empaquetar el tamaño del string
    pack(buffer, mensaje.size());
    
    // 2. Empaquetar los datos del string
    packv(buffer, mensaje.data(), mensaje.size());
    
    // 3. Enviar el buffer empaquetado
    sendMSG(clientID, buffer);
    
    cout << "Enviado: \"" << mensaje << "\" (" << mensaje.size() << " bytes)" << endl;
}

/**
 * Recibe un string del servidor/cliente especificado
 * Debe coincidir con el patrón de envío
 */
string recibirString(int clientID) {
    vector<unsigned char> buffer;
    string mensaje;
    
    // 1. Recibir el buffer completo
    recvMSG(clientID, buffer);
    
    // 2. Desempaquetar el tamaño
    size_t tamaño = unpack<size_t>(buffer);
    
    // 3. Redimensionar el string al tamaño correcto
    mensaje.resize(tamaño);
    
    // 4. Desempaquetar los datos del string
    unpackv(buffer, (char*)mensaje.data(), mensaje.size());
    
    cout << "Recibido: \"" << mensaje << "\" (" << tamaño << " bytes)" << endl;
    
    return mensaje;
}

/**
 * Envía un string con confirmación (ACK)
 * Como en client.cpp: enviar mensaje + esperar confirmación
 */
bool enviarStringConACK(int clientID, const string& mensaje) {
    // Enviar el mensaje
    enviarString(clientID, mensaje);
    
    // Esperar ACK
    vector<unsigned char> buffer;
    recvMSG(clientID, buffer);
    
    int ack = unpack<int>(buffer);
    
    if (ack == 1) {
        cout << "✓ Mensaje confirmado por el receptor" << endl;
        return true;
    } else {
        cout << "✗ Error: ACK = " << ack << endl;
        return false;
    }
}

/**
 * Recibe un string y envía confirmación (ACK)
 * Como en server.cpp: recibir mensaje + enviar ACK
 */
string recibirStringConACK(int clientID) {
    // Recibir el mensaje
    string mensaje = recibirString(clientID);
    
    // Enviar ACK
    vector<unsigned char> buffer;
    int ack = 1; // Confirmación exitosa
    pack(buffer, ack);
    sendMSG(clientID, buffer);
    
    cout << "✓ ACK enviado al remitente" << endl;
    
    return mensaje;
}

/**
 * Ejemplo de protocolo multi-mensaje como en client.cpp
 * Envía: userName + mensaje en secuencia
 */
void enviarMensajeCompleto(int serverID, const string& userName, const string& mensaje) {
    cout << "\n=== Enviando mensaje completo ===" << endl;
    
    // 1. Enviar nombre de usuario
    enviarStringConACK(serverID, userName);
    
    // 2. Enviar mensaje
    enviarStringConACK(serverID, mensaje);
    
    // 3. Recibir respuesta del servidor
    string respuesta = recibirStringConACK(serverID);
    cout << "Respuesta del servidor: " << respuesta << endl;
}

/**
 * Función de servidor que procesa el protocolo multi-mensaje
 */
void procesarMensajeCompleto(int clientID) {
    cout << "\n=== Procesando mensaje completo ===" << endl;
    
    // 1. Recibir nombre de usuario
    string userName = recibirStringConACK(clientID);
    
    // 2. Recibir mensaje
    string mensaje = recibirStringConACK(clientID);
    
    // 3. Procesar y responder
    string respuesta = "Hola " + userName + ", recibí tu mensaje: '" + mensaje + "'";
    enviarStringConACK(clientID, respuesta);
    
    cout << "Usuario: " << userName << " | Mensaje: " << mensaje << endl;
}

/**
 * Muestra errores comunes al manejar strings
 */
void ejemplosErroresComunes() {
    cout << "\n=== Errores comunes con strings ===" << endl;
    
    // ❌ ERROR: Intentar enviar string directamente
    // string mensaje = "Hola";
    // sendMSG(clientID, mensaje); // ¡NO FUNCIONA!
    
    cout << "❌ No uses sendMSG directamente con std::string" << endl;
    cout << "✓ Usa pack(tamaño) + packv(datos) + sendMSG(buffer)" << endl;
    
    // ❌ ERROR: Desempaquetar en orden incorrecto
    cout << "❌ No cambies el orden: pack(A), pack(B) != unpack(B), unpack(A)" << endl;
    cout << "✓ El orden de unpack debe coincidir con pack" << endl;
    
    // ❌ ERROR: No redimensionar antes de unpackv
    cout << "❌ No olvides mensaje.resize(tamaño) antes de unpackv" << endl;
    cout << "✓ Siempre redimensiona el string al tamaño correcto" << endl;
}

int main() {
    cout << "=== Ejemplo de manejo de strings en Networking Utils ===" << endl;
    
    ejemplosErroresComunes();
    
    cout << "\nEste ejemplo muestra las funciones para enviar/recibir strings." << endl;
    cout << "Para un ejemplo completo cliente-servidor, consulta:" << endl;
    cout << "- client.cpp: función chat() y enviarMensaje()" << endl;
    cout << "- server.cpp: función atiendeCliente()" << endl;
    
    return 0;
}