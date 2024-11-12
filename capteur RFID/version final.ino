#include <SPI.h>
#include <Wire.h>
#include <Adafruit_PN532.h>
#include <PubSubClient.h> // Inclure la bibliothèque MQTT
#include <ESP8266WiFi.h> // Inclure la bibliothèque Wi-Fi pour ESP8266

#define PN532_SCK   D5
#define PN532_MISO  D6
#define PN532_MOSI  D7
#define PN532_SS    D8
#define PN532_IRQ   (2)
#define PN532_RESET (0)

Adafruit_PN532 nfc(PN532_SCK, PN532_MISO, PN532_MOSI, PN532_SS);

#define MAX_CARTES 10

struct Carte {
  uint8_t uid[4];
  unsigned long lastScanTime;
  byte scanCount; // Nombre de scans du même ID
};

uint8_t idAdmin[] = {0x10, 0xDA, 0x39, 0x9C};
Carte cartesAcceptees[MAX_CARTES];
uint8_t nombreCartes = 0;
bool modeGestion = false;
uint8_t dernierUID[4] = {0, 0, 0, 0};

#define LED_ACCEPTED D2
#define LED_REJECTED D1
#define LED_MANAGEMENT D0

// Configuration MQTT
const char* mqtt_server = "192.168.1.137";
const int mqtt_port = 1883;
const char* mqtt_user = "grp2";
const char* mqtt_password = "adminadmin";
const char* mqtt_topic = "rfid"; // Nom du sujet MQTT pour les messages RFID

WiFiClient espClient; // Utilisation de WiFiClient pour la connexion Wi-Fi
PubSubClient client(espClient);

// Déclarations de fonctions
void setup(void);
void loop(void);
void connectMQTT();
void publishMQTT(const char* message, unsigned long timeDifference);
void callback(char* topic, byte* payload, unsigned int length);
void clignoterLED(int pin);

void setup(void) {
  Serial.begin(115200);
  Serial.println("Hello!");

  nfc.begin();
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (!versiondata) {
    Serial.print("Didn't find PN53x board");
    while (1); // halt
  }
  
  nfc.SAMConfig();

  pinMode(LED_ACCEPTED, OUTPUT);
  pinMode(LED_REJECTED, OUTPUT);
  pinMode(LED_MANAGEMENT, OUTPUT);

  Serial.println("Waiting for an NFC tag...");

  // Connexion Wi-Fi
  WiFi.begin("LOM", "adminadmin"); // Remplacer SSID et password par vos informations Wi-Fi

  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");

  // Connexion au broker MQTT
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  connectMQTT();
}

