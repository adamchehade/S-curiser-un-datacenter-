#include <SPI.h>
#include <Wire.h>
#include <Adafruit_PN532.h>

#define PN532_SCK   D5
#define PN532_MISO  D6
#define PN532_MOSI  D7
#define PN532_SS    D8
#define PN532_IRQ   (2)
#define PN532_RESET (0)

Adafruit_PN532 nfc(PN532_SCK, PN532_MISO, PN532_MOSI, PN532_SS);

#define MAX_CARTES 10

uint8_t idAdmin[] = {0x10, 0xDA, 0x39, 0x9C};
uint8_t cartesAcceptees[MAX_CARTES][4];
uint8_t nombreCartes = 0;
bool modeGestion = false;
uint8_t dernierUID[4] = {0, 0, 0, 0};

#define LED_ACCEPTED D2
#define LED_REJECTED D1
#define LED_MANAGEMENT D0

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
}

void loop(void) {
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };
  uint8_t uidLength;

  if (!modeGestion) {
    success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);

    if (success) {
      if (memcmp(uid, idAdmin, 4) == 0) {
        modeGestion = true;
        Serial.println("Mode gestion activé");
        digitalWrite(LED_MANAGEMENT, HIGH);
        delay(1000);
        return;
      }

      bool carteAcceptee = false;
      for (uint8_t i = 0; i < nombreCartes; ++i) {
        if (memcmp(cartesAcceptees[i], uid, 4) == 0) {
          carteAcceptee = true;
          break;
        }
      }

      if (carteAcceptee) {
        Serial.println("Carte acceptée !");
        digitalWrite(LED_ACCEPTED, HIGH);
        digitalWrite(LED_REJECTED, LOW);
      } else {
        Serial.println("Carte non acceptée !");
        digitalWrite(LED_ACCEPTED, LOW);
        digitalWrite(LED_REJECTED, HIGH);
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
        delay(1000);
        return;
      }

      bool carteTrouvee = false;
      for (uint8_t i = 0; i < nombreCartes; ++i) {
        if (memcmp(cartesAcceptees[i], uid, 4) == 0) {
          Serial.println("Carte retirée de la liste.");
          memcpy(cartesAcceptees[i], cartesAcceptees[nombreCartes - 1], 4);
          --nombreCartes;
          carteTrouvee = true;
          delay(1000);
          return;
        }
      }
      
      if (nombreCartes < MAX_CARTES) {
        memcpy(cartesAcceptees[nombreCartes], uid, 4);
        ++nombreCartes;
        Serial.println("Carte ajoutée à la liste.");
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
