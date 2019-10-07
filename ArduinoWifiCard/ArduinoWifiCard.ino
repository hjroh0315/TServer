#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>

#ifndef STASSID
#define STASSID "SK_WiFiGIGAA904"
#define STAPSK  "1903046404"
#endif

const char* ssid     = STASSID;
const char* password = STAPSK;

const char* host = "somnus.serveo.net";
const uint16_t port = 80;


#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN         5          // Configurable, see typical pin layout above
#define SS_PIN          4         // Configurable, see typical pin layout above

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance

unsigned long getID(){
  if ( ! mfrc522.PICC_ReadCardSerial()) { //Since a PICC placed get Serial and continue
    return -1;
  }
  unsigned long hex_num;
  hex_num =  mfrc522.uid.uidByte[0] << 24;
  hex_num += mfrc522.uid.uidByte[1] << 16;
  hex_num += mfrc522.uid.uidByte[2] <<  8;
  hex_num += mfrc522.uid.uidByte[3];
  mfrc522.PICC_HaltA(); // Stop reading
  return hex_num;
}

void setup() {
	Serial.begin(115200);		// Initialize serial communications with the PC
	while (!Serial);		// Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
	SPI.begin();			// Init SPI bus
	mfrc522.PCD_Init();		// Init MFRC522
	delay(4);				// Optional delay. Some board do need more time after init to be ready, see Readme
	mfrc522.PCD_DumpVersionToSerial();	// Show details of PCD - MFRC522 Card Reader details
	Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  if(WiFi.status()!=WL_CONNECTED)return;
  HTTPClient http;
  http.begin("http://somnus.serveo.net/api/patient");
  http.addHeader("Content-Type", "application/x-www-form-urlencoded"); 
  
	// Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
	if ( mfrc522.PICC_IsNewCardPresent()) {
		unsigned long uid = getID();
    printf("0x%08x\n", uid);
    switch(uid)
    {
      case 0x2B534DBE:
        printf("노인, 3sec\n");
        http.POST("time=3&");
        break;
      case 0x7B2164BE:
        printf("장애인, 4sec\n");
        http.POST("time=4&");
        break;
      case 0x5BE04DBE:
        printf("장애인, 5sec\n");
        http.POST("time=5&");
        break;
      case 0x6B006DBE:
        printf("장애인, 6sec\n");
        http.POST("time=6&");
        break;
    }
    
    http.end();
	}
	// Dump debug info about the card; PICC_HaltA() is automatically called
	//mfrc522.PICC_DumpToSerial(&(mfrc522.uid));
  delay(2000);
}
