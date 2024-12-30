#include "Client.h"
#define BUFFER_SIZE 1024
int hilo = 0;
int enviar_chat(SOCKET fd_sock) {
    char mensaje[BUFFER_SIZE];
    SOCKET sock = fd_sock;

    while (!hilo) {
        int b;
        t_buffer* buff = malloc(sizeof(t_buffer));
        fgets(mensaje, BUFFER_SIZE, stdin);
        buff->size = strlen(mensaje) + 1;
        buff->stream = malloc(buff->size);
        memcpy(buff->stream, mensaje, buff->size);

        int bytes = buff->size + sizeof(int);

        void* a_enviar = serializar_mensaje(buff, bytes);

        b = send(sock, a_enviar, bytes, 0);

        if (b < 0) {
            perror("Error en send");
        }

        free(a_enviar);
        eliminar_mensaje(buff);
    }
    return 0; 
}

void crear_conexion(char* puerto,char* ip)
{

    SOCKET sock = INVALID_SOCKET;
    struct addrinfo* server_info = NULL, hints;
    int err;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_socktype = SOCK_STREAM;

    err = getaddrinfo(ip, puerto, &hints, &server_info);

    if (err != 0) {
        printf("getaddrinfo failed: %d\n", err);
        WSACleanup();
        return;
    }

    sock = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);

    if (sock == INVALID_SOCKET) {
        printf("Error at socket(): %ld\n", WSAGetLastError());
        closesocket(sock);
        WSACleanup();
        return;
    }


    err = connect(sock, server_info->ai_addr, server_info->ai_addrlen);
    if (err == SOCKET_ERROR) {
        closesocket(sock);
        WSACleanup();
        return;
    }
    freeaddrinfo(server_info);

    CHECK status;
    recv(sock, &status, sizeof(CHECK), MSG_WAITALL);
    if (status == LISTO) {
        printf("SERVER Conectado\n");
        enviar_chat(sock);
    }
    else if (status == WAITING) {
        printf("Esperando a que se libere un lugar\n");
        recv(sock, &status, sizeof(CHECK), MSG_WAITALL);
        if (status == LISTO) {
            printf("Server Conectado\n");
            enviar_chat(sock);
        }
    }
    closesocket(sock);
    return;
}
