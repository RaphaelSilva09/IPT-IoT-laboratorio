// Instância de Objetos
LiquidCrystal_I2C lcd(0x27, 16, 2);
Adafruit_BME280 bme;
Ultrasonic ultrasonic(2, 4);
WiFiClient client;
PubSubClient MQTT(client);

#include "UbidotsEsp32Mqtt.h"
#include <WiFi.h>

// Configurações do MQTT
const char *UBIDOTS_TOKEN = "BBUS-NCjHwn7QkENLHhbVWVqfVBLF4v8pqE";
const char *WIFI_SSID = "Inteli.Iot";
const char *WIFI_PASS = "@Intelix10T#";
const char *DEVICE_LABEL = "teste_prototipo";   
const char *CLIENT_ID = "IPTrack";

// Tópicos de envio
const char *MQTT_TEMPERATURE_TOPIC = "temperaturaMQTT";  // tópico MQTT para temperatura
const char *MQTT_DISTANCE_TOPIC = "distanciaMQTT";        // tópico MQTT para distância
const char *MQTT_VIBRATION_TOPIC = "vibracaoMQTT";      // tópico MQTT para vibração
const char *MQTT_USAGE_TIME_TOPIC = "tempoUso";      // tópico MQTT para intervalo
const char *MQTT_OIL_CHANGE_TOPIC = "tempoFuncionamentoVibracao";    // tópico MQTT para troca de óleo
const int PUBLISH_FREQUENCY = 5000;                           // Define a frequência de publicação dos dados em milissegundos (neste caso, a cada 5 segundos)

// Tópicos de recebimento
const char *TOPIC_ACCELERATION = "isacceleration"; // tópico MQTT para modo de medição de atividade por aceleração
const char *TOPIC_DISTANCE = "isdistance"; // tópico MQTT para modo de medição de atividade por distancia
const char *TOPIC_INTERVAL = "interval"; // tópico MQTT para receber intervalo de medições
const char *TOPIC_MAX_TEMPERATURE = "maxtemperature"; // tópico MQTT para receber temperatura máxima
const char *TOPIC_MAX_VIBRATION = "maxvibration"; // tópico MQTT para receber vibração máxima

Ubidots ubidots(UBIDOTS_TOKEN, CLIENT_ID);

// Configuração do FFT
#define SAMPLES 128
#define SAMPLE_FREQ 8000

// declaração de variáveis globais
unsigned long timer;
char vibracaoMQTT[10] = "";
char temperaturaMQTT[10] = "";
char distanciaMQTT[10] = "";
char aceleracaoXMQTT[10] = "";
char tempoUso[10] = "";
char tempoFuncionamentoVibracao[10]; 

class Sistema {
public:
  // atributos
  static Sistema* instance;
  unsigned long timeOn = 0;        // Tempo acumulado
  unsigned long inicioLigado = 0;  // Momento em que a máquina foi ligada
  bool estavaLigado = false;       // Estado anterior da máquina
  bool ligado = false;
  const unsigned long iolInterval = 144000000;  // 40 horas em milissegundos, para saber se precisa trocar o óleo
  long interval = 5000;                                // intervalo entre medições inicial
  float tempoEstavel;
  bool systemStatus;       // estado do dispositivo
  bool isacceleration = true;     // booleano para monitorar pela aceleração
  bool isdistance;         // booleano para monitorar pela distância
  float temperature;       // temperatura do sistema
  float maxtemperature = 28.0;  // temperatura máxima do sistema inicial
  float distance;          // distância do sistema
  float previousDistance;  // distância anterior
  float accelerationX;     // aceleração no eixo X do sistema
  float accelerationY;     // aceleração no eixo Y do sistema
  float accelerationZ;     // aceleração no eixo Z do sistema
  uint8_t SDApin;          // Pino SDA
  uint8_t SCLpin;          // Pino SCL
  uint8_t buzzer;          // Pino do buzzer
  uint8_t ledOn;           // Pino do led sistema
  uint8_t ledTemp;         // Pino do led alerta
  uint8_t botao;           // Pino do botão
  uint8_t botao2;          // Pino do botão
  uint8_t buttonDebounce;  // Debounce do botão
  unsigned long mqttReconnectTimer;
  unsigned long _previousMillis;
  static const unsigned long MQTT_TIMEOUT = 5000;          // 5 segundos de timeout
  static const unsigned long MQTT_RETRY_INTERVAL = 10000;  // 10 segundos entre tentativas
  bool mqttInitialized;
  int16_t accelData[SAMPLES];    // Array para dados do acelerômetro
  int16_t sampleIndex;           // Índice atual da amostra
  unsigned long lastSampleTime;  // Tempo da última amostra
  float vibrationIntensity;      // Intensidade da vibração
  float maxvibrationIntensity;   // Intesidade máxima da vibração
  bool isFFTReady;               // Flag para indicar quando FFT está pronta
  bool estadoBotao2;

