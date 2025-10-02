#include "utils.h"
#include <string>
#include <iostream>
#include "clientManager.h"

using namespace std;

void chat(int serverId, string userName){
	
	bool salir = false;
	string mensajeLeido;
	string mensajeRecivido;
	// Mientrar no salir 
	while (!salir) {
		// Pedir mensaje
		cout << "Introduzca un mensaje para el servidor:" << endl;
		// Leerlo
		getline(cin, mensajeLeido);
		// Si no salir
		salir = (mensajeLeido == "salir");
		if (!salir) {
			//Enviar mensaje
			enviarMensaje(serverId, userName);
			enviarMensaje(serverId, mensajeLeido);
			
			// Recibir mensajes del servidor
			mensajeRecivido = recibeMensaje(serverId);
			// Leer mensajes del servidor 
			cout << mensajeRecivido;
		}
	}
}

int main(int argc,char** argv)
{
	
	string nombreUsario="Pero pringao ponte un nombre";
	
	cout << "Inicio conexion cliente";
	auto serverConnID=initClient("127.0.0.1",1250);
	
	cout << "Conectado, introduzca usuario (sin espacios):" << endl;
	
	cin >> nombreUsario;
	
	chat (serverConnID.serverId, nombreUsario);
	
	//Pseudocodigo. Para enviar:
		// empaquetar datos
			// empaquetar tamaño del string
			// empaquetar datos del string
		// enviar datos
		// Comprobar que se recibió la respuesta
	
	
	//closeConnection(serverConnID.serverId);
	
	
	return 0;
}
