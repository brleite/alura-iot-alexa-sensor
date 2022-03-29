#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

#define RELAY D2

/************************* WiFi Access Point *********************************/

#define WLAN_SSID       "rede"         // TROQUE PELO NOME DA SUA REDE
#define WLAN_PASS       "senha"        // TROQUE PELA SENHA DA SUA REDE

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  8883
#define AIO_USERNAME    "usuario"     // TROQUE PELO SEU USUARIO
#define AIO_KEY         "chave"       // TROQUE PELO SUA CHAVE

/*************************    Global State    *******************************/

WiFiClientSecure client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_USERNAME, AIO_KEY);

/****************************** Feeds ***************************************/

Adafruit_MQTT_Subscribe escritorio = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/escritorio");

/*************************** Sketch Code ************************************/

void MQTT_connect();

void setup() {
  pinMode(RELAY, OUTPUT);
  digitalWrite(RELAY, HIGH);

  Serial.begin(115200);
  delay(10);

  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");

  mqtt.subscribe(&escritorio);
}

void loop() {

  MQTT_connect();

  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(5000))) {
    if (subscription == &escritorio) {
      Serial.print(F("On-Off button: "));
      Serial.println((char *)escritorio.lastread);
      
      if (strcmp((char *)escritorio.lastread, "ON") == 0) {
        digitalWrite(RELAY, LOW); 
      }
      if (strcmp((char *)escritorio.lastread, "OFF") == 0) {
        digitalWrite(RELAY, HIGH); 
      }
    }
  }

  if(! mqtt.ping()) {
    mqtt.disconnect();
  }
}

void MQTT_connect() {
  int8_t ret;

  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) {
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);
       retries--;
       if (retries == 0) {
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}
