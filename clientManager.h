//
// Created by jorge on 10/2/25.
//

#ifndef CLIENT_CLIENTMANAGER_H
#define CLIENT_CLIENTMANAGER_H

#include <map>
#include <bits/stl_vector.h>
#include <bits/stringfwd.h>


class clientManager {

public:

    typedef enum {
        texto = 0,
        exit = 1,
        login = 2,
        ack = 3
    } msgTypes;

    static inline std::map<std::string, int> connectionIds;

    static void enviarMensaje(int id, std::string msg);
    static std::string desempaquetaTipoTexto(std::vector<unsigned char> &buffer);

    static void enviaLogin(int id, std::string userName );
    static void recibeLogin(int id);

    static void enviarACK(int clientID, std::vector<unsigned char> buffer);

    static void atiendeCliente(int clientID);

    static void reenviaTexto(std::string userName, std::string msg);

    static std::string recibeMensaje(int serverId);
};


#endif //CLIENT_CLIENTMANAGER_H