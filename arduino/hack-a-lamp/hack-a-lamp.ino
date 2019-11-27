#include "config.h"

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

char* ssid = WIFI_SSID;
char* password = WIFI_PASSWORD;

char* mqtt_server = MQTT_HOST;
unsigned int mqtt_port = MQTT_PORT;
char* clientId = MQTT_CLIENT_ID;

unsigned long lastTry;

WiFiClient espClient;
PubSubClient client(espClient);

void onMessageReceive(char* topic, byte* payload, unsigned int length) {
  #ifdef DEBUG
    Serial.print("TOPIC: ");
    Serial.print(topic);
    Serial.print(" | VALUE: ");
    Serial.println((char)payload[0]);
  #endif
  
  if (strcmp(topic, (String(clientId) + "/light").c_str()) == 0) {    
    switch((char)payload[0]){
      case '0': // OFF
        digitalWrite(PIN_LIGHT, HIGH);
        break;
      case '1': // ON
        digitalWrite(PIN_LIGHT, LOW);
        break;
    }
  }
}

bool isWifiReady() {
  return WiFi.status() == WL_CONNECTED;
}

bool isMQTTReady() {
  return client.connected();
}

bool checkWifi() {
  if (isWifiReady()) return true;

  if (WiFi.status() == WL_CONNECTION_LOST) {
    #ifdef DEBUG
      Serial.print("WIFI;OFF;");
      Serial.println(WiFi.status());
    #endif
  }
  
  return false;
}

void connectWIFI() {
  while (WiFi.status() != WL_CONNECTED) {
    #ifdef DEBUG
      Serial.println("WIFI;TRY");
    #endif
    
    if (WiFi.status() == WL_CONNECT_FAILED) {
      #ifdef DEBUG
        Serial.print("WIFI;OFF;");
        Serial.println(WiFi.status());
      #endif
      return;
    }
  
    delay(1000);
  }

  #ifdef DEBUG
    Serial.println("WIFI;ON");
  #endif

  client.setServer(mqtt_server, mqtt_port);
  connectMQTT();
}

void connectMQTT() {
  #ifdef DEBUG
    Serial.println("MQTT;TRY");
  #endif
  
  if (millis() > lastTry + RETRY_MQTT_CONNECT) {
    lastTry = millis();
    
    if (isWifiReady() && mqtt_server) {
      
      if (isMQTTReady()) {
        #ifdef DEBUG
          Serial.println("MQTT;ON");
        #endif
      }
      else {
        String willTopic = String(clientId) + "/status";
        const char* willTopic_ = willTopic.c_str();
        
        if (client.connect(clientId, NULL, NULL, willTopic_, 0, 1, "OFFLINE", true)) {
          #ifdef DEBUG
            Serial.println("MQTT;ON");
          #endif
          client.publish(willTopic_, "ONLINE", true);
          client.subscribe((String(clientId) + "/#").c_str());
        }
        else {
          #ifdef DEBUG
            Serial.print("MQTT;ERR;");
            Serial.println(client.state());
          #endif
        }
      }
    }
  }
}

void setup() {
#ifdef DEBUG
  Serial.begin(SERIAL_BAULRATE);
  delay(200);
  
#endif

  pinMode(PIN_LIGHT, OUTPUT);
  digitalWrite(PIN_LIGHT, HIGH);

  client.setCallback(onMessageReceive);

  delay(200);

#ifdef DEBUG
  Serial.println("READY");
#endif
  
  WiFi.mode(WIFI_STA);
  WiFi.hostname(WIFI_HOSTNAME);
  WiFi.begin(ssid, password);

  connectWIFI();
}

void loop() {
  if (checkWifi()) {
    client.loop();
    if (!isMQTTReady()) connectMQTT();
  }
}
