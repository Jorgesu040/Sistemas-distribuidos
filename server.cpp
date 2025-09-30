#include "utils.h"
#include <iostream>
#include <string>
#include <thread>
#include <list>

// Como queremos poder atender a los clientes en paralelo y no de manera sequencial, creamos una funcion que este en varios threads
void atiendeCliente(int clientID){
	
	std::vector<unsigned char> bufferIn;
	// Recibir datos
	recvMSG(clientID, bufferIn);
	//Desempaquetar
	std::string mensaje;
	
	mensaje.resize(unpack<long int>(bufferIn));
	
	unpackv<char>(bufferIn, (char *)mensaje.data(), mensaje.size());
	
	// Enviar Acknowledge
	int ack = 1;
	// Limpiar buffer
	bufferIn.clear();
	pack(bufferIn, ack);
	sendMSG(clientID, bufferIn);
	
	std::cout << "Mensaje recibido: " << mensaje << std::endl;
	
	closeConnection(clientID);
	/* Otra manera de hacer el proceso
		// Recibir datos y crear buffer
		std::vector<unsigned char> bufferIn;
		recvMSG(clientID, bufferIn);
		
		// Desempaquetar
		long int msgTam = unpack<long int>(bufferIn);
		std::string msg(msgTam, '\0');
		
		// Dar fromato
		// (char*)msg.data() -> desempaqueta los datos (string es un char*, es decir empieza por ahi)
		unpackv<char>(bufferIn, (char*)msg.data(), msgTam);
				
		// Enviar Acknowledge
		int ack = 1;
		// Limpiar buffer
		bufferIn.clear();
		pack(bufferIn, ack);
		sendMSG(clientID, bufferIn);
		
		std::cout << "Mensaje recibido: " << msg << std::endl;

		// Cerrar conexion
		closeConnection(clientID);
	*/
}

int main(int argc, char** argv)
{
	std::cout << "Hola mundo desde server" << std::endl;
	
	int serverID = initServer(1250); // localhost
	
	// Por cada cliente
		// Esperar conexion
		// Recibir datos
		// Cerrar conexion 
			// Opcional cerrar con una señal con un SIGKILL por ejemplo

	bool salir = false;
	
	while(!salir){
		// Esperar conexion
		while (!checkClient()) usleep(100);
		std::cout << "Cliente conectado || ";
		int clientID = getLastClientID();
		std::cout << "Identificador de cliente: " << clientID << std::endl;
		
		std::thread *hilo = new std::thread(atiendeCliente,clientID);
		// Version en la que esperamos a que termine el cliente 
		// atiendeCliente(clientID)
		
	}
	
	close(serverID);
	return 0;
}
