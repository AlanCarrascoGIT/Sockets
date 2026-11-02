# Multi-Chat con Sockets en C

Servidor y cliente de chat por **TCP** escritos en C con **Winsock** y **hilos de Win32**. El servidor atiende a varios clientes en simultáneo (un hilo por cliente) y, cuando se llena, deja a los clientes nuevos en una **cola de espera** y los conecta automáticamente cuando se libera un lugar.

## Funcionalidades

- **Servidor concurrente:** cada cliente conectado se atiende en su propio hilo (`CreateThread`).
- **Límite de conexiones activas:** configurable con `MAX_HILOS` (por defecto, 2 clientes a la vez).
- **Cola de espera:** los clientes que llegan con el servidor lleno se guardan en una cola (lista enlazada). Cuando un cliente se desconecta, el primero de la cola pasa a ser atendido.
- **Sincronización entre hilos:** cada hilo avisa que terminó con un evento de Windows (`SetEvent`), y un hilo supervisor lo detecta con `WaitForMultipleObjects` para liberar el lugar.
- **Protocolo propio de mensajes:** cada mensaje se serializa como `[tamaño (int)][contenido]`, así el receptor sabe exactamente cuántos bytes leer.
- **Detección de desconexiones** del lado del servidor.

## Cómo funciona

```
Cliente 1 ──┐                      ┌── Hilo 1 ──┐
Cliente 2 ──┼──► accept() ─────────┼── Hilo 2 ──┼──► consola del servidor
Cliente 3 ──┘        │             └────────────┘
                     └──► cola de espera ──(se libera un lugar)──► Hilo libre
```

Al conectarse, el servidor le envía al cliente un estado:

| Estado    | Significado                                                        |
|-----------|--------------------------------------------------------------------|
| `LISTO`   | Hay un lugar libre: el cliente ya puede enviar mensajes.            |
| `WAITING` | El servidor está lleno: el cliente espera hasta recibir `LISTO`.   |

Los mensajes que escribe cada cliente se muestran en la consola del servidor.

## Estructura

```
Multi-Chat/
├── Server/
│   ├── ServMain.c    # Inicializa Winsock, acepta conexiones y encola clientes
│   ├── Server.c      # Hilos por cliente, cola de espera, hilo supervisor
│   └── Server.h      # Configuración (puerto, MAX_HILOS) y estructuras
├── Cliente/
│   ├── ClientMain.c  # Punto de entrada del cliente
│   ├── Client.c      # Conexión al servidor y envío de mensajes
│   └── Client.h      # IP y puerto del servidor
└── utils/
    ├── utils.c       # Serialización, envío y recepción de mensajes
    └── utils.h
```

### Configuración

| Parámetro   | Archivo            | Valor por defecto |
|-------------|--------------------|-------------------|
| Puerto      | `Server/Server.h` y `Cliente/Client.h` | `34053` |
| IP servidor | `Cliente/Client.h` | `localhost`       |
| Máx. clientes simultáneos | `Server/Server.h` | `2` |

## Conceptos aplicados

Sockets TCP (`socket`, `bind`, `listen`, `accept`, `connect`, `send`, `recv`) · programación concurrente con hilos · sincronización con eventos · estructuras de datos dinámicas (cola con lista enlazada) · serialización de mensajes · manejo manual de memoria.

## Próximas mejoras

- Reenviar cada mensaje a todos los clientes conectados (broadcast) para que funcione como un chat grupal.
- Recibir mensajes del servidor en el cliente con un hilo dedicado.
- Proteger las estructuras compartidas con un mutex o sección crítica.
- Versión multiplataforma con sockets POSIX (Linux).
