#include <iostream>
#include <vector>
#include "utils.h"

int main() {
    // Initialize the client
    std::string host = "127.0.0.1"; // Localhost
    int port = 8080; // Port number
    connection_t connection = initClient(host, port);

    if (connection.socket < 0) {
        std::cerr << "Failed to connect to the server." << std::endl;
        return -1;
    }

    // Prepare a message to send
    std::vector<std::string> messages = {"Hello, Server!", "This is a simple client."};

    // Send messages to the server
    for (const auto& msg : messages) {
        sendMSG(connection.id, std::vector<std::string>{msg});
        std::cout << "Sent: " << msg << std::endl;
    }

    // Clean up and close the connection
    closeConnection(connection.id);
    return 0;
}