const int laser_din = A0; // Définir la broche à laquelle le capteur laser est connecté

void setup() {
  pinMode(laser_din, INPUT); // Définir la broche du capteur laser comme une entrée
  Serial.begin(9600); // Initialiser la communication série
}

void loop() {
  int laserValeur = analogRead(laser_din); // Lire la valeur analogique du capteur laser
  
  // Si la valeur est inférieure à une certaine limite, un obstacle est détecté
  if (laserValeur < 100) { // Vous devrez ajuster cette limite en fonction de votre capteur et de votre environnement
    Serial.println("Obstacle détecté !");
  } else {
    Serial.println("Pas d'obstacle.");
  }
  
  delay(500); // Attendre un court instant avant de lire à nouveau
}
