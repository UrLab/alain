import os
import logging


MQTT_SERVER = os.environ.get("MQTT_HOST", "192.168.42.20")

INCUBATOR_BASE_URL = "https://urlab.be"
INCUBATOR_SECRET = os.environ.get("INCUBATOR_SECRET", "vairie secrette")
print(INCUBATOR_SECRET)
LOG_LEVEL = logging.DEBUG
