//
// Created by jorge on 10/2/25.
//

#include "clientManager.h"
#include "utils.h"
#include "iostream"

using namespace std;


void clientManager::enviarMensaje(int id, string msg){

    vector<unsigned char> buffer; // Para empaquetar datos

    // Empaquetamos el tamaño porque, si por ejemplo enviamos dos mensajes:
    // ¿Como sabemos donde empieza/acaba cada mensaje? Con una cabecera que creamos con pack()
    // Unicamente podriamos saber el tamaño completo del paquete si no mandamos cabecera
    
    pack(buffer, texto); // Tipo mensaje
    pack(buffer, msg.size());
    packv(buffer, msg.data(), msg.size());

    sendMSG(id, buffer); 

    // Consultar es ack
    while (bufferAcks.size() == 0) usleep(100); // Espera semiactiva
    cerrojoBuffers.lock();
    if (unpack<msgTypes>(bufferAcks) != ack ) {
        ERRORLOG("Error enviando mensaje");
    }
    bufferAcks.clear();  // Limpia después de leer
    cerrojoBuffers.unlock();

}


void clientManager::enviarMensajePrivado(int id, string msg, string recipient){

    vector<unsigned char> buffer; // Para empaquetar datos

    pack(buffer, texto_privado); // Tipo mensaje

    pack(buffer, recipient.size());
    packv(buffer, recipient.data(), recipient.size());

    pack(buffer, msg.size());
    packv(buffer, msg.data(), msg.size());

    sendMSG(id, buffer); 

    // Consultar es ack
    while (bufferAcks.size() == 0) usleep(100); // Espera semiactiva
    cerrojoBuffers.lock();
    if (unpack<msgTypes>(bufferAcks) != ack ) {
        ERRORLOG("Error enviando mensaje");
    }
    bufferAcks.clear();  // Limpia después de leer
    cerrojoBuffers.unlock();

}

void clientManager::enviarApagado(int id){

    vector<unsigned char> buffer; 

    
    pack(buffer, shutdown);
    sendMSG(id, buffer);

    buffer.clear();
    pack(buffer, ack);
    sendMSG(id, buffer);

    // Consultar ack
    while (bufferAcks.size() == 0) usleep(100); // Espera semiactiva
    cerrojoBuffers.lock();
    if (unpack<msgTypes>(bufferAcks) != ack ) {
        ERRORLOG("Error enviando mensaje de apagado");
    }
    bufferAcks.clear();  // Limpia después de leer
    cerrojoBuffers.unlock();

}

string clientManager::desempaquetaTipoTexto (vector<unsigned char> &buffer){

    string mensaje;
    mensaje.resize(unpack<long int>(buffer));
    unpackv(buffer, mensaje.data(), mensaje.size());
    return 	mensaje;

}

void clientManager::enviaLogin(int id, string userName) {

    vector<unsigned char> bufferOut;


    pack(bufferOut, login);
    pack(bufferOut, userName.size());
    packv(bufferOut, userName.data(), userName.size());

    sendMSG(id, bufferOut);

    // Consultar es ack
    while (bufferAcks.size() == 0) usleep(100); // Espera semiactiva
    cerrojoBuffers.lock();
    if (unpack<msgTypes>(bufferAcks) != ack ) {
        ERRORLOG("Error enviando login");
    }
    bufferAcks.clear();
    cerrojoBuffers.unlock();

}



