#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <DHT.h>
#include <pitches.h>

#define DHTPIN D2     // Broche de données du capteur DHT
#define DHTTYPE DHT22   // Type de capteur utilisé (DHT11 ou DHT22)

// Initialise le capteur DHT
DHT dht(DHTPIN, DHTTYPE);

// Définition des broches des capteurs
const int motionSensor = D1; // Capteur de mouvement
const int gasSensor = A0; // Capteur de gaz
const int laser_din = D6; // Capteur laser
const int buzzer_pin = D3; // Buzzer
const int led_pin = D7; // LED

const char* ssid = "LOM"; // SSID du réseau WiFi
const char* password = "adminadmin"; // Mot de passe du réseau WiFi
const char* mqtt_server = "192.168.1.137";
const int mqtt_port = 1883;
const char* mqtt_user = "grp2"; // Nom d'utilisateur MQTT
const char* mqtt_password = "adminadmin"; // Mot de passe MQTT
const char* mqtt_topic = "zone-rouge";

WiFiClient espClient;
PubSubClient client(espClient);

// Mélodie pour "La Panthère Rose"
int melody[] = {
  NOTE_D4, NOTE_FS4, NOTE_G4, NOTE_D5, NOTE_FS5, NOTE_G5,
  NOTE_D5, NOTE_CS5, NOTE_B4, NOTE_G4, NOTE_FS4, NOTE_G4,
  NOTE_D4, NOTE_FS4, NOTE_G4, NOTE_D5, NOTE_FS5, NOTE_G5,
  NOTE_D5, NOTE_CS5, NOTE_B4, NOTE_G4, NOTE_FS4, NOTE_G4
};

// Durée de chaque note
int noteDurations[] = {
  4, 8, 8, 4, 8, 8,
  4, 8, 8, 4, 8, 8,
  4, 8, 8, 4, 8, 8,
  4, 8, 8, 4, 8, 8
};

bool obstacleDetected = false;
unsigned long obstacleDetectedTime = 0;

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

  // Configuration des broches
  pinMode(LED_BUILTIN, OUTPUT); // LED intégrée
  pinMode(motionSensor, INPUT); // Capteur de mouvement
  pinMode(laser_din, INPUT); // Capteur laser
  pinMode(led_pin, OUTPUT); // LED

  // Initialisation du capteur DHT
  dht.begin();
}

unsigned long currentTime;
const unsigned long motionCheckInterval = 150UL;
unsigned long previousMotionCheckTime = 0;
bool motionDetected = false;

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  currentTime = millis();

  if (currentTime - previousMotionCheckTime >= motionCheckInterval) {
    int isMotionDetected = digitalRead(motionSensor);
    int laserValue = digitalRead(laser_din);

    if (isMotionDetected == HIGH || laserValue == LOW) {
      if (!motionDetected) {
        Serial.println("Mouvement ou obstacle détecté !");
        motionDetected = true;
        digitalWrite(led_pin, HIGH);
        client.publish(mqtt_topic, "Mouvement ou obstacle détecté !");
      }
    } else {
      if (motionDetected) {
        Serial.println("Mouvement ou obstacle terminé !");
        motionDetected = false;
        digitalWrite(led_pin, LOW);
        client.publish(mqtt_topic, "Mouvement ou obstacle terminé !");
      }
    }

    previousMotionCheckTime = currentTime;
  }

  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Échec de la lecture du capteur DHT !");
  } else {
    Serial.print("Humidité : ");
    Serial.print(humidity);
    Serial.print("%\t");

    Serial.print("Température : ");
    Serial.print(temperature);
    Serial.println("°C");

    String payload = String("Humidité: ") + humidity + "%, Température: " + temperature + "C";
    client.publish(mqtt_topic, payload.c_str());
  }

  float gasLevel = analogRead(gasSensor) * 100.0 / 1023;

  Serial.print("Niveau de gaz : ");
  Serial.print(gasLevel);
  Serial.println("%");

  client.publish(mqtt_topic, String("Niveau de gaz : " + String(gasLevel) + "%").c_str());

  if (gasLevel > 35) {
    playMelody();
  }

  delay(2000);
}

void playMelody() {
  for (int thisNote = 0; thisNote < sizeof(melody) / sizeof(melody[0]); thisNote++) {
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(buzzer_pin, melody[thisNote], noteDuration);
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    noTone(buzzer_pin);
  }
}
