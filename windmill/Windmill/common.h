#ifndef __LETMECREATE_EXAMPLES_COMMON_H__
#define __LETMECREATE_EXAMPLES_COMMON_H__

#include <stdint.h>

void sleep_ms(unsigned int ms);

#define PUERTO_MQTT 1883  /* puerto del broker MQTT */

#define MAXDATASIZE 256  /* maximo numero de bytes que podemos recibir */

#define ID_HEADER_LEN sizeof(uint8_t) * 2

/* formato de la unidad de datos de aplicacion para Datagramas*/
struct idappdata
{
    uint8_t op;                              /* codigo de operacion */
    uint8_t len;                              /* identificador */
    char data[MAXDATASIZE - ID_HEADER_LEN];   /* datos */
};

#endif