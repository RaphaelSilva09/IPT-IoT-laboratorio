// tópicos separados

#include <WiFi.h>
#include <PubSubClient.h>

// Configuração da rede Wi-Fi
#define WIFI_SSID "VIVOFIBRA-54B4"       // Substitua pelo nome da sua rede Wi-Fi
#define WIFI_PASSWORD "VvQfyJD2AL" // Substitua pela senha da sua rede Wi-Fi

// Configurações do Broker MQTT
#define MQTT_BROKER "broker.hivemq.com" // Endereço do HiveMQ
#define MQTT_PORT 1883                 // Porta padrão MQTT
#define MQTT_CLIENT_ID "larissamartins_inteli_1" // Identificador único do cliente MQTT

// Tópicos para receber mensagens
#define TOPIC_ACCELERATION "larissamartins_inteli_1/isAcceleration"
#define TOPIC_DISTANCE "larissamartins_inteli_1/isDistance"
#define TOPIC_INTERVAL "larissamartins_inteli_1/interval"
#define TOPIC_MAX_TEMPERATURE "larissamartins_inteli_1/maxTemperature"
#define TOPIC_MAX_DISTANCE "larissamartins_inteli_1/maxDistance"

WiFiClient espClient;
PubSubClient mqttClient(espClient);

// Variáveis para armazenar os atributos
bool medirPelaAceleracao = false;
bool medirPelaDistancia = false;
long intervalo = 2000;
float temperaturaMaxima = 28.0;
float distanciaMaxima = 50.0;

// Função para conectar ao Wi-Fi
void setupWiFi() {
  Serial.print("Conectando ao Wi-Fi");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(3000);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi conectado!");
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());
}

// Callback para processar mensagens recebidas
void callback(char* topic, byte* payload, unsigned int length) {
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  Serial.print("Mensagem recebida no tópico: ");
  Serial.println(topic);
  Serial.print("Mensagem: ");
  Serial.println(message);

  // Processa as mensagens de cada tópico e armazena nas variáveis
  if (String(topic) == TOPIC_ACCELERATION) {
    medirPelaAceleracao = (message == "true");
  } else if (String(topic) == TOPIC_DISTANCE) {
    medirPelaDistancia = (message == "true");
  } else if (String(topic) == TOPIC_INTERVAL) {
    intervalo = message.toInt();
  } else if (String(topic) == TOPIC_MAX_TEMPERATURE) {
    temperaturaMaxima = message.toFloat();
  } else if (String(topic) == TOPIC_MAX_DISTANCE) {
    distanciaMaxima = message.toFloat();
  }

  // Chama a função setAtributes para atribuir os valores às variáveis finais
  setAtributes(medirPelaAceleracao, medirPelaDistancia, intervalo, temperaturaMaxima, distanciaMaxima);
}

// Função para configurar a conexão ao broker MQTT
void setupMQTT() {
  mqttClient.setServer(MQTT_BROKER, MQTT_PORT);
  mqttClient.setCallback(callback);

  // Tenta conectar ao broker MQTT
  while (!mqttClient.connected()) {
    Serial.print("Conectando ao broker MQTT...");
    if (mqttClient.connect(MQTT_CLIENT_ID)) {
      Serial.println("Conectado!");

      // Inscreve-se nos tópicos separados
      mqttClient.subscribe(TOPIC_ACCELERATION);
      mqttClient.subscribe(TOPIC_DISTANCE);
      mqttClient.subscribe(TOPIC_INTERVAL);
      mqttClient.subscribe(TOPIC_MAX_TEMPERATURE);
      mqttClient.subscribe(TOPIC_MAX_DISTANCE);

      Serial.println("Inscrito nos tópicos:");
      Serial.println(TOPIC_ACCELERATION);
      Serial.println(TOPIC_DISTANCE);
      Serial.println(TOPIC_INTERVAL);
      Serial.println(TOPIC_MAX_TEMPERATURE);
      Serial.println(TOPIC_MAX_DISTANCE);
    } else {
      Serial.print("Falha na conexão. Código de erro: ");
      Serial.println(mqttClient.state());  // Imprime o código de erro
      delay(2000); // Tenta novamente a cada 2 segundos
    }
  }
}

// Função para garantir que a conexão Wi-Fi seja mantida
void checkWiFi() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Wi-Fi perdido, tentando reconectar...");
    WiFi.reconnect(); // Reconecta ao Wi-Fi
  }
}

// Função para garantir que a conexão MQTT seja mantida
void checkMQTT() {
  if (!mqttClient.connected()) {
    Serial.println("MQTT desconectado! Tentando reconectar...");
    setupMQTT(); // Reestabelece a conexão MQTT
  }
}

// Função para atualizar os atributos com os valores recebidos
void setAtributes(bool medirPelaAceleracao, bool medirPelaDistancia, long intervalo, float temperaturaMaxima, float distanciaMaxima) {
  // Atualiza as variáveis com os valores recebidos
  bool isAcceleration = medirPelaAceleracao;
  bool isDistance = medirPelaDistancia;
  long interval = intervalo;
  float maxTemperature = temperaturaMaxima;
  float maxDistance = distanciaMaxima;

  // Exibe os valores atualizados
  Serial.println("Atributos atualizados:");
  Serial.print("Aceleração: ");
  Serial.println(isAcceleration);
  Serial.print("Distância: ");
  Serial.println(isDistance);
  Serial.print("Intervalo: ");
  Serial.println(interval);
  Serial.print("Temperatura máxima: ");
  Serial.println(maxTemperature);
  Serial.print("Distância máxima: ");
  Serial.println(maxDistance);
}

void setup() {
  Serial.begin(115200);
  setupWiFi(); // Conecta ao Wi-Fi
  setupMQTT(); // Conecta ao broker MQTT
}

void loop() {
  checkWiFi();   // Verifica e mantém a conexão Wi-Fi
  checkMQTT();   // Verifica e mantém a conexão MQTT
  mqttClient.loop(); // Processa mensagens MQTT
}