  bool medirPelaAceleracao = false;
  bool medirPelaDistancia = false;
  long intervalo = 2000;
  float temperaturaMaxima = 38;
  float vibracaoMaxima = 0.5;

  // Métodos públicos
  Sistema(uint8_t debounceDoBotao = 200, bool statusDoSistema = true, float distanciaAnterior = 0.0,
          uint8_t pinoSDA = 21, uint8_t pinoSCL = 22, uint8_t pinoBuzzer = 18,
          uint8_t pinoLedOn = 13, uint8_t pinoLedTemp = 12, uint8_t pinoBotao = 33, uint8_t pinoBotao2 = 14) {
    // Inicialização dos pinos e atributos usados
    SDApin = pinoSDA;
    SCLpin = pinoSCL;
    buzzer = pinoBuzzer;
    ledOn = pinoLedOn;
    ledTemp = pinoLedTemp;
    botao = pinoBotao;
    botao2 = pinoBotao2;
    previousDistance = distanciaAnterior;
    systemStatus = statusDoSistema;
    buttonDebounce = debounceDoBotao;
    mqttReconnectTimer = 0;
    mqttInitialized = false;
    sampleIndex = 0;
    lastSampleTime = 0;
    vibrationIntensity = 0;
    isFFTReady = false;
    
    pinMode(botao2, INPUT_PULLUP);
  }

  void setAtributes(bool medirPelaAceleracao = false, bool medirPelaDistancia = false, long intervalo = 2000, float temperaturaMaxima = 28.0, float vibracaoMaxima = 0.5) {
    isacceleration = medirPelaAceleracao;
    isdistance = medirPelaDistancia;
    interval = intervalo;
    maxtemperature = temperaturaMaxima;
    maxvibrationIntensity = vibracaoMaxima / 100;

    // Exibe os valores atualizados
    Serial.println("Atributos atualizados:");
    Serial.print("Aceleração: ");
    Serial.println(isacceleration);
    Serial.print("Distância: ");
    Serial.println(isdistance);
    Serial.print("Intervalo: ");
    Serial.println(interval);
    Serial.print("Temperatura máxima: ");
    Serial.println(maxtemperature);
    Serial.print("Vibração máxima: ");
    Serial.println(maxvibrationIntensity);
  }

  void alarmOn() {
    tone(buzzer, 800);
    digitalWrite(ledTemp, HIGH);
  }

  void alarmOff() {
    Serial.print("Rodando alarme off");
    noTone(buzzer);
    digitalWrite(ledTemp, LOW);
  }

