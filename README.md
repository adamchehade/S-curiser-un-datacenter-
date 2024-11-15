# README: Sécurisation d'un Datacenter

## Introduction

Dans un datacenter, les risques sont nombreux. Il est donc crucial de multiplier les mesures de sécurité et de redondance dans différents domaines :  
- **Électrique**  
- **Thermique**  
- **Réseau**  
- **Incendie**, etc.  

La technologie, incluant des solutions comme la **biométrie**, la **vidéosurveillance** et les **systèmes anti-intrusion**, doit être accompagnée d’un contrôle humain. Pour cela, un **système de supervision des données sensibles** sera mis en place, avec une équipe en surveillance 24h/24, 7j/7 et 365j/an.  

Ce projet consiste à tester et valider un système de protection complet pour un datacenter.

---

## Objectifs

### Étape 1 : Installation des applications principales
1. **MQTT**  
2. **Node-RED**  
3. **Grafana**  
4. **InfluxDB**  

Ces applications permettront d’assurer la communication avec les différents capteurs et le traitement des données.

---

### Étape 2 : Mise en place des capteurs et zones de sécurité

#### **Zone rouge** : Cœur du datacenter  
- **Équipements** :  
  - Lecteur de carte RFID  
  - Capteur de vibration  
  - Détecteur de présence  
  - Capteur de température et d'humidité  
  - Capteur de fumée  
  - LED jaune  

#### **Zone jaune** : Zone technique  
- **Équipements** :  
  - Détecteur de présence  
  - Capteur de température et d'humidité  
  - Capteur de fumée  
  - LED verte  

#### **Zone verte** : Hall d'accueil / Salle de réunion  
- **Équipements** :  
  - Capteur de température et d'humidité  
  - Capteur de fumée  

#### **Fonctionnalités des LEDs** :
- **LED verte et jaune** : Simulent le fonctionnement d'une caméra lors de la détection d'une personne.  
- **LED rouge** : Simule une alarme incendie.  

---

## Matériel requis

### Réseau et connectique
- 1 borne **Linksys**  
- 2 câbles réseau  

### Microcontrôleurs et ordinateurs
- 3 **ESP8266** + câbles USB-µUSB  
- 2 **Raspberry Pi 4** + cartes SD + lecteur de carte SD  
- 2 ordinateurs portables  
- 2 claviers, souris et écrans HDMI  

### Alimentation
- 2 alimentations 220V USB-C  

### Câblage
- Câbles Dupont mâle-mâle  
- Câbles Dupont mâle-femelle  

### Électronique
- 3 résistances **10 kΩ**  
- 3 résistances **220 Ω**  
- 3 LEDs (rouge, jaune, verte)  

### Capteurs
- 3 capteurs de fumée **MQ135**  
- 3 capteurs de température et d'humidité **RHT03**  
- 2 capteurs de présence  
- 1 capteur **RFID / NFC**  
- 1 capteur de vibration **"tilt"**  

### Plaques de connexion
- 3 plaques de connexion  

---

## Conclusion

Ce projet intègre des solutions logicielles et matérielles pour tester un système de sécurité complet dans un datacenter. Grâce à des capteurs et des LEDs simulant des caméras et des alarmes, ce système vise à fournir une protection optimale contre les risques.

---

### Auteur
Adam CHEHADE
