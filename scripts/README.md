# MQTT Scripts

## Broker
The MQTT broker is a simple mosquitto service running in the backend. The config is modified only to allow external connections to the broker. Its ip is `192.168.42.20`.

## Channels
Channels specifications are described on the [wiki](https://urlab.be/wiki/49)

## Running the project
This project requires a `.env` file that specifies the MQTT server to connect to. It should look like this:

```bash
MQTT_HOST=localhost
INCUBATOR_SECRET=secretkey
```

You can then simply type the following command on the raspberry pi:
```bash
docker-compose up -d --build
```
