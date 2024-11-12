#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <DHT.h>

#define DHTPIN D2       // Broche de données du capteur DHT
#define DHTTYPE DHT11   // Utilisation du capteur DHT11

// Initialise le capteur DHT
DHT dht(DHTPIN, DHTTYPE);

const int gasSensor = A0; // Broche du capteur de gaz
const int ledPin = D7;    // Broche de la LED

const char* ssid = "LOM"; // Modifiez avec votre SSID
const char* password = "adminadmin"; // Modifiez avec votre mot de passe
const char* mqtt_server = "192.168.1.137";
const int mqtt_port = 1883;
const char* mqtt_user = "grp2"; // Laissez vide si non nécessaire
const char* mqtt_password = "adminadmin"; // Laissez vide si non nécessaire
const char* mqtt_topic = "zone-verte";

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

  // Initialise le capteur DHT
  dht.begin();

  // Configure la broche de la LED comme sortie
  pinMode(ledPin, OUTPUT);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Lecture de l'humidité relative
  float humidity = dht.readHumidity();
  
  // Lecture de la température en degrés Celsius
  float temperature = dht.readTemperature();

  // Vérifie si la lecture du capteur a échoué
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Échec de la lecture du capteur DHT !");
    return;
  }

  // Lecture du niveau de gaz (MQ-5)
  float gasLevel = analogRead(gasSensor) * 100.0 / 1023;

  // Formatage des données dans une seule chaîne
  String data = "Humidité : " + String(humidity) + "%, Température : " + String(temperature) + "°C, Niveau de gaz : " + String(gasLevel) + "%";

  // Affiche les données sur le moniteur série
  Serial.println(data);

  // Allume la LED si le niveau de gaz dépasse 25 %
  digitalWrite(ledPin, gasLevel > 25 ? HIGH : LOW);

  // Envoi des données MQTT
  client.publish(mqtt_topic, data.c_str());

  delay(2000); // Attendre avant de publier de nouvelles données
}
