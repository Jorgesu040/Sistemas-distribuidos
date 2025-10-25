// Modificaciones necesarias en server.cpp

#include "utils.h"
#include <iostream>
#include <string>
#include <thread>
#include <list>
#include "clientManager.h"
#include <signal.h>

static volatile sig_atomic_t keepRunning = 1;

void muerteOrdenada(int s) {
	keepRunning = 0;
}

int main(int argc, char** argv)
{
	signal(SIGINT, muerteOrdenada);

	std::cout << "Hola mundo desde server" << std::endl;

	int serverID = initServer(1250); // localhost

	while(keepRunning == 1) {
		// Esperar conexion
		while (keepRunning == 1 && !checkClient()) usleep(100);

		if (keepRunning) { 
			std::cout << "Cliente conectado || ";
			int clientID = getLastClientID();
			std::cout << "Identificador de cliente: " << clientID << std::endl;

			auto* newThread = new std::thread(clientManager::atiendeCliente, clientID);
			clientManager::cerrojoThreads.lock();
			clientManager::clientThreads[clientID] = newThread; // Guardar ID con thread
			clientManager::cerrojoThreads.unlock();
		} else {
			break;
		}
	}

	std::cout << "Iniciando apagado ordenado del servidor..." << std::endl;

	
	clientManager::cierreDePrograma = true;
	
	// Notificar a todos los clientes conectados
	clientManager::cerrojoThreads.lock();
	for (auto& [clientID, thread] : clientManager::clientThreads) {
		clientManager::enviarApagado(clientID);
	}
	clientManager::cerrojoThreads.unlock();


	// Esperar a que todos los threads terminen
	clientManager::cerrojoThreads.lock();
	for (auto& [clientID, thread] : clientManager::clientThreads) {
		if (thread->joinable()){
			thread->join();
			delete thread;
		}
	}

	clientManager::clientThreads.clear();
	clientManager::cerrojoThreads.unlock();

	close(serverID);
	std::cout << "Servidor cerrado correctamente" << std::endl;
	return 0;
}