void loop(void) {
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };
  uint8_t uidLength;

  // Lecture RFID
  if (!modeGestion) {
    success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);

    if (success) {
      if (memcmp(uid, idAdmin, 4) == 0) {
        modeGestion = true;
        Serial.println("Mode gestion activé");
        digitalWrite(LED_MANAGEMENT, HIGH);
        publishMQTT("Mode gestion activé", 0); // Passer 0 pour le temps de différence dans le mode gestion
        delay(1000);
        return;
      }

      bool carteAcceptee = false;
      for (uint8_t i = 0; i < nombreCartes; ++i) {
        if (memcmp(cartesAcceptees[i].uid, uid, 4) == 0) {
          unsigned long currentTime = millis();
          unsigned long timeDifference = currentTime - cartesAcceptees[i].lastScanTime;
          Serial.print("Temps écoulé depuis le dernier scan de cette carte : ");
          Serial.print(timeDifference);
          Serial.println(" ms");
          if (cartesAcceptees[i].scanCount == 1) {
            Serial.println("Temps écoulé depuis le dernier scan remis à zéro.");
            timeDifference = 0;  // Remettre le temps à zéro
            cartesAcceptees[i].scanCount = 0;  // Remettre le compteur de scans à zéro
          } else {
            ++cartesAcceptees[i].scanCount;  // Incrémenter le compteur de scans
          }
          cartesAcceptees[i].lastScanTime = currentTime;  // Mettre à jour le dernier temps de scan
          carteAcceptee = true;
          String message = "Temps écoulé depuis le dernier scan : " + String(timeDifference / 1000) + " secondes"; publishMQTT(message.c_str(), timeDifference);
          break;
        }
      }

      if (carteAcceptee) {
        Serial.println("Carte acceptée !");
        clignoterLED(LED_ACCEPTED);
        publishMQTT("Carte acceptée !", 0); // Passer 0 pour le temps de différence dans le cas d'une carte acceptée
      } else {
        Serial.println("Carte non acceptée !");
        clignoterLED(LED_REJECTED);
        publishMQTT("Carte non acceptée !", 0); // Passer 0 pour le temps de différence dans le cas d'une carte non acceptée
      }

      Serial.print("UID Length: ");Serial.print(uidLength, DEC);Serial.println(" bytes");
      Serial.print("UID Value: ");
      for (uint8_t i=0; i < uidLength; i++) {
        Serial.print(" 0x");Serial.print(uid[i], HEX);
      }
      Serial.println("");
      delay(1000);
    }
  } else {
    success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);

    if (success) {
      if (memcmp(uid, idAdmin, 4) == 0 && memcmp(uid, dernierUID, 4) != 0) {
        modeGestion = false;
        Serial.println("Mode gestion désactivé");
        digitalWrite(LED_MANAGEMENT, LOW);
        publishMQTT("Mode gestion désactivé", 0); // Passer 0 pour le temps de différence dans le mode gestion désactivé
        delay(1000);
        return;
      }

      bool carteTrouvee = false;
      for (uint8_t i = 0; i < nombreCartes; ++i) {
        if (memcmp(cartesAcceptees[i].uid, uid, 4) == 0) {
          Serial.println("Carte retirée de la liste.");
          publishMQTT("Carte retirée de la liste.", 0); // Passer 0 pour le temps de différence dans le cas d'une carte retirée
          memcpy(cartesAcceptees[i].uid, cartesAcceptees[nombreCartes - 1].uid, 4);
          cartesAcceptees[i].lastScanTime = cartesAcceptees[nombreCartes - 1].lastScanTime;
          --nombreCartes;
          delay(1000);
          return;
        }
      }
      
      if (nombreCartes < MAX_CARTES) {
        memcpy(cartesAcceptees[nombreCartes].uid, uid, 4);
        cartesAcceptees[nombreCartes].lastScanTime = millis();
        ++nombreCartes;
        Serial.println("Carte ajoutée à la liste.");
        publishMQTT("Carte ajoutée à la liste.", 0); // Passer 0 pour le temps de différence dans le cas d'une carte ajoutée
        delay(1000);
        return;
      } else {
        Serial.println("Capacité maximale atteinte. Impossible d'ajouter plus de cartes.");
        delay(1000);
        return;
      }
    }
  }

  memcpy(dernierUID, uid, 4);

  if (modeGestion) {
    Serial.println("En mode gestion");
  } else {
    Serial.println("En attente d'une carte");
  }
}

void connectMQTT() {
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");

    if (client.connect("ESP8266Client", mqtt_user, mqtt_password)) {
      Serial.println("connected");
      client.subscribe(mqtt_topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void publishMQTT(const char* message, unsigned long timeDifference) {
  if (!client.connected()) {
    connectMQTT();
  }
  client.publish(mqtt_topic, message);
  if (timeDifference > 0) {
    String message = "Temps écoulé depuis le dernier scan : " + String(timeDifference / 1000) + " secondes"; client.publish(mqtt_topic, message.c_str());
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  // Fonction de rappel pour gérer les messages MQTT entrants (facultatif)
}

void clignoterLED(int pin) {
  digitalWrite(pin, HIGH);
  delay(5000); // 5 secondes
  digitalWrite(pin, LOW);
}