// Como queremos poder atender a los clientes en paralelo y no de manera sequencial, creamos una funcion que este en varios threads
void clientManager::atiendeCliente(int clientID){

    vector<unsigned char> buffer; // Para crear un paquete de datos
    bool salir = false;
    string userName = "default";

    while (!salir && !cierreDePrograma) {

        // Recibir datos y crear buffer
        recvMSG(clientID, buffer);
        // Desempaquetar tipo paquete
        // Dependiendo del tipo de paquete
        auto type = unpack<msgTypes>(buffer);
        switch (type) {
            // Tipo texto
            case texto: {
                // Desempaquetar mensaje
                string msg = desempaquetaTipoTexto(buffer);
                // Reenviar
                reenviaTexto(userName, msg);
            }break;
            case texto_privado: {
                string receptor = desempaquetaTipoTexto(buffer);
                string msg = desempaquetaTipoTexto(buffer);

                reenviaTextoPrivado(userName, msg, receptor);
            } break;
            // Tipo exit
            case exit: {
                // Eliminar usuario
                connectionIds.erase(userName);
                // Eliminar el hilo del mapa
                cerrojoThreads.lock();
                auto it = clientThreads.find(clientID);
                if (it != clientThreads.end()) {
                    it->second->detach(); // Desvincular el hilo
                    delete it->second;    // Liberar la memoria del objeto thread
                    clientThreads.erase(it); // Eliminar del mapa
                }
                cerrojoThreads.unlock();
                salir = true;

                // Log de salida
                cout << "El usuario " << userName << " ha cerrado la conexion." << endl;

            }break;
            // Tipo login
            case login: {
                // Desempaquetar usuario
                userName=desempaquetaTipoTexto(buffer);
                // Añadir usuario si no existe
                if (connectionIds.find(userName) == connectionIds.end() && userName != "Servidor") {
                    connectionIds[userName] = clientID;
                } else {
                    ERRORLOG("El usuario ya existe");
                    salir = true;
                    break;
                }
                // Enviar acknowledge
            }break;
            // Tipo ack
            case ack: {
                cerrojoBuffers.lock();
                pack(bufferAcks, ack);
                cerrojoBuffers.unlock();
                buffer.clear();
            }break;
            // Default
            default: {
                ERRORLOG("Mensaje no reconocido. Tipo recibido: " + to_string(type));
                connectionIds.erase(userName);
                salir = true;

            }
        }

        // Si el cliente ha pedido salir, como ya cerro su conexion, no enviamos ack (lanzaria error)
        if (!salir) {
            buffer.clear();
            pack(buffer, ack);
            sendMSG(clientID, buffer);
        }
    }

    connectionIds.erase(userName);
    closeConnection(clientID);

}

void clientManager::reenviaTexto(string userName, string msg) {
    
    // Empaquetar mensaje
    vector<unsigned char> buffer;
    pack(buffer, texto); // Tipo mensaje

    pack(buffer, userName.size()); // Tamaño mensaje
    packv(buffer, userName.data(), userName.size()); // Datos mensaje

    pack(buffer, msg.size());
    packv(buffer, (char*)msg.data(), msg.size());


    for (const auto& client : connectionIds) {
        if (client.first != userName)
            sendMSG(client.second, buffer);
    }
    buffer.clear(); 
}


void clientManager::reenviaTextoPrivado(string userName, string msg, const string& recipient) {

    vector<unsigned char> buffer;

    // Comprobar si el destinatario existe
    auto it = connectionIds.find(recipient);

    if (it == connectionIds.end()) {
        // Usuario no encontrado - enviar mensaje de error al remitente
        string server = "Servidor";
        string errorMsg = "Error, Usuario '" + recipient + "' no encontrado";
        pack(buffer, texto);

        pack(buffer, server.size()); // Tamaño mensaje
        packv(buffer, server.data(), server.size()); // Datos mensaje

        pack(buffer, errorMsg.size());
        packv(buffer, errorMsg.data(), errorMsg.size());
        sendMSG(connectionIds[userName], buffer);

    } else if (it == connectionIds.find(userName)) {
        string server = "Servidor";
        string errorMsg = "Error, no puedes enviarte mensajes a ti mismo";
        pack(buffer, texto);

        pack(buffer, server.size()); // Tamaño mensaje
        packv(buffer, server.data(), server.size()); // Datos mensaje

        pack(buffer, errorMsg.size());
        packv(buffer, errorMsg.data(), errorMsg.size());
        sendMSG(connectionIds[userName], buffer);
    
    } else {
        userName = "Mensaje privado de "+ userName; 
        pack(buffer, texto); // Tipo mensaje

        pack(buffer, userName.size()); // Tamaño mensaje
        packv(buffer, userName.data(), userName.size()); // Datos mensaje

        pack(buffer, msg.size());
        packv(buffer, (char*)msg.data(), msg.size());

        sendMSG(connectionIds[recipient], buffer);
    }

   
    buffer.clear(); 
}