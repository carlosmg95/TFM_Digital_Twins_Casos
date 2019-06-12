#include <mosquitto.h>
#include <stdio.h>
#include <stdlib.h>

#include "common.h"

struct idappdata* operacion;
struct mosquitto* mosq;

int main(int argc, char* argv[])
{

    if (argc < 7)
    {
        fprintf (stderr, "uso: publish_mqtt host, port, user, stage, data, op, message\n");
        exit (1);
    }

    mosquitto_lib_init();

    mosq = mosquitto_new(NULL, 1, NULL);

    if(!mosq) {
        fprintf(stderr, "Can't init Mosquitto library\n");
        exit(1);
    }

    char data[50];
    char host[16];
    char msg[MAXDATASIZE - ID_HEADER_LEN];
    char stage[50];
    char topic[200];
    char user[50];
    uint8_t qos = 0;
    uint16_t mid = 0;
    uint16_t port = 0;

    memset(data, '\0', 50);
    memset(host, '\0', 16);
    memset(msg, '\0', MAXDATASIZE - ID_HEADER_LEN);
    memset(stage, '\0', 200);
    memset(topic, '\0', 50);
    memset(user, '\0', 50);

    strcpy(host, argv[1]);
    strcpy(user, argv[3]);
    strcpy(stage, argv[4]);
    strcpy(data, argv[5]);
    strcpy(msg, argv[7]);
    port = (uint16_t) strtol(argv[2], NULL, 10);

    sprintf(topic, "dgiotwins/user/%s/stage/%s/data/%s", user, stage, data);

    operacion = (struct idappdata*) malloc(MAXDATASIZE);    

    memset(operacion->data, '\0', MAXDATASIZE - ID_HEADER_LEN);

    operacion->op = (uint8_t) (0xD0 + strtol(argv[6], NULL, 10));
    strcpy(operacion->data, msg);
    operacion->len = strlen(operacion->data);

    int ret = mosquitto_connect(mosq, host, port, 0);

    if(ret) {
        fprintf(stderr, "Can't connect to Mosquitto server\n");
        exit(1);
    }

    mosquitto_publish(mosq, &mid, topic, ID_HEADER_LEN + operacion->len, (char*) operacion, qos, 0);

    free(operacion);

    return 0;
}
