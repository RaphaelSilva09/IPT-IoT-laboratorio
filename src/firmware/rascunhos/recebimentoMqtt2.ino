// tópicos juntos

#include <WiFi.h>
#include <PubSubClient.h>

// Configuração da rede Wi-Fi
#define WIFI_SSID "VIVOFIBRA-54B4"       // Nome da sua rede Wi-Fi
#define WIFI_PASSWORD "VvQfyJD2AL" // Senha da sua rede Wi-Fi

// Configurações do Broker MQTT
#define MQTT_BROKER "broker.hivemq.com" // Endereço do HiveMQ
#define MQTT_PORT 1883                 // Porta padrão MQTT
#define MQTT_CLIENT_ID "larissamartins_inteli_2" // Identificador único do cliente MQTT
#define TOPIC_SUBSCRIBE "larissamartins_inteli_2/teste"    // Tópico para receber mensagens

WiFiClient espClient;
PubSubClient mqttClient(espClient);

// Variáveis para armazenar os atributos
bool isAcceleration = false;
bool isDistance = false;
long interval = 2000;
float maxTemperature = 28.0;
float maxDistance = 50.0;

// Declaração antecipada da função setAtributes
void setAtributes(bool medirPelaAceleracao, bool medirPelaDistancia, long intervalo, float temperaturaMaxima, float distanciaMaxima);

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
  Serial.print("Mensagem recebida no tópico: ");
  Serial.println(topic);

  // Converte o payload para string para facilitar a manipulação
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  Serial.print("Mensagem: ");
  Serial.println(message);

  // Processa a mensagem para atualizar os atributos
  // Espera-se que a mensagem tenha o formato: "isAcceleration:true,isDistance:true,interval:5000,maxTemperature:30.0,maxDistance:60.0"
  if (message.indexOf("isAcceleration") != -1) {
    isAcceleration = message.substring(message.indexOf("isAcceleration:") + 15, message.indexOf(",")).equals("true");
  }
  if (message.indexOf("isDistance") != -1) {
    isDistance = message.substring(message.indexOf("isDistance:") + 11, message.indexOf(",", message.indexOf("isDistance"))).equals("true");
  }
  if (message.indexOf("interval") != -1) {
    interval = message.substring(message.indexOf("interval:") + 9, message.indexOf(",", message.indexOf("interval"))).toInt();
  }
  if (message.indexOf("maxTemperature") != -1) {
    maxTemperature = message.substring(message.indexOf("maxTemperature:") + 15, message.indexOf(",", message.indexOf("maxTemperature"))).toFloat();
  }
  if (message.indexOf("maxDistance") != -1) {
    maxDistance = message.substring(message.indexOf("maxDistance:") + 12).toFloat();
  }

  // Chama a função setAtributes para atualizar os valores
  setAtributes(isAcceleration, isDistance, interval, maxTemperature, maxDistance);
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
      mqttClient.subscribe(TOPIC_SUBSCRIBE); // Inscreve-se no tópico
      Serial.print("Inscrito no tópico: ");
      Serial.println(TOPIC_SUBSCRIBE);
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
