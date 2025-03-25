// Inclusão de bibliotecas
#include <WiFi.h>
#include <PubSubClient.h>
#include <ESPmDNS.h>
#include <Adafruit_BME280.h>
#include <Ultrasonic.h>

// Configuração de rede
#define NET_SSID "Redmi Note 12"
#define NET_PASSWORD "Aninha24"

// Configurações do MQTT
#define MQTT_ID "anabeggiato_esp32_inteli"                   // identificador do Cliente MQTT
#define MQTT_BROKER "broker.hivemq.com"                      // endereço do broker
#define MQTT_PORT 1883                                       // porta padrão MQTT
#define MQTT_TEMPERATURE_TOPIC "inteli_beggiana_temperature" // tópico MQTT para temperatura
#define MQTT_DISTANCE_TOPIC "inteli_beggiana_distance"       // tópico MQTT para distância
const int PUBLISH_FREQUENCY = 5000;                          // Define a frequência de publicação dos dados em milissegundos (neste caso, a cada 5 segundos)

WiFiClient espClient;
PubSubClient MQTT(espClient);

// Configuração do sensor de temperatura
#define BME280_ADDRESS 0x76
Adafruit_BME280 bme;

// Configuração do sensor ultrassônico
#define pino_trigger 2
#define pino_echo 4
Ultrasonic ultrasonic(pino_trigger, pino_echo);

// declaração de variáveis globais
unsigned long timer;
char temperatura[10] = "";
char distancia[10] = "";

// função para configuração da rede
void setupWifi()
{
    if (WiFi.status() == WL_CONNECTED) {return};

    // tentativa de conexão
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(NET_SSID);

    WiFi.begin(NET_SSID, NET_PASSWORD); // onde a conexão efetivamente acontece

    // printa pontos na tela para indicar a tentaiva de conexão
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    // Sucesso da conexão
    Serial.println("\nWiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
}

//função para conexão com o broker
void setupMQTT()
{
    MQTT.setServer(MQTT_BROKER, MQTT_PORT); //conecta com o broker e a porta indicados anteriormente

    // indica tentativa de conexão
    while (!MQTT.connected())
    {
        Serial.print("Tentando se conectar ao broker MQTT: ");
        Serial.println(MQTT_BROKER);
        if (MQTT.connect(MQTT_ID)) // Sucesso na conexão
        {
            Serial.println("Conectado com sucesso ao broker MQTT!");
        }
        else // Falha na conexão
        {
            Serial.println("Falha ao conectar no broker.");
            Serial.println("Haverá outra tentativa de conexão em 2s");
            delay(2000);
        }
    }
}

// função para medir a temperatura
float medirTemperatura()
{
    return bme.readTemperature();
}

// função para medir a distância
float medirDistancia()
{
    return ultrasonic.read();
}

void setup()
{
    Serial.begin(115200);

    setupWifi(); // inicia a conexão do wifi
    setupMQTT(); // inicia a conexão do broker

    // Inicializa o sensor BME280
    if (!bme.begin(BME280_ADDRESS))
    {
        Serial.println("Falha ao iniciar o sensor BME280!");
        while (1); // Para o programa se o sensor não for detectado
    }
    else
    {
        Serial.println("Sensor BME280 iniciado com sucesso!");
    }

    timer = millis();
}

void loop()
{
    if (!MQTT.connected())
    {
        setupMQTT(); // tenta reconectar com o broker caso ele seja desconectado
    }

    if ((millis() - timer) >= PUBLISH_FREQUENCY) // Verifica se o intervalo de tempo definido para publicação foi alcançado
    {
        // Lê a temperatura e a distancia nos sensores
        float temp = medirTemperatura();
        float dist = ultrasonic.read();

        sprintf(temperatura, "%.2f", temp);                // Formata a temperatura com duas casas decimais e econverte para string
        MQTT.publish(MQTT_TEMPERATURE_TOPIC, temperatura); // Publicação no tópico MQTT

        sprintf(distancia, "%.2f", dist);             // Formata a distancia com duas casas decimais e econverte para string
        MQTT.publish(MQTT_DISTANCE_TOPIC, distancia); // Publicação no tópico MQTT
    }
}
