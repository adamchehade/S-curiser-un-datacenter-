#include <DHT.h>

#define DHTPIN D2     // Broche de données du capteur DHT
#define DHTTYPE DHT22   // Type de capteur utilisé (DHT11 ou DHT22)

// Initialise le capteur DHT
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  Serial.println("Lecture du capteur DHT22...");

  // Initialise le capteur DHT
  dht.begin();
}

void loop() {
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
}
