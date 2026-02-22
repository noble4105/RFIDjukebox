#include <SPI.h>
#include <MFRC522.h>
#include <Audio.h>
#include <WiFi.h>
#include <pgmspace.h>

#define RST_PIN         10        
#define SS_1_PIN        21       

const int wificount = 4;
const char* ssid[wificount] = {"OAK_Div001", "2.4ext", "Wells Wifi 2", "Joe Booty"};
const char* password[wificount] = {"Avon@1320", "Wifi1234", "DDTC1964", "dirtylittleslut"}; 

typedef struct { //Database struct for the different songs and their respective titles and tag IDs
  char tag[32];
  char url[256];
  char name[32];
} discid;

 // DISC TAG AND URL LISTING
const discid ids[] PROGMEM = {
  { " 04 82 C9 56 19 61 80", "https://drive.google.com/uc?export=download&id=1SLTJFwr5t6Qkim_Rnk2ACSPftEu_Fi8x", "Strad" },
  { " 04 FD A7 57 19 61 80", "https://drive.google.com/uc?export=download&id=1SLTJFwr5t6Qkim_Rnk2ACSPftEu_Fi8x", "Strad"},
  { " 04 2B FA 56 19 61 80", "https://drive.google.com/uc?export=download&id=1rOawNFF2dyU8UrqcGXJnEA0Xnu0HO6MK", "Stal" },
  { " 04 8E 2D 57 19 61 80", "https://drive.google.com/uc?export=download&id=1HSbeSOiyz2W6UItoESfwlM4eHZFqV3xr", "Cat" },
  { " 04 E0 17 58 19 61 80", "https://drive.google.com/uc?export=download&id=1UtodT8X90msYB383CrfXmplhruQ2fThJ", "Wait" },
  { " 04 66 DB 56 19 61 80", "https://drive.google.com/uc?export=download&id=1VFxzOojATp37kjynvDMtJ1au-BN7yjB6", "Otherside" },
  { " 04 26 27 57 19 61 80", "https://drive.google.com/uc?export=download&id=1dMgkd3t2chCCVsjvlveSqzOWBe8EetuY", "Pigstep" },
  { " 04 03 37 56 19 61 80", "https://drive.google.com/uc?export=download&id=1PQivUtS2_A0cgnD69WkPxxBwEM-pX71Z", "Blocks" },
  { " 04 6B 23 58 19 61 80", "https://drive.google.com/uc?export=download&id=1IV7henjjskzEO6Ya8tR-7O2oUv6VbzY3", "Mellohi" },
  { " 04 19 69 56 19 61 80", "https://drive.google.com/uc?export=download&id=1nDUoE1ATC7Q8J31ENY_Oy5JQPciQ4uyc", "Chirp" },
  { " 04 1E E9 56 19 61 80", "https://drive.google.com/uc?export=download&id=1yVqohoypvgIBbH3komSk4ZPxf0OHBgG9", "Far" },
  { " 04 02 98 58 19 61 80", "https://drive.google.com/uc?export=download&id=1-WePQ0OSnk0xWJ39hGChJp54bULiVMTE", "Precipice" },
  { " 04 E0 5C 57 19 61 80", "https://drive.google.com/uc?export=download&id=1icx1UtEKhWiqBc_emVD85PK7LLxFrHug", "Creator" },
  { " 04 29 B0 56 19 61 80", "https://drive.google.com/uc?export=download&id=1V7naNydDrKnCNWBDaGZWTApUre95KYM_", "Thirteen"},
  { " 04 42 26 57 19 61 80", "https://drive.google.com/uc?export=download&id=13Vhj-HGMY1qqiauypwkwEeIWcas3lUCq", "Mall"},
  { " 04 73 65 56 19 61 80", "https://drive.google.com/uc?export=download&id=1yMIDiMIQtgWlbLExMJr448LTY7sPzVSm", "Ward"},
  { " 04 A4 DA 56 19 61 80", "https://drive.google.com/uc?export=download&id=1UYyAF4LCCUB2jUm-RhXgwXPwuYCiIWCk", "Eleven"},
  { " 04 AA 3E 57 19 61 80", "https://drive.google.com/uc?export=download&id=1BSYQgw_djBVuj6AwHUx3e_nLySnfeUjw", "Creator (Music Box)"},
  { " 04 65 8F 56 19 61 81", "https://drive.google.com/uc?export=download&id=1-f4JsN7YY0APEsC-_U_wPpvoD5AerTNM", "Five"},
  { " 04 35 3C 57 19 61 80", "https://drive.google.com/uc?export=download&id=1iq36PBHoPKDtSoQOxkeicHnm-UcrJTrX", "Relic"},
  { " 04 A6 D7 57 19 61 80", "https://drive.google.com/uc?export=download&id=1Vki-GStv42kmywZCZupybLchyCV-duWN", "Tears"},
  { " 04 A2 F8 57 19 61 80", "https://drive.google.com/uc?export=download&id=1vtagbQZUd2GYBzVlmNSMe6E2NGm2AtjC", "Lava Chicken"}
};

int led = 16; //LEDs for wifi connectivity info and for volume info
int volgreen = 33;
int volyellow = 32;
int volred = 14;

const int potpin = 34; //potentiometer pin
const int volumemin = 0; 
const int volumemax = 21;
int potval; // Mapping potentiometer value to volume values
int volume;
int lastvolume = -1;

