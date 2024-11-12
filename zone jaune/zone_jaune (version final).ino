#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <DHT.h>

#define DHTPIN D2      // Broche de données du capteur DHT
#define DHTTYPE DHT22  // Type de capteur utilisé (DHT11 ou DHT22)

// Définition des broches des capteurs
const int motionSensor = D1;  // Capteur de mouvement
const int gasSensor = A0;      // Capteur de gaz
const int led_pin = D7;        // LED

const char* ssid = "LOM";                 // SSID du réseau WiFi
const char* password = "adminadmin";      // Mot de passe du réseau WiFi
const char* mqtt_server = "192.168.1.137";
const int mqtt_port = 1883;
const char* mqtt_user = "grp2";           // Nom d'utilisateur MQTT
const char* mqtt_password = "adminadmin";  // Mot de passe MQTT
const char* mqtt_topic = "zone-jaune";

WiFiClient espClient;
PubSubClient client(espClient);

DHT dht(DHTPIN, DHTTYPE);  // Initialisation du capteur DHT

void setup_wifi() {
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

  // Configuration des broches
  pinMode(LED_BUILTIN, OUTPUT);  // LED intégrée
  pinMode(motionSensor, INPUT);  // Capteur de mouvement
  pinMode(led_pin, OUTPUT);      // LED

  // Initialisation du capteur DHT
  dht.begin();
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Lecture du capteur de mouvement
  int isMotionDetected = digitalRead(motionSensor);

  // Affichage des informations du capteur de mouvement
  String motionStatus = (isMotionDetected == HIGH) ? "détecté" : "non détecté";
  Serial.println("Mouvement : " + motionStatus);

  // Lecture des informations du capteur DHT
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  // Affichage des informations du capteur DHT
  if (!isnan(humidity) && !isnan(temperature)) {
    Serial.print("Humidité : ");
    Serial.print(humidity);
    Serial.print("%\t");
    Serial.print("Température : ");
    Serial.print(temperature);
    Serial.println("°C");
  } else {
    Serial.println("Échec de lecture du capteur DHT !");
  }

  // Lecture des informations du capteur de gaz (CO2)
  float co2Level = analogRead(gasSensor) * 100.0 / 1023;

  // Affichage des informations du capteur de gaz (CO2)
  Serial.print("Niveau de CO2 : ");
  Serial.print(co2Level);
  Serial.println("%");

  // Envoi des données via MQTT
  String payload = "Mouvement : " + motionStatus + ", Humidité : " + String(humidity) + "%, Température : " + String(temperature) + "°C, Niveau de CO2 : " + String(co2Level) + "%";
  client.publish(mqtt_topic, payload.c_str());

  // Contrôle de la LED en fonction du mouvement détecté
  if (isMotionDetected == HIGH) {
    digitalWrite(led_pin, HIGH);
  } else {
    digitalWrite(led_pin, LOW);
  }

  delay(2000);
}
