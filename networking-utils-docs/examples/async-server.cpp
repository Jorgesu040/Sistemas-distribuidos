#include "utils.h"
#include <iostream>
#include <thread>
#include <vector>

void handleClient(int clientID) {
    std::vector<unsigned char> data;
    while (true) {
        recvMSG(clientID, data);
        if (data.empty()) {
            std::cout << "Client " << clientID << " disconnected." << std::endl;
            break;
        }
        std::cout << "Received message from client " << clientID << ": " << data.data() << std::endl;

        // Echo the message back to the client
        sendMSG(clientID, data);
    }
}

int main() {
    int port = 8080; // Specify the port number
    int server_fd = initServer(port);
    std::cout << "Server is running on port " << port << std::endl;

    while (true) {
        int clientID = waitForConnections(server_fd);
        std::cout << "New client connected: " << clientID << std::endl;

        // Start a new thread to handle the client
        std::thread clientThread(handleClient, clientID);
        clientThread.detach(); // Detach the thread to allow it to run independently
    }

    closeConnection(server_fd);
    return 0;
}