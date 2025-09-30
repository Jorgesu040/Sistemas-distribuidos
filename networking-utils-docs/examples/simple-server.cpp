#include "utils.h"
#include <iostream>
#include <vector>

int main() {
    // Initialize the server on port 8080
    int port = 8080;
    int server_fd = initServer(port);
    if (server_fd < 0) {
        std::cerr << "Failed to initialize server." << std::endl;
        return -1;
    }
    std::cout << "Server is running on port " << port << std::endl;

    while (true) {
        // Wait for client connections
        int clientSocket = waitForConnections(server_fd);
        if (clientSocket < 0) {
            std::cerr << "Failed to accept client connection." << std::endl;
            continue;
        }

        std::cout << "Client connected." << std::endl;

        // Create a vector to hold the received message
        std::vector<unsigned char> receivedData;
        recvMSG<unsigned char>(clientSocket, receivedData);

        // Process the received message (for demonstration, we just print it)
        std::cout << "Received message: ";
        for (unsigned char byte : receivedData) {
            std::cout << byte;
        }
        std::cout << std::endl;

        // Send a response back to the client
        std::string response = "Message received";
        std::vector<unsigned char> responseData(response.begin(), response.end());
        sendMSG(clientSocket, responseData);

        // Close the client connection
        closeConnection(clientSocket);
        std::cout << "Client disconnected." << std::endl;
    }

    return 0;
}