#include <stdio.h>
#include <xc.h>

#include <contiki.h>
#include <contiki-net.h>

#include "button-sensor.h"

#include "dev/leds.h"
#include "lib/memb.h"
#include "sys/clock.h"

#include "letmecreate/core/common.h"
#include "letmecreate/core/debug.h"
#include "letmecreate/core/network.h"

#include "common.h"

#include <lib/sensors.h>
#include <pic32_gpio.h>

MEMB(appdata, struct idappdata, MAXDATASIZE);

static struct uip_udp_conn* conn;

/*---------------------------------------------------------------------------*/
void leds_toggle1()
{
    static char topic[200];
    static struct idappdata* envio;
    envio = memb_alloc(&appdata);

    envio->op = 0;

    memset(envio->data, '\0', MAXDATASIZE - ID_HEADER_LEN);
    sprintf(topic, "dgiotwins/user/carlosmg95/stage/clicker/data/led1");
    

    leds_toggle(LED1);

    if((leds_get() & LED1) == 0) {
        sprintf(envio->data, "3");
    } else {
        sprintf(envio->data, "0");
    }

    envio->len = strlen(envio->data);
    strcat(envio->data, topic);

    udp_packet_send(conn, (char*) envio, ID_HEADER_LEN + strlen(envio->data));
}
/*---------------------------------------------------------------------------*/
void leds_toggle2()
{
    static char topic[200];
    static struct idappdata* envio;
    envio = memb_alloc(&appdata);

    envio->op = 0;

    memset(envio->data, '\0', MAXDATASIZE - ID_HEADER_LEN);
    sprintf(topic, "dgiotwins/user/carlosmg95/stage/clicker/data/led2");
    

    leds_toggle(LED2);

    if((leds_get() & LED2) == 0) {
        sprintf(envio->data, "3");
    } else {
        sprintf(envio->data, "0");
    }

    envio->len = strlen(envio->data);
    strcat(envio->data, topic);

    udp_packet_send(conn, (char*) envio, ID_HEADER_LEN + strlen(envio->data));
}
/*---------------------------------------------------------------------------*/
void click_btn_der(void)
{
    leds_toggle2();
}
/*---------------------------------------------------------------------------*/
void click_btn_izq(void)
{
    leds_toggle1();
}
/*---------------------------------------------------------------------------*/
PROCESS(send_data_proccess, "Main process");
//PROCESS(Recv_data_proccess, "Second process");
AUTOSTART_PROCESSES(&send_data_proccess);//, &Recv_data_proccess);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(send_data_proccess, ev, data)
{
    PROCESS_BEGIN();
    INIT_NETWORK_DEBUG();
    {
        static char buffer[1];
        static char data_stage[BUFFER_SIZE];
        static char network_data[BUFFER_SIZE];
        static int length;
        static struct idappdata* operacion;
        static struct idappdata* resultado;
        static uint8_t flashes = 0;
        static uint8_t i = 0;

        operacion = memb_alloc(&appdata);
        resultado = memb_alloc(&appdata);

        PRINTF("=====Start=====\n");

        flashes = 2;
        while(flashes--) {
            // Flash every second
            for(i = 0; i < 20; i++)
                clock_delay_usec(50000);

            leds_toggle1();
        }

        // CONFIGURACIÓN SOCKET UDP
        conn = udp_new_connection(3000, 3001, IP6_CI40);
        PROCESS_WAIT_UDP_CONNECTED();
        PROCESS_WAIT_UDP_CONNECTED();
        PROCESS_WAIT_UDP_CONNECTED();

        memset(buffer, '\0', 1);

        udp_packet_send(conn, buffer, strlen(buffer));
        PROCESS_WAIT_UDP_SENT();

        flashes = 4;
        while(flashes--) {
            /* Flash every second */
            for(i = 0; i < 20; i++)
                clock_delay_usec(50000);

            leds_toggle1();
        }

        PRINTF("Starting loop\n");

        // COMIENZO DEL BUCLE
        while(1)
        {
            PROCESS_YIELD();
            if (ev == sensors_event && data == &button_sensor) {
                click_btn_izq();
            }
            if (ev == sensors_event && data == &button_sensor2) {
                click_btn_der();
            }
            if (ev == tcpip_event && uip_newdata()) {
                length = udp_packet_receive(network_data, BUFFER_SIZE, NULL);

                resultado = (struct idappdata*) network_data;

                if (resultado->op == 0xd8) {
                    memset(data_stage, '\0', BUFFER_SIZE);
                    strcpy(data_stage, resultado->data + resultado->len);

                    if (strcmp(data_stage, "click_btn_der") == 0) {
                        click_btn_der();
                    } else if (strcmp(data_stage, "click_btn_izq") == 0) {
                        click_btn_izq();
                    }
                }
            }
        }
        memb_free(&appdata, operacion);
    }
    PROCESS_END();
}
/*---------------------------------------------------------------------------*/