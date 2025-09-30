# Messaging Guide for Networking Utils Library

## Introduction

The Messaging Guide provides an overview of how to effectively use the messaging functionalities of the Networking Utils library. This guide is designed for users with basic knowledge of C++ and networking concepts, and it will help you understand how to send and receive messages, manage message buffers, and ensure message integrity in your applications.

## Key Concepts

### 1. Message Structure

In the Networking Utils library, messages are encapsulated in a structure called `msg_t`. This structure contains two main fields:

- **size**: An integer representing the size of the message data.
- **data**: A pointer to an array of unsigned characters that holds the actual message content.

This structure allows for flexible message handling, enabling the transmission of various data types.

### 2. Sending Messages

To send messages to a connected client, you can use the `sendMSG` function. This function takes the client ID and a vector of data as parameters. It performs the following steps:

1. **Calculate the Data Length**: The function computes the total size of the data to be sent.
2. **Send the Size**: It first sends the size of the data as an integer to the client.
3. **Send the Data**: Finally, it sends the actual data buffer.

Example usage:
```cpp
std::vector<int> dataToSend = {1, 2, 3, 4, 5};
sendMSG(clientID, dataToSend);
```

### 3. Receiving Messages

To receive messages from a client, the `recvMSG` function is used. This function reads the incoming data in the following manner:

1. **Read the Size**: It first reads an integer that indicates the size of the incoming message.
2. **Allocate Buffer**: It resizes the data vector to accommodate the incoming message.
3. **Read the Data**: It reads the data into the allocated buffer.

Example usage:
```cpp
std::vector<int> receivedData;
recvMSG(clientID, receivedData);
```

### 4. Asynchronous Message Handling

The library supports asynchronous message handling through the `recvMSGAsync` function. This function continuously listens for incoming messages on a separate thread, allowing your application to remain responsive while processing messages.

### 5. Message Integrity

To ensure message integrity, the library provides mechanisms to check for pending messages and handle any errors that may occur during transmission. Functions like `checkPendingMessages` can be used to verify if there are unread messages in the buffer.

## Conclusion

This guide provides a foundational understanding of how to utilize the messaging functionalities in the Networking Utils library. By mastering these concepts, you will be able to implement robust client-server communication in your applications. For more advanced usage and examples, refer to the other documentation files in this project.