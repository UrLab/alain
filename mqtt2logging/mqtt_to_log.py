import paho.mqtt.client as mqtt

import settings


def on_connect(client, userdata, flags, rc):
    print("Connected with result code " + str(rc))
    client.subscribe("debug/+")


def on_message(client, userdata, msg):
    hostname = msg.topic.split("/")[1]

    filename = "{}/{}.log".format(settings.LOG_FOLDER, hostname)
    text = msg.payload.decode("utf-8") + "\n"
    with open(filename, "a") as log_file:
        log_file.write(text)


client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

client.connect(settings.MQTT_SERVER, 1883, 60)
client.loop_forever()
