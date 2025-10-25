//
// Created by jorge on 10/2/25.
//
#include "utils.h"

#ifndef CLIENT_CLIENTMANAGER_H
#define CLIENT_CLIENTMANAGER_H

#include <map>
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <atomic>

#define ERRORLOG(msg) std::cerr << "[ERROR] " << msg << " (" << __FILE__ << ":" << __LINE__ << ")" << std::endl;


class clientManager {

public:

    typedef enum {
        texto = 0,
        exit = 1,
        login = 2,
        ack = 3,
        shutdown = 4,
        texto_privado = 5 // Nuevo tipo para mensajes privados
    } msgTypes;

    static inline std::atomic<bool> cierreDePrograma = false;
    static inline std::vector<unsigned char> bufferAcks;
    static inline std::vector<unsigned char> bufferTxt;
    static inline std::mutex cerrojoBuffers;

    static inline std::map<int, std::thread*> clientThreads; // Guardar ID junto con thread
    static inline std::mutex cerrojoThreads;

    static inline std::map<std::string, int> connectionIds;

    static void enviarMensaje(int id, std::string msg);
    static std::string desempaquetaTipoTexto(std::vector<unsigned char> &buffer);
    static void enviaLogin(int id, std::string userName );
    static void atiendeCliente(int clientID);
    static void reenviaTexto(std::string userName, std::string msg);

    // Nuevas funciones para mensajes privados
    static void reenviaTextoPrivado(std::string userName, std::string msg, const std::string& recipient);
    static void enviarMensajePrivado(int id, std::string msg, std::string recipient);

    static void enviarApagado(int id);
};

#endif //CLIENT_CLIENTMANAGER_H