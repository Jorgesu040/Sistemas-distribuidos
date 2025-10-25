#include "utils.h"
#include <string>
#include <iostream>
#include "clientManager.h"

using namespace std;

void leerMensajeTextoExternos(int id)
{
	string txt;
	string user;
	// Mientras no cerrar programa
	while (!clientManager::cierreDePrograma)
	{

		while (clientManager::bufferTxt.size() == 0)
			usleep(100);

		clientManager::cerrojoBuffers.lock();
		user = clientManager::desempaquetaTipoTexto(clientManager::bufferTxt);
		txt = clientManager::desempaquetaTipoTexto(clientManager::bufferTxt);
		clientManager::cerrojoBuffers.unlock();
		cout << '[' + user + "]: " << txt << '\n';
	}

	cout << "Cierre de hilo recepcion de mensajes de clientes\n";
}

void recibePaquetesAsync(int id)
{
	vector<unsigned char> buffer;
	// Mientras no cerrar programa
	while (!clientManager::cierreDePrograma)
	{
		// Recibe datos
		if (!clientManager::cierreDePrograma) {
			recvMSG(id, buffer);
		} else {
			break;
		}

		auto tipo = unpack<clientManager::msgTypes>(buffer);

		clientManager::cerrojoBuffers.lock();

		switch (tipo)
		{
		case clientManager::ack:
		{
			pack(clientManager::bufferAcks, clientManager::ack);
		}
		break;
		case clientManager::texto:
		{
			string user = clientManager::desempaquetaTipoTexto(buffer);
			string txt = clientManager::desempaquetaTipoTexto(buffer);
			pack(clientManager::bufferTxt, user.size());
			packv(clientManager::bufferTxt, user.data(), user.size());
			pack(clientManager::bufferTxt, txt.size());
			packv(clientManager::bufferTxt, txt.data(), txt.size());
		}
		break;
		case clientManager::shutdown:
		{
			clientManager::cierreDePrograma = true;
            vector<unsigned char> ack_buffer;
            pack(ack_buffer, clientManager::ack);
            sendMSG(id, ack_buffer);
			cout << "Cierre del servidor. Pulse ENTER para salir" << endl;
		} break;
		default:
		{
			ERRORLOG("Mensaje no reconocido. Tipo recibido: " + to_string(tipo));
		}
		break;
		}
		clientManager::cerrojoBuffers.unlock();
	}
}

void chat(int serverId, string userName)
{

	bool salir = false;
	string mensajeLeido;
	string mensajeRecibido;
	// Mientras no salir
	cout << "Cliente conectado, introduzca usuario \n";
	cin >> userName;
	cin.ignore(); // Limpiar buffer de entrada
	
	// enviar login
	clientManager::enviaLogin(serverId, userName);


	// Mientras no salir o servidor no cierre programa
	while (!salir && !clientManager::cierreDePrograma)
	{
		// Pedir mensaje
		cout << "Introduzca un mensaje para el servidor:" << endl;
		// Leerlo
		getline(cin, mensajeLeido);

		if (clientManager::cierreDePrograma) break;

		// Si no salir
		salir = (mensajeLeido == "salir");
		if (!salir && !clientManager::cierreDePrograma)
		{	
			if (mensajeLeido == "/privado")
			{	
				string recipient;
				cout << "Introduzca el nombre cliente al que enviar el mensaje:" << endl;
				
				getline(cin, recipient);

				cout << "Introduzca el mensaje privado:" << endl;
				
				getline(cin, mensajeLeido);

				clientManager::enviarMensajePrivado(serverId, mensajeLeido, recipient);

			} else {
				// Enviar mensaje
				clientManager::enviarMensaje(serverId, mensajeLeido);
			}
		} 
	}

	if (salir) {
		// Enviar mensaje de salida
		vector<unsigned char> buffer;
		pack(buffer, clientManager::exit);
		sendMSG(serverId, buffer);
		
	} else if (clientManager::cierreDePrograma) {
		vector<unsigned char> buffer;
		pack(buffer, clientManager::ack);
		sendMSG(serverId, buffer);	
	}
}

int main(int argc, char **argv)
{

	string nombreUsario;

	cout << "Inicio conexion cliente ";
	auto serverConnID = initClient("127.0.0.1", 1250);

	auto *th = new thread(leerMensajeTextoExternos, serverConnID.serverId);
	auto *th2 = new thread(recibePaquetesAsync, serverConnID.serverId);

	chat(serverConnID.serverId, nombreUsario);

	// Señalar cierre de programa si se sale por chat (usuario escribe "salir")
	clientManager::cierreDePrograma = true;
	closeConnection(serverConnID.serverId);

	if (th->joinable()) th->join();
	delete th;
	if (th2->joinable()) th2->join();
	delete th2;

	return 0;
}
