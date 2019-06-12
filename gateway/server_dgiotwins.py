#!/usr/bin/python3

import _thread as thread
import argparse
import os, sys
import re
import socket
import struct
import subprocess
import time

class __Color:
    INFO = '\033[93m'    
    DEFAULT = '\033[0m'

def print_debug(*print_args, **kargs):
    print(__Color.INFO + print_args[0] + __Color.DEFAULT, **kargs)

def main():
    # Lee parámetros
    parser = argparse.ArgumentParser()
    parser.add_argument('-u', '--user', help='User who send', default='carlosmg95', type=str)
    parser.add_argument('-s', '--stage', help='Stage that sent', default='clicker', type=str)
    parser.add_argument('-p', '--port', help='Port to bind to', default=3001, type=int)
    parser.add_argument('-b', '--buffer', help='Buffer size', default=256, type=int)
    args = parser.parse_args()

    # No IP to connect to needed for a server

    IP = '::'
    PORT = args.port

    # Creates a socket using IPV6 and accepting datagrams
    sock = socket.socket(socket.AF_INET6, socket.SOCK_DGRAM)
    sock.bind((IP, PORT))

    print_debug('Waiting for UDP msg. Use Ctrl + C to stop')

    # Lee datos de envio

    data, address = sock.recvfrom(args.buffer)

    print_debug('Received. Use Ctrl + C to stop')

    try:
        thread.start_new_thread(readMQTT, (args, sock, address))
        thread.start_new_thread(readUDP, (args, sock))
    except:
        print('Error: unable to start thread')

    while 1:
        pass

def readMQTT(args, sock, address):
    # Ejecutar comando mosquitto

    comando = 'mosquitto_sub -h localhost -t dgiotwins/user/+/stage/+/data/+ -v'
    resultado = subprocess.Popen(comando, 
                                   shell=True, 
                                   stdout=subprocess.PIPE)

    print_debug('Server initialised, reading MQTT messages. Use Ctrl + C to stop')
    for salida11 in resultado.stdout:
        # Lee topic y mensaje
        topic = salida11[:salida11.index(32)].decode(sys.getdefaultencoding()).rstrip().split(' ')[0]
        msg = salida11[salida11.index(32) + 1:-1]

        # Lee user, stage y data

        regex = re.compile(r'dgiotwins/user/(.+)/stage/(.+)/data/(.+)')
        matcher = regex.search(topic)

        user = matcher.group(1)
        stage = matcher.group(2)
        data = matcher.group(3)

        msg += bytes(data, 'utf-8')
        op = msg[0] & 0b00001111

        if (args.user == user and args.stage == stage and op == 0x08):
            print('Recibido: Topic: {}\nEnviando mensaje: {}'.format(topic, msg))
            sock.sendto(msg, address)

def readUDP(args, sock):
    print_debug('Server initialised, awaiting data. Use Ctrl + C to stop')
    while True:
        data, address = sock.recvfrom(args.buffer)
        if (data.decode('utf-8') == 'dummy'):  # Si el mensaje es de configuración se lo salta
            continue

        # Lee los datos enviados por el 6LoWPAN Clicker
        op = data[0] & 0b00001111
        len_data = data[1]
        data_msg = data[2:len_data + 2].decode('utf-8')
        topic = data[len_data + 2:].decode('utf-8')

        regex = re.compile(r'dgiotwins/user/(.+)/stage/(.+)/data/(.+)')
        matcher = regex.search(topic)

        user = matcher.group(1)
        stage = matcher.group(2)
        data = matcher.group(3)

        if (data):
            send_msg_mqtt('localhost', 1883, user, stage, data, op, data_msg)

def send_msg_mqtt(host, port, user, stage, data, op, message):
    comando = 'publish_mqtt {} {} {} {} {}'.format(host, port, user, stage, data, op, message)
    print('Ejecuntando comando {}'.format(comando))
    subprocess.call(comando, shell=True)

if (__name__ == '__main__'):
    main()
