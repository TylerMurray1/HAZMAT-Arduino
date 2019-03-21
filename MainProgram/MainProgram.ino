#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9

MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class

char weight[21];
char finalWeight[7];

void setup() {
  Serial.begin(9600);
  SPI.begin(); // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522
}

void loop() {
  getTag();
  Serial.flush();
}

void printHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? "0" : "");
    Serial.print(buffer[i], HEX);
  }
    Serial.print(":");
    Serial.println(finalWeight);
}

void getTag() {
  if ( ! rfid.PICC_IsNewCardPresent())
    return;

  MFRC522::StatusCode status;
  MFRC522::MIFARE_Key key;
  byte buffer[18];
  byte size = sizeof(buffer);
  for (byte i = 0; i < MFRC522::MF_KEY_SIZE; ++i) {
    key.keyByte[i] = 0xFF;
  }
  
  if (rfid.PICC_ReadCardSerial()) { 
    byte piccType = rfid.PICC_GetType(rfid.uid.sak);
    if( piccType == 4 ) {
    status = rfid.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 10, &key, &(rfid.uid));
      if (status == MFRC522::STATUS_OK) {
          status = rfid.MIFARE_Read(10, buffer, &size);
//          if (status == MFRC522::STATUS_OK) {
//              Serial.print(F("Data (block = 10): "));
//              Serial.print(char(buffer[0]));
//          }

          printHex(rfid.uid.uidByte, rfid.uid.size);
      }
    } else if ( piccType == 6 ) {
          status = rfid.MIFARE_Read(10, buffer, &size);
//        if (status == MFRC522::STATUS_OK) {
//            Serial.print(F("Data (block = 10): "));
//            Serial.print(char(buffer[0]));
//        }

          delay(3000);

          char compareWeight[7];

          for (int i = 0; i < 5; i += 1){
  
              Serial.readBytes(weight, 21);
              
              compareWeight[0] = weight[7];
              compareWeight[1] = weight[8];
              compareWeight[2] = weight[9];
              compareWeight[3] = weight[10];
              compareWeight[4] = weight[11];
              compareWeight[5] = weight[12];
        
              memcpy(finalWeight, compareWeight, 7);
          }

          printHex(rfid.uid.uidByte, rfid.uid.size);
          
    } else {
      Serial.println("RFID tag not recognized");
    }
  }

  // Halt PICC
  rfid.PICC_HaltA();

  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();
}
