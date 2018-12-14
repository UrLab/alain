import logging

MQTT_SERVER = "localhost"

INCUBATOR_BASE_URL = "https://urlab.be"
INCUBATOR_SECRET = "vairie secrette"
LOG_LEVEL = logging.DEBUG

try:
    from local_settings import * # NOQA
except ImportError:
    pass
