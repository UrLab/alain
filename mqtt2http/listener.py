import paho.mqtt.client as mqtt
import requests

import settings


# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, rc):
    print("Connected with result code " + str(rc))

    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    client.subscribe("action/+/state_change")


# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
    print(msg.topic + " " + str(msg.payload))

    if msg.topic == "action/OpenLever/state_change":
        assert msg.payload in (b"0", b"1")
        is_up = msg.payload == b"1"
        requests.post(
            settings.INCUBATOR_BASE_URL + '/space/change_status',
            data={
                'open': 1 if is_up else 0,
                'secret': settings.INCUBATOR_SECRET,
            }
        )


client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

client.connect(settings.MQTT_SERVER, 1883, 60)

# Blocking call that processes network traffic, dispatches callbacks and
# handles reconnecting.
# Other loop*() functions are available that give a threaded interface and a
# manual interface.
client.loop_forever()
