#include <WinSock2.h>
#include <ws2tcpip.h>
#include "Client.h"
#pragma comment(lib, "Ws2_32.lib")
#define BUFFER_SIZE 1024
DWORD WINAPI recibir_chat(LPVOID fd_sock) {
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

void limpiarBuffer();
int main()
{
    WSADATA wsaData;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("Error al inicializar Winsock: %d\n");
        return 1;
    }

    crear_conexion(PORT,IP);
    
    WSACleanup();
    return 0;
}

void limpiarBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
        // Consume caracteres
    }
}