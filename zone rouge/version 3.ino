#include <DHT.h>

#define DHTPIN D2     // Broche de données du capteur DHT
#define DHTTYPE DHT22   // Type de capteur utilisé (DHT11 ou DHT22)

// Initialise le capteur DHT
DHT dht(DHTPIN, DHTTYPE);

// Définition de la broche du capteur de mouvement
const int motionSensor = A0;

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
}

unsigned long currentTime;

// Intervalle de vérification du mouvement
const unsigned long motionCheckInterval = 150UL;
unsigned long previousMotionCheckTime = 0;

bool motionDetected = false; // Variable pour suivre l'état du mouvement

void loop() {
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

    // Met à jour le temps de la dernière vérification
    previousMotionCheckTime = currentTime;
  }

  // Attendre une courte période entre les lectures
  delay(2000);

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
  float gasLevel = analogRead(A0) / 1023.0 * 100;

  // Affichage du niveau de gaz
  Serial.print("Niveau de gaz : ");
  Serial.print(gasLevel);
  Serial.println("%");
}
