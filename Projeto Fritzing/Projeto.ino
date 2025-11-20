#include <DHT.h>

// ===================================
// CONFIGURAÇÕES DO PROJETO
// ===================================

// Definições de pinos
#define DHTPIN 15          // Pino do sensor DHT22
#define DHTTYPE DHT22      // Tipo do sensor
#define MOSFET_PIN 25      // Pino de controle do MOSFET (ou Relé)

// Configurações do sistema
#define UMIDADE_MIN 60.0   // Umidade mínima desejada (%) - AJUSTE AQUI!
#define TEMPO_LEITURA 2000 // Intervalo entre leituras (ms)

// Inicializa o sensor DHT
DHT dht(DHTPIN, DHTTYPE);

// Variáveis globais
bool umidificadorLigado = false;

void setup() {
  // Inicializa comunicação serial
  Serial.begin(115200);
  Serial.println("=== Sistema de Controle de Umidade ===");
  Serial.println("Inicializando...\n");
  
  // Configura pino do MOSFET como saída
  pinMode(MOSFET_PIN, OUTPUT);
  digitalWrite(MOSFET_PIN, LOW);
  
  // Inicializa sensor DHT22
  dht.begin();
  
  delay(2000); // Aguarda estabilização do sensor
  Serial.println("Sistema pronto!\n");
}

void loop() {
  // Lê os dados do sensor
  float umidade = dht.readHumidity();
  float temperatura = dht.readTemperature();
  
  // Verifica se a leitura foi bem-sucedida
  if (isnan(umidade) || isnan(temperatura)) {
    Serial.println("Erro ao ler o sensor DHT22!");
    delay(TEMPO_LEITURA);
    return;
  }
  
  // Exibe leituras no monitor serial
  Serial.println("------------------------");
  Serial.print("Temperatura: ");
  Serial.print(temperatura);
  Serial.println(" °C");
  
  Serial.print("Umidade: ");
  Serial.print(umidade);
  Serial.println(" %");
  
  // Lógica de controle do umidificador
  if (umidade < UMIDADE_MIN) {
    // Umidade baixa - LIGA o umidificador
    if (!umidificadorLigado) {
      digitalWrite(MOSFET_PIN, HIGH);
      umidificadorLigado = true;
      Serial.println(">> UMIDIFICADOR LIGADO <<");
      Serial.println("Umidade abaixo do limite!");
    } else {
      Serial.println("Status: Umidificador ativo");
    }
  } else {
    // Umidade adequada - DESLIGA o umidificador
    if (umidificadorLigado) {
      digitalWrite(MOSFET_PIN, LOW);
      umidificadorLigado = false;
      Serial.println(">> UMIDIFICADOR DESLIGADO <<");
      Serial.println("Umidade normalizada!");
    } else {
      Serial.println("Status: Umidade OK");
    }
  }
  
  Serial.println("------------------------\n");
  
  // Aguarda antes da próxima leitura
  delay(TEMPO_LEITURA);
}