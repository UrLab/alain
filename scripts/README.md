# MQTT Scripts

This project requires a `.env` file that specifies the MQTT server to connect to. It should look like this:

```bash
MQTT_HOST=localhost
INCUBATOR_SECRET=secretkey
```

## Running
Simply type the following command on the raspberry pi:
```bash
docker-compose up -d --build
```
