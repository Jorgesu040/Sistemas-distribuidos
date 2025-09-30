# Data Structures in Networking Utils

This document describes the key data structures used in the Networking Utils library. Understanding these structures is essential for effectively utilizing the library's functionalities, especially for those new to networking concepts.

## 1. `msg_t`

The `msg_t` structure is used to represent a message that can be sent or received over the network. It contains the following fields:

- **size**: An integer that indicates the size of the message data in bytes. This field is crucial for determining how much data to read or write during network operations.
  
- **data**: A pointer to an array of unsigned characters (`unsigned char*`). This array holds the actual message content. The use of `unsigned char` allows for the representation of raw binary data, which is essential for various types of messages, including text, images, or any other binary format.

### Usage

The `msg_t` structure is typically used in conjunction with functions that handle message transmission. For example, when sending a message, the size of the message is first sent, followed by the actual data. When receiving a message, the size is read first to allocate the appropriate amount of memory for the incoming data.

## 2. `connection_t`

The `connection_t` structure represents a connection to a client or server. It contains several important fields:

- **id**: An unsigned integer that uniquely identifies the connection. This ID is essential for managing multiple connections and for referencing specific clients in the system.

- **serverId**: An unsigned integer that indicates the ID of the server to which this connection belongs. This is particularly useful in scenarios where multiple servers are running, allowing for better management of connections.

- **socket**: An integer that represents the socket file descriptor associated with the connection. This descriptor is used for all read and write operations on the network.

- **buffer**: A pointer to a list of `msg_t*`. This buffer holds messages that are pending to be processed. It allows for asynchronous message handling, where messages can be queued and processed at a later time.

- **alive**: A boolean value that indicates whether the connection is still active. This field is important for managing connection lifecycles and ensuring that resources are properly released when a connection is closed.

### Usage

The `connection_t` structure is used throughout the library to manage client-server interactions. When a new client connects, a `connection_t` instance is created to represent that connection. This structure is then used to send and receive messages, manage the connection state, and handle any errors that may occur.

## Conclusion

Understanding the `msg_t` and `connection_t` structures is fundamental for working with the Networking Utils library. These structures encapsulate the essential data needed for message transmission and connection management, enabling developers to build robust networking applications. As you explore the library further, you'll see how these data structures interact with various functions to facilitate communication between clients and servers.