# Client Guide for Networking Utils Library

## Introduction

This guide provides step-by-step instructions on how to utilize the client functionalities of the Networking Utils library. It is designed for users with basic knowledge of C++ and aims to help you understand how to initialize a client, connect to a server, send and receive messages, and handle potential errors.

## Table of Contents

1. [Initializing a Client](#initializing-a-client)
2. [Connecting to a Server](#connecting-to-a-server)
3. [Sending Messages](#sending-messages)
4. [Receiving Messages](#receiving-messages)
5. [Error Handling](#error-handling)

## Initializing a Client

To start using the client functionalities, you need to initialize a client connection. This is done using the `initClient` function, which requires the server's hostname (or IP address) and port number.

### Example

```cpp
connection_t client = initClient("127.0.0.1", 8080);
if (client.socket == -1) {
    // Handle connection error
}
```

## Connecting to a Server

Once the client is initialized, it attempts to connect to the specified server. The connection is established using the TCP protocol, which ensures reliable communication.

### Key Concepts

- **TCP (Transmission Control Protocol)**: A connection-oriented protocol that guarantees the delivery of data packets in the same order they were sent.
- **Socket**: An endpoint for sending or receiving data across a network.

## Sending Messages

To send messages to the server, you can use the `sendMSG` function. This function takes the client ID and a vector of data to be sent.

### Example

```cpp
std::vector<std::string> messages = {"Hello, Server!"};
sendMSG(client.id, messages);
```

## Receiving Messages

To receive messages from the server, use the `recvMSG` function. This function reads data from the server and stores it in a vector.

### Example

```cpp
std::vector<std::string> receivedMessages;
recvMSG(client.id, receivedMessages);
for (const auto& msg : receivedMessages) {
    std::cout << "Received: " << msg << std::endl;
}
```

## Error Handling

Error handling is crucial in networking applications. The library provides mechanisms to check for errors during connection, sending, and receiving messages.

### Common Errors

- **Connection Failed**: If the connection to the server fails, the `socket` field in the `connection_t` structure will be set to -1.
- **Message Transmission Errors**: Always check the return values of `sendMSG` and `recvMSG` to ensure that messages are sent and received successfully.

### Example

```cpp
if (client.socket == -1) {
    std::cerr << "Error: Unable to connect to the server." << std::endl;
}
```

## Conclusion

This guide has provided an overview of how to use the client functionalities of the Networking Utils library. By following the steps outlined above, you should be able to establish a connection to a server, send and receive messages, and handle errors effectively. For more advanced usage, refer to the [Advanced Usage Guide](advanced-usage.md).