# HAL1
HAL1 est le nouveau bot d'UrLab pour remplacer feu Hal.

Page contenant plus d'info sur notre wiki: https://urlab.be/wiki/hal1/

Il est basé sur un systeme distribué de senseurs (température, levier, ...) et animations (leds, buzzers, relays, ...) et utilise `MQTT` comme systeme de communication.
Le but étant de rajoutter plein de wemos mini (`ESP8266`) pour chaque feature que l'on souhaite implémenter

# Ajouter un nouvel ESP?

Nous utilisons PlatformIO https://platformio.org/ pour compiler et pusher le code sur les wemos.
Il est diponible en teminal ou sur atom (https://docs.platformio.org/en/latest/ide/atom.html).

Chaque projet PlatformIO pour chaque ESP se trouve dans son dossier respectif dans ce repo. (MarieGuiGui, Opinator...)

Nous avons un peu de documentation sur notre wiki ainsi que des templates pour
permettre une connexion au wifi ainsi que permettre un upgrade via le wifi pour ne pas devoir se connecter physiquement aux ESP à chaque fois. (ATTENTION : évitez de laisser des mots de passe en clair dans votre code. Le fichier qui sera utilisé en prod pour compiller les firmwares sur les ESP est wifiConfig.h dans le repo).
