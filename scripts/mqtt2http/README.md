# MQTT2HTTP
This component listens to MQTT events and pushes them to HTTP endpoints

| Channel | Web Endpoint | expected values | Action |
|---|---|---|---|
| `opinator` | `/space/change_status` | `0` or `1` | Changes the space status on the website |