  void systemOff() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Desligado.");
    Serial.println("Dispositivo desligado!");
    alarmOff();
  }

  void estadoDoBotao () {
    if(digitalRead(botao2) == LOW) {
        estadoBotao2 = 1;
      }
  }

  void printTemperature() {
    lcd.clear();
    lcd.setCursor(0, 0);
    temperature = bme.readTemperature();
      if (temperature > maxtemperature)  {
        estadoDoBotao();

        if(estadoBotao2 == 1) {
        alarmOff();

        lcd.print("Estabilizando....");
      } else {
        estadoBotao2 = 0;
        
        lcd.print("ALERTA: Temp Alta");
        Serial.println("ALERTA: Temp Alta");
        alarmOn();
      }
    } else {
      Serial.print("Temperatura: ");
      Serial.print(temperature);
      Serial.println(" °C");
      lcd.print("Temp Normal: ");
      lcd.setCursor(0, 1);
      lcd.print(temperature);
      lcd.setCursor(5, 1);
      lcd.print(" Celsius");
      alarmOff();
    }
  }

  void printDistance() {
    distance = ultrasonic.read();  // Leitura da distância
    Serial.println(distance);
    float variacaoDistancia = abs(distance - previousDistance);
    // Verifica se a variação da distância está dentro do limite definido
    if (variacaoDistancia == previousDistance) {
      tempoEstavel += interval;   // Incrementa o tempo estável
      if (tempoEstavel > 2000) {  // Se o tempo de estabilidade exceder 10 segundos
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Sistema");
        lcd.setCursor(0, 1);
        lcd.print("Desligado");
        Serial.println("Sistema Desligado");
      }
    } else {
      // Reseta o tempo de estabilidade se houver variação na distância
      tempoEstavel = 0;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Sistema Ligado");
      lcd.setCursor(0, 1);
      Serial.println("Sistema Ligado");
    }
    // Atualiza a variável previousDistance para a próxima leitura
    previousDistance = distance;
  }

  void printVibration() {
    collectAccelSample();

    if (isFFTReady) {
      lcd.clear();
      lcd.setCursor(0, 0);

      if (vibrationIntensity > maxvibrationIntensity) {
        lcd.print("Sistema:");
        lcd.setCursor(0, 1);
        lcd.print("Operante");
        Serial.println("Dispositivo em funcionamento");
        Serial.print("Intensidade: ");
        Serial.println(vibrationIntensity);
        ligado = true;
      } else {
        lcd.print("Sistema:");
        lcd.setCursor(0, 1);
        lcd.print("Inoperante");
        ligado = false;
      }

      isFFTReady = false;
    }
  }

  void MachineOperationTime() {
        // Atualiza o tempo enquanto a máquina está ligada
    if (ligado) {
        if (!estavaLigado) {
            // Máquina acabou de ser ligada
            inicioLigado = millis();
            estavaLigado = true;
            Serial.println("Máquina ligada.");
        } else {
            // Máquina continua ligada, atualiza o tempo
            timeOn = millis() - inicioLigado; // Calcula o tempo atual de operação
            sprintf(tempoFuncionamentoVibracao, "%lu", timeOn);
            ubidots.add(MQTT_OIL_CHANGE_TOPIC, atof(tempoFuncionamentoVibracao));
            ubidots.publish(DEVICE_LABEL);
        }
    } else if (estavaLigado) {
            // Máquina acabou de ser desligada
            estavaLigado = false;
            Serial.println("Máquina desligada.");
    }

    if (timeOn >= iolInterval) {
      Serial.println("Trocar o óleo!");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Troca de oleo!");
      timeOn = 0;
      ubidots.add(MQTT_OIL_CHANGE_TOPIC, 1);
      ubidots.publish(DEVICE_LABEL);
    }
    Serial.print("Tempo acumulado: ");
    Serial.println(timeOn / 1000);
  }
  

  void setupWifi() {
    if (WiFi.status() == WL_CONNECTED)
      return;

    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(WIFI_SSID);

    WiFi.begin(WIFI_SSID, WIFI_PASS);

    unsigned long startAttemptTime = millis();

    while (WiFi.status() != WL_CONNECTED) {
      if (millis() - startAttemptTime > 20000) {
        Serial.println("Failed to connect to WiFi. Restarting...");
        ESP.restart();
        return;
      }
      delay(500);
      Serial.print(".");
    }

    Serial.println("\nWiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }

  static void staticCallback(char* topic, uint8_t* payload, unsigned int length) {
    if (instance) {
      instance->callback(topic, payload, length);
    }
  }

  // Função callback personalizada para o MQTT
  void callback(char* topic, byte* payload, unsigned int length) {
    // Converter o payload recebido para string
    String message = "";
    for (int i = 0; i < length; i++) {
      message += (char)payload[i];
    }

    Serial.print("Mensagem recebida no tópico: ");
    Serial.println(topic);
    Serial.print("Mensagem: ");
    Serial.println(message);

    // Processa as mensagens de cada tópico e atualiza as variáveis
    if (String(topic) == "/v2.0/devices/teste_prototipo/isacceleration/lv") {
      medirPelaAceleracao = message.toInt();
    } else if (String(topic) == "/v2.0/devices/teste_prototipo/isdistance/lv") {
      medirPelaDistancia = message.toInt();
    } else if (String(topic) == "/v2.0/devices/teste_prototipo/interval/lv") {
      intervalo = message.toInt();
    } else if (String(topic) == "/v2.0/devices/teste_prototipo/maxtemperature/lv") {
      temperaturaMaxima = message.toInt();
    } else if (String(topic) == "/v2.0/devices/teste_prototipo/maxvibration/lv") {
      vibracaoMaxima = message.toInt();
    }
      
  // atualiza as variáveis com as mensagens recebidas
  setAtributes(medirPelaAceleracao, medirPelaDistancia, intervalo, temperaturaMaxima, vibracaoMaxima);
  }

  bool setupMQTT() {
    if (!WiFi.isConnected()) {
      Serial.println("WiFi not connected. Cannot setup ubidots.");
      return false;
    }

    unsigned long startAttempt = millis();
    while (!ubidots.connected()) {
      ubidots.connectToWifi(WIFI_SSID, WIFI_PASS);
      ubidots.setup();
      ubidots.reconnect();

      if (ubidots.connect()) {
        Serial.println("Conectado com sucesso ao broker MQTT!");
        mqttInitialized = true;
        instance = this;
        ubidots.setCallback(staticCallback);

        // Inscreve-se nos tópicos separados
        ubidots.subscribeLastValue(DEVICE_LABEL, TOPIC_ACCELERATION);
        ubidots.subscribeLastValue(DEVICE_LABEL, TOPIC_DISTANCE);
        ubidots.subscribeLastValue(DEVICE_LABEL, TOPIC_INTERVAL);
        ubidots.subscribeLastValue(DEVICE_LABEL, TOPIC_MAX_TEMPERATURE);
        ubidots.subscribeLastValue(DEVICE_LABEL, TOPIC_MAX_VIBRATION);

        Serial.println("Inscrito nos tópicos:");
        Serial.println(TOPIC_ACCELERATION);
        Serial.println(TOPIC_DISTANCE);
        Serial.println(TOPIC_INTERVAL);
        Serial.println(TOPIC_MAX_TEMPERATURE);
        Serial.println(TOPIC_MAX_VIBRATION);

        return true;
      }

      delay(2000);
    }

    Serial.println("Falha ao conectar no broker.");
    return false;
  }

  bool validateConnection() {
    if (!mqttInitialized) {
      return false;
    }
    if (!ubidots.connected()) {
      if (millis() - mqttReconnectTimer < MQTT_RETRY_INTERVAL) {
        return false;
      }

      mqttReconnectTimer = millis();
      return setupMQTT();
    }

    return true;
  }

  bool publishTemperature() {
    if (!validateConnection()) {
      Serial.println("MQTT não conectado. Não foi possível publicar temperatura.");
      return false;
    }

    dtostrf(temperature, 4, 2, temperaturaMQTT);
    ubidots.add(MQTT_TEMPERATURE_TOPIC, atof(temperaturaMQTT));
    bool success = ubidots.publish(DEVICE_LABEL);
    Serial.println("Temperatura enviada ao Broker com sucesso");
    ubidots.loop();
    return success;
  }

  bool publishDistance() {
    if (!validateConnection()) {
      Serial.println("MQTT não conectado. Não foi possível publicar distância.");
      return false;
    }

    dtostrf(distance, 4, 2, distanciaMQTT);
    bool success = ubidots.publish(DEVICE_LABEL);
    ubidots.add(MQTT_DISTANCE_TOPIC, atof(distanciaMQTT));
    Serial.println("Distancia enviada ao Broker com sucesso");
    ubidots.loop();
    return success;
  }

  void performFFT() {
    ZeroFFT(accelData, SAMPLES);

    vibrationIntensity = 0;
    for (int i = 1; i < SAMPLES / 2; i++) {
      float frequency = FFT_BIN(i, SAMPLE_FREQ, SAMPLES);
      float magnitude = accelData[i] / 32768.0f;
      vibrationIntensity += magnitude * (frequency / SAMPLE_FREQ);
    }

    isFFTReady = true;
  }

  void collectAccelSample() {
    unsigned long currentTime = millis();

    if (currentTime - lastSampleTime >= (1000000 / SAMPLE_FREQ)) {
      Wire.beginTransmission(0x1C);
      Wire.write(0x02);
      Wire.endTransmission(false);
      Wire.requestFrom(0x1C, 6);

      if (Wire.available() == 6) {
        int16_t ax = (Wire.read() << 8 | Wire.read()) >> 2;
        int16_t ay = (Wire.read() << 8 | Wire.read()) >> 2;
        int16_t az = (Wire.read() << 8 | Wire.read()) >> 2;

        float magnitude = sqrt(
          pow(ax / 4096.0, 2) + pow(ay / 4096.0, 2) + pow(az / 4096.0, 2));

        accelData[sampleIndex] = (int16_t)(magnitude * 32768.0f);

        sampleIndex++;
        lastSampleTime = currentTime;

        if (sampleIndex >= SAMPLES) {
          performFFT();
          sampleIndex = 0;
        }
      }
    }
  }

  bool publishVibration() {
    if (!validateConnection()) {
      Serial.println("MQTT não conectado. Não foi possível publicar vibração.");
      return false;
    }

    dtostrf(vibrationIntensity, 4, 2, vibracaoMQTT);
    ubidots.add(MQTT_VIBRATION_TOPIC, atof(vibracaoMQTT));
    bool success = ubidots.publish(DEVICE_LABEL);
    Serial.println("Vibração enviada ao Broker com sucesso");
    ubidots.loop();
    return success;
  }

  bool publishUsageTime() {
    if (!validateConnection()) {
      Serial.println("MQTT nao conectado. Nao foi possivel publicar tempo de uso.");
      return false;
    }
    
    dtostrf(timeOn / 1000, 4, 2, tempoUso);
    ubidots.add(MQTT_USAGE_TIME_TOPIC, atof(tempoUso));
    bool success = ubidots.publish(DEVICE_LABEL);
    Serial.println("Tempo de uso enviado ao Broker com sucesso");
    ubidots.loop();
    return success;
  }

  void reconnect() {
    // Loop até reconectar
    ubidots.setCallback(staticCallback);
    if (!ubidots.connected()) {
      Serial.println("Tentando reconectar!");
      
    ubidots.reconnect();
    // Inscreve-se nos tópicos separados
        ubidots.subscribeLastValue(DEVICE_LABEL, TOPIC_ACCELERATION);
        ubidots.subscribeLastValue(DEVICE_LABEL, TOPIC_DISTANCE);
        ubidots.subscribeLastValue(DEVICE_LABEL, TOPIC_INTERVAL);
        ubidots.subscribeLastValue(DEVICE_LABEL, TOPIC_MAX_TEMPERATURE);
        ubidots.subscribeLastValue(DEVICE_LABEL, TOPIC_MAX_VIBRATION);

        Serial.println("Inscrito nos tópicos:");
        Serial.println(TOPIC_ACCELERATION);
        Serial.println(TOPIC_DISTANCE);
        Serial.println(TOPIC_INTERVAL);
        Serial.println(TOPIC_MAX_TEMPERATURE);
        Serial.println(TOPIC_MAX_VIBRATION);
  }
  ubidots.loop();
  }
};