# Basic Networking Concepts

Understanding the fundamental concepts of networking is crucial for effectively using the networking library. This document introduces key terms and ideas that form the foundation of network communication.

## 1. IP Addresses

An **IP address** (Internet Protocol address) is a unique identifier assigned to each device connected to a network. It serves two main functions: identifying the host or network interface and providing the location of the device in the network. IP addresses can be classified into two types:

- **IPv4**: This is the most commonly used version, represented as four decimal numbers separated by dots (e.g., 192.168.1.1). Each number can range from 0 to 255.
- **IPv6**: This newer version was developed to address the limitations of IPv4. It uses a longer format, represented as eight groups of four hexadecimal digits (e.g., 2001:0db8:85a3:0000:0000:8a2e:0370:7334).

## 2. Ports

A **port** is a numerical identifier in the range of 0 to 65535 that is used to distinguish different services or applications running on a device. When a device communicates over a network, it uses a combination of an IP address and a port number to establish a connection. Common port numbers include:

- **HTTP**: Port 80
- **HTTPS**: Port 443
- **FTP**: Port 21

## 3. Protocols

**Protocols** are sets of rules that define how data is transmitted over a network. The two most common protocols used in networking are:

- **TCP (Transmission Control Protocol)**: This is a connection-oriented protocol that ensures reliable data transmission. It establishes a connection between the sender and receiver before data is sent, and it guarantees that data packets are delivered in order and without errors.
  
- **UDP (User Datagram Protocol)**: This is a connectionless protocol that allows for faster data transmission but does not guarantee delivery, order, or error checking. It is often used in applications where speed is more critical than reliability, such as video streaming or online gaming.

## 4. Sockets

A **socket** is an endpoint for sending or receiving data across a network. It is a combination of an IP address and a port number. Sockets can be used in both client-server architectures, where a client connects to a server to exchange data. There are two main types of sockets:

- **Stream Sockets**: These use TCP for reliable communication. They are suitable for applications that require guaranteed delivery of data.
  
- **Datagram Sockets**: These use UDP for faster, connectionless communication. They are suitable for applications that can tolerate some data loss.

## Conclusion

Understanding these basic networking concepts is essential for effectively utilizing the networking library. By grasping how IP addresses, ports, protocols, and sockets work, you will be better equipped to implement and troubleshoot network communication in your applications.