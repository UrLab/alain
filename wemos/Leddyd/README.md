# Leddyd (prononcer Lady Di)

## Résumé :
On a plein de leds au hs, ce serait dommage qu'elles restent toutes éteintes :/

## Concretement :
On a créé un site web en local depuis lequel on va pouvoir contrôler toutes les leds du hs !
Pour ca, on a aussi créé des petits PCBs qui, reliés à Hal1, vont pouvoir gérer ces leds en fonction des informations qu'on leur envoie.

## Hardware :
-Des wemos, amovibles sur des petits PCBs dont les schémas viendront bientôt
-Des bandes leds, beaucoup de bandes leds !

Il y a pour l'instant 3 PCBs:
-Un qui gère les leds RGB du plafond et la bande led verte de la porte du salon
-Un pour la bande led mauve dans la rigole en bois au dessus de la porte du salon et pour la led orange autour du panneau belgacom (Il devra aussi bientôt gérer les yeux du type sur le panneau)
-Un pour la bande led de la rigole du couloir et pour la bande RGB derrière le panneau A.L.C.O.O.L.

## Software
Les codes des trois wemos sont dans les trois dossiers du répertoire dans l'ordre cité si dessus

## Problèmes connus :
Actuellement, le troisième PCB n'est pas encore opérationnel
