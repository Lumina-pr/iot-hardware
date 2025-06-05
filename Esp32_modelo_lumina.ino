#include <WiFi.h>
#include <PubSubClient.h>

// WiFi and MQTT Configuration
const char* ssid = "Felipe Trolao";
const char* password = "mugiwara";
const char* mqtt_server = "186.64.123.246";
const int mqtt_port = 1883;

// Unique device ID
const char* deviceId = "683de0089d9e7e653d66bc67";

// MQTT Topics
String pingTopic = String("device/") + deviceId + "/ping";
String pulseTopic = String("device/") + deviceId + "/pulse";

//Timer
unsigned long previousMillis = 0;

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  delay(10);
  Serial.println("Conectando a WiFi...");
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("WiFi conectado");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

void sender(){
  Serial.println("Rhym with Grug");
  
  int Voltage = random(18, 25);
  int Current = random(10, 120);

  client.publish(pulseTopic.c_str(), ("{\"status\":\"ok\",\"data\":{\"voltage\":" + String(Voltage) + ",\"current\":" + String(Current) + "}}").c_str());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Mensaje recibido [");
  Serial.print(topic);
  Serial.print("] ");
  
  if (String(topic) == pingTopic) {
    Serial.println("Ping recibido! Respondiendo con reply...");
    
    // Reply to the correct topic that NestJS expects
    String replyTopic = pingTopic + "/reply";
    
    client.publish(replyTopic.c_str(), "{\"status\":\"ok\"}");
    
    Serial.println("Mensaje enviado");
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Conectando a MQTT...");
    if (client.connect(deviceId)) {
      Serial.println("conectado");
      
      // Subscribe to ping topic
      client.subscribe(pingTopic.c_str());
    } else {
      Serial.print("falló, rc=");
      Serial.print(client.state());
      Serial.println(" reintentando en 5 segundos");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= 3000) {
    previousMillis = currentMillis;
    sender();
  }
}