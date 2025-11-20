# DropSense – Monitorador Inteligente de Umidade com ESP32

Sistema IoT para monitoramento e controle automático da umidade em ambientes internos, utilizando ESP32/ESP8266, sensor DHT22 e um mini umidificador ultrassônico acionado via MOSFET. O objetivo do projeto é proporcionar ambientes mais saudáveis ao detectar níveis baixos de umidade e acionar automaticamente um umidificador.

<img width="2845" height="1637" alt="a" src="https://github.com/user-attachments/assets/d7ed33d6-b585-4974-bebe-faf9240e9829" />


---

## Visão Geral

O **DropSense** é um protótipo de baixo custo, escalável e simples de implementar, projetado para:

- Monitorar em tempo real a umidade e temperatura
- Enviar leituras via MQTT
- Exibir dados em um painel Node-RED Dashboard
- Acionar automaticamente o umidificador quando a umidade estiver abaixo de um limite configurado
- Permitir automação residencial e integração com sistemas IoT

---

## Componentes Utilizados

| Componente | Função |
|------------|--------|
| **ESP32 / ESP8266 NodeMCU** | Microcontrolador principal, responsável pela lógica e conectividade Wi-Fi |
| **Sensor DHT22** | Mede temperatura e umidade relativa do ar |
| **Mini umidificador ultrassônico 5V** | Atuador que gera névoa para aumentar a umidade |
| **MOSFET N-Channel** | Chave eletrônica para controlar o umidificador |
| **Resistor 220Ω** | Proteção e limitação de corrente do gate do MOSFET |
| **Protoboard** | Montagem do protótipo sem solda |
| **Jumpers** | Conexões entre os componentes |

---

## Diagrama de Funcionamento

1. O sensor **DHT22** envia leituras periódicas ao ESP32
2. O ESP32 compara o valor de umidade com o limite definido
3. Caso esteja abaixo do mínimo:
   - Ativa o gate do MOSFET
   - O MOSFET permite corrente ao mini umidificador, ligando-o
4. Quando a umidade retorna ao nível ideal:
   - O ESP32 desativa o gate do MOSFET
   - O umidificador é desligado
5. Todos os dados são publicados via MQTT
6. O Node-RED recebe, trata e exibe os dados em um dashboard

---

## Fluxo IoT / MQTT

O ESP32 publica os dados em tópicos como:

```
/dropsense/umidade
/dropsense/temperatura
/dropsense/status
```

O Node-RED interpreta esses dados e exibe indicadores visuais.

---

## Dashboard (Node-RED)

O painel apresenta:

- ✔ Gauge de umidade
- ✔ Indicador de temperatura
- ✔ Estado do umidificador ("Ligado/Desligado")
- ✔ Histórico de leituras

---

## Simulação em Wokwi

O projeto também foi testado no simulador **Wokwi**, permitindo testar:

- Sensor DHT22
- Lógica de acionamento
- Publicação MQTT

---

## Tecnologias e Ferramentas

- **Arduino IDE**
- **C/C++**
- **Biblioteca DHT**
- **MQTT**
- **Mosquitto** (broker)
- **Node-RED + Dashboard**
- **Wokwi** (simulação)
- **Fritzing** (montagem do protótipo)

---

## Resultados Obtidos

- O sistema detecta rapidamente variações de umidade
- Aciona o umidificador de forma confiável ao atingir o limite programado
- Tempo de resposta dos sensores e comunicação MQTT é baixo (ms)
- Dashboard apresenta leituras em tempo real e status do sistema
- Comportamento validado tanto fisicamente quanto via simulação

---

## Conclusão

O **DropSense** demonstrou ser:

- Eficiente para monitoramento ambiental
- De baixo custo e fácil replicação
- Escalável para aplicações maiores
- Ótimo para projetos educacionais, residenciais e laboratoriais

O sistema destaca o potencial da IoT na automação residencial, integrando hardware, software e comunicação de forma prática e funcional.

---

**Universidade Presbiteriana Mackenzie – ADS**

---

## Licença

Este projeto é de código aberto e está disponível sob a licença MIT.

---
