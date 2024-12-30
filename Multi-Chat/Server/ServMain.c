#include "Server.h"
#pragma comment(lib, "Ws2_32.lib")
#define BUFFER_SIZE 1024

void limpiarBuffer();

int main()
{
    LIST_SOCK* cabeza = NULL;
    LIST_SOCK* cola = NULL;
    PACK_LIST* pack=malloc(sizeof(PACK_LIST));
    pack->cabeza = &cabeza;
    pack->cola = &cola;
    HANDLE manejar_espera;
    int err;
    int l;
    WSADATA wsaData;
    SOCKET sock;

    int len = 0;

    // Inicializa Winsock
    int result;
    result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        printf("Error al inicializar Winsock: %d\n", result);
        return 1;
    }

    sock = iniciar_servidor();
    for (int i = 0; i < MAX_HILOS; i++) {
        eventos[i] = CreateEvent(NULL, TRUE, FALSE, NULL);  // Evento manual-reset
        if (eventos[i] == NULL) {
            printf("Error al crear el evento para el hilo %d\n", i);
            return 1;
        }
        hilo_activo[i] = 0;
    }
    manejar_espera = CreateThread(NULL, 0, esperar_hilos, pack, 0, NULL);
    while (1) {
         SOCKET* nuevo_socket = malloc(sizeof(SOCKET));
        *nuevo_socket = accept(sock, NULL, NULL);
        if (*nuevo_socket == INVALID_SOCKET) {
            printf("accept failed: %d\n", WSAGetLastError());
            closesocket(sock);
            free(nuevo_socket);
            break;
        }

        l = conectar_cliente(nuevo_socket);
        if (l) {
            encolar_list(&cabeza, &cola, nuevo_socket);
        }
    }

    for (int i = 0; i < MAX_HILOS; i++) {
        CloseHandle(eventos[i]);
    }

    closesocket(sock);
    WSACleanup();
    return 0;
}

void limpiarBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
        // Consume caracteres
    }
}