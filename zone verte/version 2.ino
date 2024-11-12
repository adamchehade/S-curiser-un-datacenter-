void setup() {
  Serial.begin(115200);
  delay(10);
  Serial.println("Starting...");
}

void loop() {
  float h = analogRead(A0);
  if (isnan(h)) {
    Serial.println("Failed to read from MQ-5 sensor!");
    return;
  }

  Serial.print("Gas Level: ");
  Serial.println(h / 1023 * 100);
 
  delay(1500); // Minimum delay between updates for ThingSpeak
}
