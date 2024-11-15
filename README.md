

Dans un datacenter, les risques sont multiples, c’est pourquoi vous allez multiplier les mesures de sécurité et de redondance : électrique, thermique, réseau, incendie, etc. 

La technologie (biométrie, vidéosurveillance, intrusion) s’accompagne toujours du contrôle humain et vous mettrez en place un système de supervision des données sensibles de votre datacenter. Une  équipe sera en place dans la salle de surveillance 24h/24, 7j/7 et 365j/an.

Ce travail va consister à tester le système de protection que vous allez mettre en place.

Dans un premier temps, mettre en place chaque application ( MQTT, Nodered, GRAFANA, InfluxDB) et pour dialoguer avec les différents capteurs proposés.

Dans un second temps,  mettre en place l'ensemble des capteurs qui vous permettront de sécuriser ce datacenter à savoir:

zone rouge=>Coeur du datacenter: lecteur de carte RFID/ capteur de vibration/détecteur de présence/capteur température de d'humidité/capteur de fumée/ LED jaune
zone jaune=>Zone technique: détecteur de présence/capteur température de d'humidité/capteur de fumée/LED verte
zone verte=> Hall d'accueil/salle de réunion...: capteur température de d'humidité/capteur de fumée
Les led vertes et jaune simulent le fonctionnement de caméra lors de la détection d'une personne
Une LED ROUGE simulera l'alarme incendie

MATERIEL:
Borne Linksys
2 Câbles réseaux
3 (ESP8266 + cables USB-µUSB)
2 raspberryPi 4 +cartes SD+lecture de carte SD
2 ordinateurs portables
2 claviers + souris + écrans HDMI
2 alim 220V USB-C
Câbles Dupont male-male
Câbles Dupont male-femelle
3 résistances 10kohms
3 résistances de 220ohms
3 LED ( rouge/jaune/vert)
3 capteurs de fumée MQ135
3 capteurs de température et d'humidité RHT03
2 capteurs de présence
1 capteur RFID /NFC
Un capteur de vibration "tilt"
3 plaques de connexion
