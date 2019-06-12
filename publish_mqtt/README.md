# Publish_MQTT

Programa que sirve para publicar un mensaje MQTT para poder ser leído por la plataforma DgIoTwins

## Compilar

Antes de compilar en una máquina Ubuntu hay que ejecutar los siguientes comando:

```
sudo apt-add-repository ppa:mosquitto-dev/mosquitto-ppa

sudo apt update

sudo apt install mosquitto-dev

sudo apt install libmosquitto-dev

sudo apt install mosquitto-clients
```

Una vez instalado mosquitto ejecutar la orden `make` desde la carpeta ./Publish_mqtt.