#include <mosquitto.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <letmecreate/letmecreate.h>

#include "common.h"

static volatile uint8_t status = 0;

char dataAction[100];
char host[16];
char msg[MAXDATASIZE - ID_HEADER_LEN];
char topicAction[400];
char stage[100];
char user[100];
struct idappdata* operacion;
struct mosquitto* mosq;
uint8_t qos = 0;
uint16_t mid = 0;
uint16_t port = 0;

void switch_1_pressed()
{
    if (status == 0) {
        operacion->op = (uint8_t) (0xd0 + 0x00);
        strcpy(operacion->data, "0");
        operacion->len = strlen(operacion->data);

        sprintf(topicAction, "dgiotwins/user/%s/stage/%s/data/%s", user, stage, dataAction);

        mosquitto_publish(mosq, &mid, topicAction, ID_HEADER_LEN + operacion->len, (char*) operacion, qos, 0);

        printf("Aspas arrancadas.\n");
    }
    status = 1;
}

void switch_2_pressed()
{
    if (status == 1) {
        operacion->op = (uint8_t) (0xd0 + 0x00);
        strcpy(operacion->data, "3");
        operacion->len = strlen(operacion->data);

        sprintf(topicAction, "dgiotwins/user/%s/stage/%s/data/%s", user, stage, dataAction);

        mosquitto_publish(mosq, &mid, topicAction, ID_HEADER_LEN + operacion->len, (char*) operacion, qos, 0);

        printf("Aspas paradas.\n");
    }
    status = 0;
}

int main(int argc, char* argv[])
{
    char dataData[100];
    char topicData[400];
    float power = 0.f;

    if (argc < 7) {
        fprintf (stderr, "uso: windmill host, port, user, stage, dataAction, dataData\n");
        exit (1);
    }

    switch_init();

    switch_add_callback(SWITCH_1_PRESSED, switch_1_pressed);
    switch_add_callback(SWITCH_2_PRESSED, switch_2_pressed);

    mosquitto_lib_init();

    mosq = mosquitto_new(NULL, true, NULL);

    if(!mosq) {
        fprintf(stderr, "Can't init Mosquitto library\n");
        exit(1);
    }

    memset(dataAction, '\0', 100);
    memset(dataData, '\0', 100);
    memset(host, '\0', 16);
    memset(msg, '\0', MAXDATASIZE - ID_HEADER_LEN);
    memset(stage, '\0', 100);
    memset(topicAction, '\0', 400);
    memset(topicData, '\0', 400);
    memset(user, '\0', 100);

    strcpy(host, argv[1]);
    strcpy(user, argv[3]);
    strcpy(stage, argv[4]);
    strcpy(dataAction, argv[5]);
    strcpy(dataData, argv[6]);
    port = (uint16_t) strtol(argv[2], NULL, 10);

    sprintf(topicData, "dgiotwins/user/%s/stage/%s/data/%s", user, stage, dataData);

    int ret = mosquitto_connect(mosq, host, port, 0);

    if(ret) {
        fprintf(stderr, "Can't connect to Mosquitto server\n");
        exit(1);
    }

    operacion = (struct idappdata*) malloc(MAXDATASIZE);

    printf("PROGRAMA INICIADO\n");

    while(1) {
        if (status == 1) {
            srand48(time(NULL));
            power = (13 - 2) * drand48() + 2;
            sprintf(msg, "%.3f", power);

            operacion->op = (uint8_t) (0xd0 + 0x05);
            strcpy(operacion->data, msg);
            operacion->len = strlen(operacion->data);

            mosquitto_publish(mosq, &mid, topicData, ID_HEADER_LEN + operacion->len, (char*) operacion, qos, 0);

            printf("Mensaje enviado: \"%s\"\n", msg);
        }

        sleep_ms(10 * 1000);
    }

    free(operacion);
    switch_release();

    return 0;
}
