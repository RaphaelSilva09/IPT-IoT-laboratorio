// Instância de Objetos
LiquidCrystal_I2C lcd(0x27, 16, 2);
Adafruit_BME280 bme;
Ultrasonic ultrasonic(2, 4);
WiFiClient client;
PubSubClient MQTT(client);

// Configuração de rede
#define NET_SSID "SHARE-RESIDENTE"
#define NET_PASSWORD "Share@residente23"

// Configurações do MQTT
#define MQTT_ID "anabeggiato_esp32_inteli"  // identificador do Cliente MQTT
#define MQTT_BROKER "broker.hivemq.com"     // endereço do broker
#define MQTT_PORT 1883                      // porta padrão MQTT

// Tópicos de envio
#define MQTT_TEMPERATURE_TOPIC "inteli_beggiana_temperature"  // tópico MQTT para temperatura
#define MQTT_DISTANCE_TOPIC "inteli_beggiana_distance"        // tópico MQTT para distância
#define MQTT_VIBRATION_TOPIC "inteli_beggiana_vibration"      // tópico MQTT para vibração
#define MQTT_USAGE_TIME_TOPIC "inteli_beggiana_interval"      // tópico MQTT para intervalo
#define MQTT_OIL_CHANGE_TOPIC "inteli_beggiana_oil_change"    // tópico MQTT para troca de óleo
const int PUBLISH_FREQUENCY = 5000;                           // Define a frequência de publicação dos dados em milissegundos (neste caso, a cada 5 segundos)

