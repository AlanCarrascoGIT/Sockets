#ifndef _CLIENT_
#define _CLIENT_
#include <WinSock2.h>
#include <ws2tcpip.h>
#include "../utils/utils.h"
#include <stdio.h>
#define PORT "34053"
#define IP "localhost"
extern int hilo;


void crear_conexion(char* p,char* i);


#endif // 
