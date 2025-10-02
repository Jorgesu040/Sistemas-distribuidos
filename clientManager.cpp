//
// Created by jorge on 10/2/25.
//

#include "clientManager.h"
#include "utils.h"
#include "iostream"

#define ERRORLOG(msg) std::cerr << "[ERROR] " << msg << " (" << __FILE__ << ":" << __LINE__ << ")" << std::endl;

using namespace std;

void clientManager::enviarMensaje(int id, string msg){

    vector<unsigned char> buffer; // Para empaquetar datos

    // Empaquetamos el tamaño porque, si por ejemplo enviamos dos mensajes:
    // ¿Como sabemos donde empieza/acaba cada mensaje? Con una cabecera que creamos con pack()
    // Unicamente podriamos saber el tamaño completo del paquete si no mandamos cabecera
    pack(buffer, msg.size());

    packv(buffer, msg.data(), msg.size());

    sendMSG(id, buffer);

    // Limpiar buffer
    buffer.clear();

    recvMSG(id, buffer);

    int ack = unpack<int>(buffer);
    if (ack != 1){
        cout<<"Error enviando mensaje";
    } else {
        cout << "Recibido correctamente ACK: " << ack << endl;
    }

}

string clientManager::desempaquetaTipoTexto (vector<unsigned char> buffer){

    string mensaje;
    mensaje.resize(unpack<long int>(buffer));
    unpackv(buffer, (char*)mensaje.data(), mensaje.size());
    return 	mensaje;

}

void clientManager::enviaLogin(int id, string userName) {

    vector<unsigned char> bufferOut;


    pack(bufferOut, login);

    pack(bufferOut, userName.size());

    packv(bufferOut, (char *)userName.data(), userName.size());

    sendMSG(id, bufferOut);

    bufferOut.clear();

    recvMSG(id, bufferOut);

    if (unpack<msgTypes>(bufferOut) != ack ) {
        ERRORLOG("Error enviando login");

    }

}

void clientManager::recibeLogin(int id) {
}

void clientManager::enviarACK(int clientID, vector<unsigned char> buffer) {
    buffer.clear();
    pack(buffer, ack);
    sendMSG(clientID, buffer);
}

// Como queremos poder atender a los clientes en paralelo y no de manera sequencial, creamos una funcion que este en varios threads
void clientManager::atiendeCliente(int clientID){

    vector<unsigned char> buffer; // Para crear un paquete de datos
    bool salir = false;
    string userName = "default";

    while (!salir) {
        // Recibir datos y crear buffer
        recvMSG(clientID, buffer);
        // Desempaquetar tipo paquete
        // Dependiendo del tipo de paquete
        switch (unpack<msgTypes>(buffer)) {
            // Tipo texto
            case texto: {
                // Desempaquetar mensaje
                string msg = desempaquetaTipoTexto(buffer);
                // Reenviar
                reenviaTexto(userName, msg);
            }break;
            // Tipo exit
            case exit: {
                // Eliminar usuario
                connectionIds.erase(userName);
                // Cerrar conexion
                salir = true;

            }break;
            // Tipo login
            case login: {
                // Desempaquetar usuario
                userName=desempaquetaTipoTexto(buffer);
                // Añadir usuario si no existe
                if (connectionIds.find(userName) == connectionIds.end()) {
                    connectionIds[userName] = clientID;
                } else {
                    ERRORLOG("El usuario ya existe");
                    salir = true;
                    break;
                }
                // Enviar acknowledge
            }
            // Default
            default: {
                // Error
                ERRORLOG("Mensaje no reconocido");
                connectionIds.erase(userName);
                salir = true;
            }
        }
    }

    enviarACK(clientID, buffer);
}

void clientManager::reenviaTexto(string userName, string msg) {
    // Buscar usuario en la lista
    // Si existe, enviar mensaje
    // Si no existe, error
}