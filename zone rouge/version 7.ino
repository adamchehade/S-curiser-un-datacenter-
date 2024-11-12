#include <DHT.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_PN532.h>

#define DHTPIN D2     // Broche de données du capteur DHT
#define DHTTYPE DHT22   // Type de capteur utilisé (DHT11 ou DHT22)

// Initialise le capteur DHT
DHT dht(DHTPIN, DHTTYPE);

// Définition de la broche du capteur de mouvement
const int motionSensor = A0;

#define PN532_SCK  D5
#define PN532_MISO D6
#define PN532_MOSI D7
#define PN532_SS   D8
#define PN532_IRQ  (2)
#define PN532_RESET (0)

Adafruit_PN532 nfc(PN532_SCK, PN532_MISO, PN532_MOSI, PN532_SS);

void setup() {
  // Met en place le moniteur série
  Serial.begin(9600);
  Serial.println("Initialisation...");

  // Configure la broche LED_BUILTIN en sortie
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW); // Allume la LED au démarrage

  // Configure la broche du capteur de mouvement en entrée
  pinMode(motionSensor, INPUT);

  // Initialise le capteur DHT
  dht.begin();

  Serial.println("Hello!");

  nfc.begin();
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (!versiondata) {
    Serial.print("Didn't find PN53x board");
    while (1); // halt
  }
  
  nfc.SAMConfig();
  
  Serial.println("Waiting for an NFC tag...");
}

unsigned long currentTime;

// Intervalle de vérification du mouvement
const unsigned long motionCheckInterval = 5000UL;
unsigned long previousMotionCheckTime = 0;

// Intervalle de lecture des capteurs (température, humidité, gaz)
const unsigned long sensorReadInterval = 10000UL;
unsigned long previousSensorReadTime = 0;

bool motionDetected = false; // Variable pour suivre l'état du mouvement

void loop() {
  // Obtient le temps actuel en millisecondes
  currentTime = millis();

  // Vérifie si l'intervalle de temps est écoulé pour la vérification du mouvement
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
      }
    } else { // Si le mouvement n'est pas détecté
      if (motionDetected) { // Si le mouvement vient de se terminer
        // Affiche un message sur le moniteur série
        Serial.println("Mouvement terminé !");
        motionDetected = false; // Met à jour l'état du mouvement
        // Éteint la LED
        digitalWrite(LED_BUILTIN, LOW);
      }
    }

    // Met à jour le temps de la dernière vérification de mouvement
    previousMotionCheckTime = currentTime;
  }

  // Vérifie si l'intervalle de temps est écoulé pour la lecture des capteurs
  if (currentTime - previousSensorReadTime >= sensorReadInterval) {
    // Lecture de l'humidité relative
    float humidity = dht.readHumidity();
    
    // Lecture de la température en degrés Celsius
    float temperature = dht.readTemperature();

    // Vérifie si la lecture du capteur a échoué
    if (isnan(humidity) || isnan(temperature)) {
      Serial.println("Échec de la lecture du capteur DHT !");
    } else {
      // Affiche les données lues sur le moniteur série
      Serial.print("Humidité : ");
      Serial.print(humidity);
      Serial.print("%\t");

      Serial.print("Température : ");
      Serial.print(temperature);
      Serial.println("°C");
    }

    // Lecture du niveau de gaz (MQ-5)
    float gasLevel = analogRead(A0) / 1023.0 * 100;

    // Affichage du niveau de gaz
    Serial.print("Niveau de gaz : ");
    Serial.print(gasLevel);
    Serial.println("%");

    // Met à jour le temps de la dernière lecture des capteurs
    previousSensorReadTime = currentTime;
  }

  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)

  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);

  if (success) {
    Serial.println("Found an NFC tag!");

    Serial.print("UID Length: ");Serial.print(uidLength, DEC);Serial.println(" bytes");
    Serial.print("UID Value: ");
    for (uint8_t i=0; i < uidLength; i++) {
      Serial.print(" 0x");Serial.print(uid[i], HEX);
    }
    Serial.println("");
  }

  // Attendre une courte période entre les lectures
  delay(100);
}