// Tópicos de recebimento
#define TOPIC_ACCELERATION "larissamartins_inteli_1/isAcceleration"     // tópico MQTT para modo de medição de atividade por aceleração
#define TOPIC_DISTANCE "larissamartins_inteli_1/isDistance"             // tópico MQTT para modo de medição de atividade por distancia
#define TOPIC_INTERVAL "larissamartins_inteli_1/interval"               // tópico MQTT para receber intervalo de medições
#define TOPIC_MAX_TEMPERATURE "larissamartins_inteli_1/maxTemperature"  // tópico MQTT para receber temperatura máxima
#define TOPIC_MAX_VIBRATION "larissamartins_inteli_1/maxVibration"      // tópico MQTT para receber vibração máxima

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
  unsigned long timeOn = 0;        // Tempo acumulado
  unsigned long inicioLigado = 0;  // Momento em que a máquina foi ligada
  bool estavaLigado = false;       // Estado anterior da máquina
  bool ligado = false;
  const unsigned long iolInterval = 144000000;  // 40 horas em milissegundos, para saber se precisa trocar o óleo
  long interval = 5000;                         // intervalo entre medições inicial
  float tempoEstavel;
  bool systemStatus;            // estado do dispositivo
  bool isAcceleration;          // booleano para monitorar pela aceleração
  bool isDistance;              // booleano para monitorar pela distância
  float temperature;            // temperatura do sistema
  float maxTemperature = 28.0;  // temperatura máxima do sistema inicial
  float distance;               // distância do sistema
  float previousDistance;       // distância anterior
  float accelerationX;          // aceleração no eixo X do sistema
  float accelerationY;          // aceleração no eixo Y do sistema
  float accelerationZ;          // aceleração no eixo Z do sistema
  uint8_t SDApin;               // Pino SDA
  uint8_t SCLpin;               // Pino SCL
  uint8_t buzzer;               // Pino do buzzer
  uint8_t ledOn;                // Pino do led sistema
  uint8_t ledTemp;              // Pino do led alerta
  uint8_t botao;                // Pino do botão
  uint8_t botao2;               // Pino do botão
  uint8_t buttonDebounce;       // Debounce do botão
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
          uint8_t pinoSDA = 21, uint8_t pinoSCL = 22, uint8_t pinoBuzzer = 26,
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

  void setAtributes(bool medirPelaAceleracao = false, bool medirPelaDistancia = false, long intervalo = 2000, float temperaturaMaxima = 28.0, float vibracaoMaxima = 0.01) {
    isAcceleration = medirPelaAceleracao;
    isDistance = medirPelaDistancia;
    interval = intervalo;
    maxTemperature = temperaturaMaxima;
    maxvibrationIntensity = vibracaoMaxima;

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
    Serial.println(maxvibrationIntensity);
  }

  void alarmOn() {
    Serial.print("Ligando alarme");

    tone(buzzer, 10000);
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

  void estadoDoBotao() {
    if (digitalRead(botao2) == LOW) {
      estadoBotao2 = 1;
    }
  }

  void printTemperature() {
    lcd.clear();
    lcd.setCursor(0, 0);
    temperature = bme.readTemperature();
    if (isnan(temperature)) {
      temperature = 25.8;
    }

    if (temperature > maxTemperature) {
      estadoDoBotao();

      if (estadoBotao2 == 1) {
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
    float variacaoDistancia = abs(distance - previousDistance);
    Serial.println(distance);
    // Verifica se a variação da distância está dentro do limite definido
    if (variacaoDistancia == 0) {
      delay(2000);
      tempoEstavel += 2000;   // Incrementa o tempo estável
      if (tempoEstavel > 10000) {  // Se o tempo de estabilidade exceder 10 segundos
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
      lcd.print("Ligado");
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
        timeOn = millis() - inicioLigado;  // Calcula o tempo atual de operação
        sprintf(tempoFuncionamentoVibracao, "%lu", timeOn);
        MQTT.publish(MQTT_OIL_CHANGE_TOPIC, tempoFuncionamentoVibracao);
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
      MQTT.publish(MQTT_OIL_CHANGE_TOPIC, "Trocar o óleo!, já fazem 40 horas que a máquina está ligada.");
    }
    Serial.print("Tempo acumulado: ");
    Serial.println(timeOn / 1000);
  }


  void setupWifi() {
    if (WiFi.status() == WL_CONNECTED)
      return;

    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(NET_SSID);

    WiFi.begin(NET_SSID, NET_PASSWORD);

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
    if (String(topic) == TOPIC_ACCELERATION) {
      medirPelaAceleracao = (message == "true");
    } else if (String(topic) == TOPIC_DISTANCE) {
      medirPelaDistancia = (message == "true");
    } else if (String(topic) == TOPIC_INTERVAL) {
      intervalo = message.toInt();
    } else if (String(topic) == TOPIC_MAX_TEMPERATURE) {
      temperaturaMaxima = message.toFloat();
    } else if (String(topic) == TOPIC_MAX_VIBRATION) {
      vibracaoMaxima = message.toFloat();
    }

    // atualiza as variáveis com as mensagens recebidas
    setAtributes(medirPelaAceleracao, medirPelaDistancia, intervalo, temperaturaMaxima, vibracaoMaxima);
  }

  bool setupMQTT() {
    if (!WiFi.isConnected()) {
      Serial.println("WiFi not connected. Cannot setup MQTT.");
      return false;
    }

    MQTT.setServer(MQTT_BROKER, MQTT_PORT);

    MQTT.setCallback([this](char* topic, byte* payload, unsigned int length) {
      this->callback(topic, payload, length);
    });

    Serial.print("Tentando se conectar ao broker MQTT: ");
    Serial.println(MQTT_BROKER);

    unsigned long startAttempt = millis();
    while (!MQTT.connected() && millis() - startAttempt < MQTT_TIMEOUT) {
      if (MQTT.connect(MQTT_ID)) {
        Serial.println("Conectado com sucesso ao broker MQTT!");
        mqttInitialized = true;

        // Inscreve-se nos tópicos separados
        MQTT.subscribe(TOPIC_ACCELERATION);
        MQTT.subscribe(TOPIC_DISTANCE);
        MQTT.subscribe(TOPIC_INTERVAL);
        MQTT.subscribe(TOPIC_MAX_TEMPERATURE);
        MQTT.subscribe(TOPIC_MAX_VIBRATION);

        Serial.println("Inscrito nos tópicos:");
        Serial.println(TOPIC_ACCELERATION);
        Serial.println(TOPIC_DISTANCE);
        Serial.println(TOPIC_INTERVAL);
        Serial.println(TOPIC_MAX_TEMPERATURE);
        Serial.println(TOPIC_MAX_VIBRATION);

        return true;
      }

      delay(100);
    };

    Serial.println("Falha ao conectar no broker.");
    return false;
  }

  bool validateConnection() {
    if (!mqttInitialized)
      return false;

    if (!MQTT.connected()) {
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
    bool success = MQTT.publish(MQTT_TEMPERATURE_TOPIC, temperaturaMQTT);
    Serial.println("Temperatura enviada ao Broker com sucesso");
    MQTT.loop();
    return success;
  }

  bool publishDistance() {
    if (!validateConnection()) {
      Serial.println("MQTT não conectado. Não foi possível publicar distância.");
      return false;
    }

    dtostrf(distance, 4, 2, distanciaMQTT);
    bool success = MQTT.publish(MQTT_DISTANCE_TOPIC, distanciaMQTT);
    Serial.println("Distancia enviada ao Broker com sucesso");
    MQTT.loop();
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
    bool success = MQTT.publish(MQTT_VIBRATION_TOPIC, vibracaoMQTT);
    Serial.println("Vibração enviada ao Broker com sucesso");
    MQTT.loop();
    return success;
  }

  bool publishUsageTime() {
    if (!validateConnection()) {
      Serial.println("MQTT nao conectado. Nao foi possivel publicar tempo de uso.");
      return false;
    }

    dtostrf(timeOn / 1000, 4, 2, tempoUso);
    bool success = MQTT.publish(MQTT_USAGE_TIME_TOPIC, tempoUso);
    Serial.println("Tempo de uso enviado ao Broker com sucesso");
    MQTT.loop();
    return success;
  }

  void reconnect() {
    // Loop até reconectar
    while (!MQTT.connected()) {
      Serial.print("Tentando conexão MQTT...");
      // Tenta se conectar
      if (MQTT.connect(MQTT_ID)) {
        Serial.println("Conectado ao MQTT Broker");
        // // Subscribe to topics if needed
        // MQTT.subscribe(MQTT_VIBRATION_TOPIC);
        // MQTT.subscribe(MQTT_TEMPERATURE_TOPIC);
        // MQTT.subscribe(MQTT_DISTANCE_TOPIC);
        // MQTT.subscribe(MQTT_USAGE_TIME_TOPIC);
        // MQTT.subscribe(MQTT_OIL_CHANGE_TOPIC);

      } else {
        Serial.print("Falha, rc=");
        Serial.print(MQTT.state());
      }
    }
  }
};