void volumecontrol(void); // Real time volume indicating LEDs helper function
void connectwifi(void); // Wifi connection

const int songcount = sizeof(ids) / sizeof(ids[0]);

byte ssPins = SS_1_PIN;
Audio audio;
MFRC522 mfrc522;   // Create MFRC522 instance.

/**
 * Initialize.
 */
void setup() {
  pinMode(led, OUTPUT);
  pinMode(volgreen, OUTPUT);
  pinMode(volred, OUTPUT);
  pinMode(volyellow, OUTPUT);

  Serial.begin(115200); // Initialize serial communications with the PC
  delay(100);
  Serial.flush();
  while (!Serial);    // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
  bool connected = false;

  // WIFI SETUP
  connectwifi();
  Serial.println("Connected to: " + WiFi.SSID());

// AUDIO SETUP
  audio.setPinout(22, 25, 26);
  audio.setVolume(9);

  SPI.begin();        // Init SPI bus

  mfrc522.PCD_Init(ssPins, RST_PIN); // Init each MFRC522 card
  mfrc522.PCD_DumpVersionToSerial();

  Serial.println("Ready to scan! :)");
  Serial.println();
}

void loop() {
  //Volume control mapping
  volumecontrol();

  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()){
    return;
  }
  
  //Store latest scanned card onto string cardID
  char cardID[32] = "";
  byte lastUID[10];
  byte uidlength = mfrc522.uid.size;
  memcpy(lastUID, mfrc522.uid.uidByte, uidlength);

  for (byte i = 0; i < mfrc522.uid.size; i++) {
    char byteStr[4];  // 3 characters + null
    sprintf(byteStr, " %02X", mfrc522.uid.uidByte[i]);
    strcat(cardID, byteStr);
  }

    Serial.println("Card UID: ");
    Serial.println(cardID);
    discid current;

    for (int i = 0; i < songcount; i++) { // Scan tag list for match and stream associated disc url
      memcpy_P(&current, &ids[i], sizeof(discid));
      
      if (strcmp(cardID, current.tag) == 0) {
 
        Serial.print("Disc found c:\nNow playing "); 
        Serial.println(current.name);
        digitalWrite(led, HIGH);          // Print which song is playing, LED on whenever song is playing.
        delay(200);
        audio.connecttohost(current.url);

        unsigned long lastSeen = millis();
        const unsigned long readtimer = 500;

        while (true) {
          audio.loop();
          volumecontrol();
      
          static unsigned long lastCheck = 0;
         if (millis() - lastCheck > 100) {
          lastCheck = millis();

          if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
            if (mfrc522.uid.size == uidlength && memcmp(lastUID, mfrc522.uid.uidByte, uidlength) == 0) {
            lastSeen = millis(); // Same card still present
            }
          }
        }      

  // Exit playback if card has been gone too long
          if (millis() - lastSeen > readtimer) {
            Serial.println("Disc removed! Playback stopped.\n\n Ready to scan! :)");
            audio.stopSong();
            digitalWrite(led, LOW);
            break;
          }
        }
      }
    }

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
}
  
void volumecontrol(void) {
  potval = analogRead(potpin); //volume control realtime
  volume = map(potval, 0, 4095, volumemin, volumemax);

  if (volume != lastvolume) {
    audio.setVolume(volume);
    lastvolume = volume;
  }

  if (volume >= 14) { //LEDs Indicating volume level
     digitalWrite(volred, HIGH);
     digitalWrite(volyellow, HIGH);
     digitalWrite(volgreen, HIGH);
  } else if (volume >= 7) {
     digitalWrite(volred, LOW);
     digitalWrite(volyellow, HIGH);
     digitalWrite(volgreen, HIGH);
   } else if (volume > 0) {
    digitalWrite(volred, LOW);
    digitalWrite(volyellow, LOW);
    digitalWrite(volgreen, HIGH);
  } else {
    digitalWrite(volred, LOW);
    digitalWrite(volyellow, LOW);
    digitalWrite(volgreen, LOW);
  }
}

void connectwifi(void) {
  for (int i = 0; i < wificount; i++) { // Run through list of wifi's
    WiFi.begin(ssid[i], password[i]);
    Serial.println("\nAttempting to connect to wifi...");

    unsigned long startTime = millis();
    const unsigned long timeout = 2500;

    while (WiFi.status() != WL_CONNECTED && millis() - startTime < timeout) { // Blink slowly while attempting to connect
      digitalWrite(led, HIGH);
      delay(400);
      digitalWrite(led, LOW);
      delay(400);
    }

    if (WiFi.status() == WL_CONNECTED) { // Quick blink burst if it failed, then try next wifi settings
      Serial.println("Connected!");
      digitalWrite(led, HIGH);
      delay(2000);
      digitalWrite(led, LOW);
      Serial.println(WiFi.RSSI());
      break;
    }
    else {
      Serial.println("Attempt failed, moving on...");
      for (int j = 0; j < 3; j++) {
        digitalWrite(led, HIGH);
        delay(100);
        digitalWrite(led, LOW);
        delay(100);
      }
    }
  }
      if (WiFi.status() == WL_CONNECT_FAILED) {
      digitalWrite(led, HIGH);
      Serial.println("Couldn't connect to a network... ;-;");
    }
}