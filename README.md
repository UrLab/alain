# HAL1 :
HAL1 est le nouveau bot d'UrLab pour remplacer feu Hal.
Il est basé sur MQTT. Le but étant de rajoutter plein d'ESP32 ou ESP8266 pour chaque
feature que l'on souhaite implémenter

# Ajouter un nouvel ESP?

Nous utilisons PlatformIO avec atom.
Nous avons un peu de documentation sur notre wiki ainsi que des templates pour
permettre une connexion au wifi ainsi que permettre un upgrade via le wifi pour ne pas devoir se connecter physiquement aux ESP à chaque fois. (ATTENTION : évitez de laisser des mots de passe en clair dans votre code. Le fichier qui sera utilisé en prod pour compiller les firmwares sur les ESP est wifiConfig.h dans le repo)

Toutes les infos sur : https://urlab.be/wiki/hal1/
