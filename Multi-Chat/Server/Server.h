#ifndef _SERVER_
#define _SERVER_
#include <WinSock2.h>
#include <stdio.h>
#include <ws2tcpip.h>
#include "../utils/utils.h"
#define MAX_HILOS 2 
#define PORT "34053"
extern HANDLE hilos[MAX_HILOS];
extern HANDLE eventos[MAX_HILOS];
extern int hilo_activo[MAX_HILOS];

typedef struct {
	SOCKET* sock;
	int indice;
}AUX;
typedef struct {
	int a;
}PORB;

typedef struct LIST_SOCK {
	SOCKET* sock;
	struct LIST_SOCK* next;
}LIST_SOCK;

typedef struct {
	LIST_SOCK** cabeza;
	LIST_SOCK** cola;
}PACK_LIST;

DWORD WINAPI recibir_chat(LPVOID fd_sock);
SOCKET iniciar_servidor(void);
DWORD WINAPI esperar_hilos(LPVOID pack);
int conectar_cliente(SOCKET* sock);
SOCKET* desencolar_list(LIST_SOCK** cabeza, LIST_SOCK** cola);
void encolar_list(LIST_SOCK** cabeza, LIST_SOCK** cola, SOCKET* socke);
#endif // !_SERVER_/