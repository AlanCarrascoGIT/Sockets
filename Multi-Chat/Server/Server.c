#include "Server.h"
HANDLE hilos[MAX_HILOS] = { NULL };
HANDLE eventos[MAX_HILOS];
int hilo_activo[MAX_HILOS] = { 0 };
SOCKET socket_disp[MAX_HILOS] = { 0 };
int pun = 0;
DWORD WINAPI recibir_chat(LPVOID pack) {
	AUX* paquete = (AUX*)pack;
	SOCKET sock = *(paquete->sock);
	int size;
	int indice = paquete->indice;
	free(paquete->sock);
	free(paquete);
	while (1) {
		char* buffer = recibir_buffer(&size, sock);
		if (buffer == NULL) {
			printf("cliente desconectado\n");
			break;
		}
		else {
			printf("%s", buffer);
			free(buffer);
		}
	}
	closesocket(sock);
	SetEvent(eventos[indice]);
	return 0; 
}

SOCKET iniciar_servidor(void)
{
	SOCKET socket_servidor; 
	int err;

	struct addrinfo hints, * servinfo;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(NULL, PORT, &hints, &servinfo);
	if (getaddrinfo(NULL, PORT, &hints, &servinfo) != 0) {
		printf("getaddrinfo failed\n");
		WSACleanup();
		return 1;
	}
	// Creamos el socket de escucha del servidor
	socket_servidor = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
	if (socket_servidor == -1)
	{
		
		close(socket_servidor);
		freeaddrinfo(servinfo);
		abort();
	}
	int reuse = 1;
	if (setsockopt(socket_servidor, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
		printf("SO_REUSEADDR failed: %s \n");
		return 1;
	}
	// Asociamos el socket a un puerto
	err = bind(socket_servidor, servinfo->ai_addr, servinfo->ai_addrlen);
	if (err == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(servinfo);
		closesocket(socket_servidor);
		WSACleanup();
		return 1;
	}

	err=listen(socket_servidor, 2);

	if (err == SOCKET_ERROR) {
		printf("Listen failed with error: %ld\n", WSAGetLastError());
		closesocket(socket_servidor);
		WSACleanup();
		return 1;
	}
	freeaddrinfo(servinfo);

	return socket_servidor;
}

int conectar_cliente(SOCKET* sock) {
	AUX* aux = malloc(sizeof(AUX));
	CHECK* status = malloc(sizeof(CHECK));
	aux->sock = sock;
	for (int i = 0; i < MAX_HILOS; i++) {
		if (hilo_activo[i] == 0) {
			*status = LISTO;
			send(*sock, status, sizeof(CHECK), 0);
			aux->indice = i;
			hilos[i] = CreateThread(NULL, 0, recibir_chat, aux, 0, NULL);
			if (hilos[i] == NULL) {
				printf("Error al crear el hilo para cliente #%d\n", i + 1);
				free(status);
				return 1; // Indicar fallo
			}
			printf("CLIENTE #%d Conectado \n", i + 1);
			hilo_activo[i] = 1;
			free(status);
			return 0;
		}
	}

	printf("Clientes llenos\n");
	return 1;// Indicar fallo
}

DWORD WINAPI esperar_hilos(LPVOID pack)
{
	//PACK_LIST* pk = (PACK_LIST*)pack;
	SOCKET* sock;
	DWORD ver;
	int res;
	while (1) {
		ver = WaitForMultipleObjects(MAX_HILOS, eventos, FALSE, INFINITE);
		res = ver - WAIT_OBJECT_0;
		CloseHandle(hilos[res]);
		hilos[res] = NULL; // Marcar como disponible
		hilo_activo[res] = 0;
		printf("Cliente #%d liberado\n", res + 1);
		if (pun) {
			sock = desencolar_list(((PACK_LIST*)pack)->cabeza, ((PACK_LIST*)pack)->cola);
			ResetEvent(eventos[res]);
			conectar_cliente(sock);
		}
		else {
			ResetEvent(eventos[res]);
		}
	}
	return 0;// Código de retorno del hilo
}


void encolar_list(LIST_SOCK** cabeza, LIST_SOCK** cola, SOCKET* socke) {
	LIST_SOCK* aux = malloc(sizeof(LIST_SOCK));
	CHECK* status = malloc(sizeof(CHECK));
	aux->sock = socke;
	aux->next = NULL;
	if (*cabeza == NULL) {
		*cabeza = aux;
	}
	else {
		(*cola)->next = aux;
	}
	*status = WAITING;
	send(*socke,status,sizeof(CHECK), 0);
	pun++;
	*cola = aux;
	free(status);
	return;
}

SOCKET* desencolar_list(LIST_SOCK** cabeza, LIST_SOCK** cola) {
	SOCKET* sock;
	LIST_SOCK* aux = *cabeza;
	sock = aux->sock;
	*cabeza = aux->next;
	if (*cabeza == NULL) {
		*cola = NULL;
	}
	pun--;
	free(aux);
	return sock;
}