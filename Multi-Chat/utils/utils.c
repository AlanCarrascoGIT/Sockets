#include "./utils.h"
void* recibir_buffer(int* size, SOCKET socket_cliente)
{
	int se;
	void* buffer;

	se=recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	if (se == 0||se<0) {
		buffer = NULL;
		return buffer;
	}
	else {
		buffer = malloc(*size);
		recv(socket_cliente, buffer, *size, MSG_WAITALL);
	}
	return buffer;
}
void eliminar_mensaje(t_buffer* buff)
{
	free(buff->stream);
	free(buff);
}
void* serializar_mensaje(t_buffer* buff, int bytes)
{
	void* magic = malloc(bytes);
	int offset = 0;
	memcpy((int*)magic + offset, &(buff->size), sizeof(int));
	offset += sizeof(int);
	memcpy((char*)magic + offset, buff->stream, buff->size);
	offset += buff->size;

	return magic;
}
void enviar_mensaje(char* mensaje, SOCKET socket_cliente)
{
	t_buffer* buff = malloc(sizeof(t_buffer));

	buff->size = strlen(mensaje) + 1;
	buff->stream = malloc(buff->size);
	memcpy(buff->stream, mensaje, buff->size);

	int bytes = buff->size + sizeof(int);

	void* a_enviar = serializar_mensaje(buff, bytes);

	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
	eliminar_mensaje(buff);
}

void recibir_mensaje(SOCKET socket_cliente)
{
	int size;
	char* buffer = recibir_buffer(&size, socket_cliente);
	printf("%s", buffer);
	free(buffer);
}
/*
int enviar_chat(SOCKET fd_sock) {
	char mensaje[BUFFER_SIZE];
	while (1) {
		t_buffer* buff = malloc(sizeof(t_buffer));
		fgets(mensaje, BUFFER_SIZE, stdin);
		buff->size = strlen(mensaje) + 1;
		buff->stream = malloc(buff->size);
		memcpy(buff->stream, mensaje, buff->size);

		int bytes = buff->size + sizeof(int);

		void* a_enviar = serializar_mensaje(buff, bytes);
		int b;
		b = send(*(SOCKET*)fd_sock, a_enviar, bytes, 0);
		if (b <= 0) {
			perror("Error en send, cliente desconectado.");
			closesocket(*(SOCKET*)fd_sock);
			break; // Salir del bucle
		}

		free(a_enviar);
		eliminar_mensaje(buff);
	}
	return 0;
}

int recibir_chat(SOCKET fd_sock) {
	int size;
	SOCKET sock = *(SOCKET*)fd_sock;
	while (!hilo) {
		char* buffer = recibir_buffer(&size, sock);
		if (buffer == NULL) {
			hilo = 1;
			printf("server desconectado\n");
		}
		else {
			printf("%s", buffer);
			free(buffer);
		}
	}
	return 0;
}
*/