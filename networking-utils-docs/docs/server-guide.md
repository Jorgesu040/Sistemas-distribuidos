# Server Guide for Networking Utils Library

## Introduction

This guide provides detailed instructions on how to set up and manage a server using the Networking Utils library. It is designed for users with basic knowledge of C++ and networking concepts. By the end of this guide, you will be able to initialize a server, accept client connections, and manage multiple clients effectively.

## Table of Contents

1. [Setting Up the Server](#setting-up-the-server)
2. [Accepting Client Connections](#accepting-client-connections)
3. [Managing Multiple Clients](#managing-multiple-clients)
4. [Error Handling](#error-handling)
5. [Conclusion](#conclusion)

## Setting Up the Server

To set up a server using the Networking Utils library, you need to follow these steps:

1. **Include the Header File**: Start by including the `utils.h` header file in your server implementation.

   ```cpp
   #include "utils.h"
   ```

2. **Initialize the Server**: Use the `initServer` function to create a socket and bind it to a specific port.

   ```cpp
   int server_fd = initServer(port);
   ```

   - **Parameters**: 
     - `port`: The port number on which the server will listen for incoming connections.

3. **Start Listening**: The server will start listening for incoming connections once initialized.

## Accepting Client Connections

Once the server is set up, you can accept client connections using the `waitForConnections` function. This function blocks until a client connects.

```cpp
int client_socket = waitForConnections(server_fd);
```

- **Returns**: A new socket descriptor for the connected client.

## Managing Multiple Clients

To handle multiple clients, you can use threads. The library provides an asynchronous way to manage connections. Here’s how to do it:

1. **Create a Thread for Each Client**: When a client connects, spawn a new thread to handle the communication with that client.

   ```cpp
   std::thread clientThread(recvMSGAsync, connection);
   clientThread.detach(); // Detach the thread to allow it to run independently
   ```

2. **Receive Messages**: Use the `recvMSGAsync` function to handle incoming messages from the client in a separate thread.

## Error Handling

Error handling is crucial in networking applications. Here are some common error scenarios:

- **Socket Creation Failure**: Check if the socket was created successfully. If not, handle the error appropriately.

  ```cpp
  if (server_fd < 0) {
      perror("Error creating socket");
      return -1;
  }
  ```

- **Connection Issues**: Always check the return values of functions like `bind`, `listen`, and `accept` to ensure they succeed.

## Conclusion

Setting up a server using the Networking Utils library is straightforward. By following the steps outlined in this guide, you can create a robust server capable of handling multiple clients. Remember to implement proper error handling to ensure your server runs smoothly.

For further details on client functionalities, refer to the [Client Guide](client-guide.md). For advanced usage, check the [Advanced Usage](advanced-usage.md) document.