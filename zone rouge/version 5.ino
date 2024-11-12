#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <DHT.h>

#define DHTPIN D2     // Broche de données du capteur DHT
#define DHTTYPE DHT22   // Type de capteur utilisé (DHT11 ou DHT22)

// Initialise le capteur DHT
DHT dht(DHTPIN, DHTTYPE);

// Définition de la broche du capteur de mouvement
const int motionSensor = A0;
const int gasSensor = A0; // Broche du capteur de gaz

const char* ssid = "LOM"; // Modifiez avec votre SSID
const char* password = "adminadmin"; // Modifiez avec votre mot de passe
const char* mqtt_server = "192.168.1.137";
const int mqtt_port = 1883;
const char* mqtt_user = "grp2"; // Laissez vide si non nécessaire
const char* mqtt_password = "adminadmin"; // Laissez vide si non nécessaire
const char* mqtt_topic = "maison/capteur";

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connexion au réseau WiFi: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connecté");
  Serial.println("Adresse IP: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Tentative de connexion MQTT ... ");
    if (client.connect("ESP8266Client", mqtt_user, mqtt_password)) {
      Serial.println("Connecté");
    } else {
      Serial.print("Échec, rc=");
      Serial.print(client.state());
      Serial.println(" Nouvelle tentative dans 5 secondes");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(9600);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);

  // Configure la broche LED_BUILTIN en sortie
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW); // Allume la LED au démarrage

  // Configure la broche du capteur de mouvement en entrée
  pinMode(motionSensor, INPUT);

  // Initialise le capteur DHT
  dht.begin();
}

unsigned long currentTime;

// Intervalle de vérification du mouvement
const unsigned long motionCheckInterval = 150UL;
unsigned long previousMotionCheckTime = 0;

bool motionDetected = false; // Variable pour suivre l'état du mouvement

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Obtient le temps actuel en millisecondes
  currentTime = millis();

  // Vérifie si l'intervalle de temps est écoulé
  if (currentTime - previousMotionCheckTime >= motionCheckInterval) {
    // Lit l'état du capteur de mouvement
    int isMotionDetected = analogRead(motionSensor);

    // Si le mouvement est détecté
    if (isMotionDetected > 100) { // Vous pouvez ajuster la valeur de seuil selon vos besoins
      if (!motionDetected) { // Si le mouvement vient de commencer
        // Affiche un message sur le moniteur série
        Serial.println("Mouvement détecté !");
        motionDetected = true; // Met à jour l'état du mouvement
        // Allume la LED
        digitalWrite(LED_BUILTIN, HIGH);

        // Envoie l'information de détection de mouvement via MQTT
        client.publish(mqtt_topic, "Mouvement détecté !");
      }
    } else { // Si le mouvement n'est pas détecté
      if (motionDetected) { // Si le mouvement vient de se terminer
        // Affiche un message sur le moniteur série
        Serial.println("Mouvement terminé !");
        motionDetected = false; // Met à jour l'état du mouvement
        // Éteint la LED
        digitalWrite(LED_BUILTIN, LOW);

        // Envoie l'information de fin de mouvement via MQTT
        client.publish(mqtt_topic, "Mouvement terminé !");
      }
    }

    // Met à jour le temps de la dernière vérification
    previousMotionCheckTime = currentTime;
  }

  // Lecture de l'humidité relative
  float humidity = dht.readHumidity();
  
  // Lecture de la température en degrés Celsius
  float temperature = dht.readTemperature();

  // Vérifie si la lecture du capteur a échoué
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Échec de la lecture du capteur DHT !");
    return;
  }

  // Affiche les données lues sur le moniteur série
  Serial.print("Humidité : ");
  Serial.print(humidity);
  Serial.print("%\t");

  Serial.print("Température : ");
  Serial.print(temperature);
  Serial.println("°C");

  // Lecture du niveau de gaz (MQ-5)
  float gasLevel = analogRead(gasSensor) / 1023.0 * 100;

  // Affichage du niveau de gaz
  Serial.print("Niveau de gaz : ");
  Serial.print(gasLevel);
  Serial.println("%");

  // Envoi des données MQTT
  String payload = String("Humidité: ") + humidity + "%, Température: " + temperature + "C, Gaz: " + gasLevel + "%";
  client.publish(mqtt_topic, payload.c_str());

  delay(2000); // Attendre avant de publier de nouvelles données
}
