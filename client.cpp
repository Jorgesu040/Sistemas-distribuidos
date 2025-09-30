#include "utils.h"
#include <string>
#include <iostream>

using namespace std;

void enviarMensaje(int id, string msg){
	
	vector<unsigned char> buffer; // Para empaquetar datos
	
	// Empaquetamos el tamaño porque, si por ejemplo enviamos dos mensajes:
	// ¿Como sabemos donde empieza/acaba cada mensaje? Con una cabecera que creamos con pack()
	// Unicamente podriamos saber el tamaño completo del paquete si no mandamos cabecera	
	pack(buffer, msg.size());
	
	packv(buffer, msg.data(), msg.size());
	
	sendMSG(id, buffer);
	
	// Limpiar buffer
	buffer.clear();
	
	recvMSG(id, buffer);
	
	int ack = unpack<int>(buffer);
	if (ack != 1){
		cout<<"Error enviando mensaje";
	} else {
		cout << "Recibido correctamente ACK: " << ack << endl;
	}

}

string recibeMensaje (int id){
	
	string mensaje;
	vector<unsigned char> buffer; // Para crear un paquete de datos
	recvMSG(id, buffer);

	mensaje.resize(unpack<long int>(buffer));

	unpackv(buffer, (char*)mensaje.data(), mensaje.size());
	
	// Send ACK
	int ack = 1;
	// Limpiar buffer
	buffer.clear();
	pack(buffer, ack);
	sendMSG(id, buffer);
	
	return 	mensaje;
	
}


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
