#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

// MQTT / WiFi settings
const char* ssid = "Wokwi-GUEST";
const char* password = "";
const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;
const char* publish_topic = "home/esp/dht";
const char* subscribe_topic = "home/esp/cmd";

WiFiClient espClient;
PubSubClient client(espClient);

#define DHTPIN 15
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

#define POT_PIN 34  
#define LED_PIN 25

// Variáveis de controle
unsigned long lastPublish = 0;
unsigned long lastWifiCheck = 0;
int wifiAttempts = 0;

void callback(char* topic, byte* payload, unsigned int length) {
  String msg;
  for (unsigned int i = 0; i < length; i++) {
    msg += (char)payload[i];
  }

  Serial.print("Mensagem recebida [");
  Serial.print(topic);
  Serial.print("]: ");
  Serial.println(msg);

  if (String(topic) == subscribe_topic) {
    msg.trim();
    if (msg == "ON" || msg == "1") {
      digitalWrite(LED_PIN, HIGH);
    } else if (msg == "OFF" || msg == "0") {
      digitalWrite(LED_PIN, LOW);
    }
  }
}

void reconnect() {
  // Tenta conectar apenas 3 vezes
  int attempts = 0;
  while (!client.connected() && attempts < 3) {
    Serial.print("🔌 Conectando ao MQTT...");
    String clientId = "esp32-wokwi-" + String(random(0xffff), HEX);

    if (client.connect(clientId.c_str())) {
      Serial.println(" ✅ conectado!");
      client.subscribe(subscribe_topic);
      Serial.print("📡 Inscrito no tópico: ");
      Serial.println(subscribe_topic);
      return;
    } else {
      Serial.print(" ❌ falhou, rc=");
      Serial.print(client.state());
      Serial.println(" tentando novamente...");
      attempts++;
      delay(2000);
    }
  }
  
  if (!client.connected()) {
    Serial.println("⚠️  Não foi possível conectar ao MQTT, continuando offline...");
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);


  // Configura LED
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  // Configura pino analógico do potenciômetro
  pinMode(POT_PIN, INPUT);

  // Inicializa DHT22
  dht.begin();

  // Conecta WiFi com timeout
  Serial.println(" Iniciando conexão WiFi...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  int wifi_timeout = 0;
  while (WiFi.status() != WL_CONNECTED && wifi_timeout < 30) {
    delay(500);
    Serial.print(".");
    wifi_timeout++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n WiFi conectado");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
    
    // Configura MQTT
    client.setServer(mqtt_server, mqtt_port);
    client.setCallback(callback);
    Serial.println("MQTT configurado");
    
    // Tenta conectar ao MQTT
    reconnect();
  } else {
    Serial.println("\nWiFi não conectou");
  }

}

void loop() {
  unsigned long now = millis();
  
  // Verifica WiFi periodicamente (a cada 10s)
  if (now - lastWifiCheck > 10000) {
    lastWifiCheck = now;
    
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("WiFi desconectado, tentando reconectar...");
      WiFi.reconnect();
    }
  }
  
  // Só tenta manter MQTT se WiFi estiver conectado
  if (WiFi.status() == WL_CONNECTED) {
    if (!client.connected()) {
      reconnect();
    }
    client.loop();
  }

  // Publica dados a cada 5 segundos
  if (now - lastPublish > 1000) {
    unsigned long t_start_total = millis();

    // Lê DHT22
    unsigned long t_dht_start = millis();
    float temperature = dht.readTemperature();
    unsigned long t_dht_end = millis();
    unsigned long t_dht_ms = t_dht_end - t_dht_start;

    // Lê potenciômetro (simulando umidade)
    unsigned long t_pot_start = millis();
    int potValue = analogRead(POT_PIN);
    float humidity = map(potValue, 0, 4095, 0, 100);
    unsigned long t_pot_end = millis();
    unsigned long t_pot_ms = t_pot_end - t_pot_start;

    // Publica via MQTT
    unsigned long t_pub_start = millis();
    bool success = false;
    
    if (!isnan(temperature)) {
      String payload = "{\"temperature\":";
      payload += String(temperature, 1);
      payload += ",\"humidity\":";
      payload += String(humidity, 1);
      payload += "}";

      if (client.connected()) {
        success = client.publish(publish_topic, payload.c_str(), true);
      }
      
      unsigned long t_pub_end = millis();
      unsigned long t_pub_ms = t_pub_end - t_pub_start;
      unsigned long t_total_ms = millis() - t_start_total;

      Serial.println("====== LOG DE TEMPO ======");
      Serial.printf("DHT22 leitura: %lu ms\n", t_dht_ms);
      Serial.printf("Potenciômetro leitura: %lu ms\n", t_pot_ms);
      
      if (client.connected()) {
        Serial.printf("Publicação MQTT: %lu ms (%s)\n", t_pub_ms, success ? "OK" : "❌ FALHA");
      } else {
        Serial.println("Publicação MQTT:  OFFLINE");
      }
      
      Serial.printf("Tempo total do ciclo: %lu ms\n", t_total_ms);
      Serial.println("-----------------------------");

      Serial.printf("Temperatura: %.1f °C | Umidade: %.1f%% (pot: %d)\n", 
                    temperature, humidity, potValue);
      
      if (success) {
        Serial.println("Dados enviados ao Node-RED!");
      }
    } else {
      Serial.println(" Falha ao ler temperatura do DHT22");
    }

    lastPublish = now;
    Serial.println();
  }
}