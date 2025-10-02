#include "utils.h"
#include <iostream>
#include <string>
#include <thread>
#include <list>


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
