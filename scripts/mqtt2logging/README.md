# Goal
Allowing for logging for the ESP8266.

# How we do dis?
Each device will send any logging messages it wants onto the `debug/{hostname}` topic. Those messages will then be captured by this script and written to a separate logfile per device.

### The end
