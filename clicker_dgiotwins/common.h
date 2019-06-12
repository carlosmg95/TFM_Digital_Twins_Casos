#include <stdint.h>

#define IP6_CI40 "fe80::19:f5ff:fe89:1e32"
#define SERVER_IP_ADDR "fe80::19:f5ff:fe89:2f43"

#define PUERTO_MQTT 1883  /* puerto del broker MQTT */

#define MAXDATASIZE 256  /* maximo numero de bytes que podemos recibir */
#define BUFFER_SIZE 4096

#define ID_HEADER_LEN sizeof(uint8_t) * 2

/* formato de la unidad de datos de aplicacion para Datagramas*/
struct idappdata
{
    uint8_t op;                               /* codigo de operacion */
    uint8_t len;                              /* identificador */
    char data[MAXDATASIZE - ID_HEADER_LEN];   /* datos */
};