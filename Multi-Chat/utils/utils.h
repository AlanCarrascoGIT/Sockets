#ifndef _UTILS_
#define _UTILS_
#include <WinSock2.h>
#include <ws2tcpip.h>

typedef struct
{
	int size;
	void* stream;
} t_buffer;

typedef enum {
	LISTO,
	WAITING
}CHECK;



void* recibir_buffer(int* size, SOCKET socket_cliente);
void recibir_mensaje(SOCKET socket_servidor);
void enviar_mensaje(char* mensaje, SOCKET socket_cliente);
void* serializar_mensaje(t_buffer* buff, int bytes);
void eliminar_mensaje(t_buffer* buff);

#endif // 