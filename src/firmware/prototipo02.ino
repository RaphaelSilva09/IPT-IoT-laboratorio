// Definição de bibliotecas
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_BME280.h>  // Biblioteca do BME280
#include <Ultrasonic.h>

// Definição do pino do BME280
#define BME280_ADDRESS 0x76  // Endereço I2C do BME280 (confirme o endereço)
#define SDA_PIN 21
#define SCL_PIN 22
#define MMA845X_ADDR 0x1C    // Endereço I2C do MMA845X (verifique o endereço correto no datasheet)

// Definição dos pinos do Ultrassônico
#define pino_trigger 2
#define pino_echo 4

// Inicialização de objetos
LiquidCrystal_I2C lcd(0x27, 16, 2); // Endereço I2C do LCD 16x2
Adafruit_BME280 bme;
Ultrasonic ultrasonic(pino_trigger, pino_echo);

// Definição dos pinos
int buzzer = 26;  // Pino do buzzer
int ledOn = 13;   // LED indicador de ligado
int ledTemp = 12; // LED de alerta de temperatura alta
int botao = 33;   // Pino do botão

unsigned long previousMillis = 0;  // Armazena o último tempo em que ocorreu a leitura
const long interval = 2000;        // Intervalo de 2 segundos para a leitura

int cont = 0;  // Contador para ligar/desligar o sistema
bool estadoBotao = false;  // Estado anterior do botão

// Classe para gerenciar o sistema de monitoramento
class SistemaMonitoramento {
public:
  float temperature;
  float distancia;
  float aceleracao;

  // Função para ativar os LEDs e o buzzer
  void ativarAlarmes() {
    tone(buzzer, 1000); // Ativar alarme sonoro
    digitalWrite(ledTemp, HIGH); // Acender LED de alerta
  }

  // Função para desativar os LEDs e o buzzer
  void desativarAlarmes() {
    noTone(buzzer);  // Desativa o som
    digitalWrite(ledTemp, LOW);  // Apagar LED de alerta
  }
};

// Instancia a classe
SistemaMonitoramento sistema;

// Função para ler a temperatura do BME280
float lerTemperatura() {
  return bme.readTemperature(); // Lê a temperatura do sensor BME280 em Celsius
}

// Função para exibir as informações no LCD com alertas
void exibirInformacoes(float temperatura, float distancia, float ax, float ay, float az) {
  lcd.clear();
  lcd.setCursor(0, 0);

  if (temperatura > 25.0) {
    lcd.print("ALERTA: Temp Alta");
    Serial.println("ALERTA: Temp Alta");
    sistema.ativarAlarmes();
  } else {
    lcd.print("Temp Normal:");
    lcd.print(temperatura);
  }
  
  if (distancia < 50.0) {
    lcd.print("Prensa esta ligada");
    Serial.println("Prensa esta ligada");
  } 
  
  if (abs(ax) > 2 || abs(ay) > 4 || abs(az) > 4) {
    lcd.print("Acelerometro ativ.");
    Serial.println("Acelerometro ativ.");
  } 

  // Exibe os valores no Serial Monitor
  Serial.print("Temp: ");
  Serial.print(temperatura, 1);
  Serial.print(" C, Distancia: ");
  Serial.print(distancia);
  Serial.print(" cm, Acel X: ");
  Serial.print(ax);
  Serial.print(" Y: ");
  Serial.print(ay);
  Serial.print(" Z: ");
  Serial.println(az);
}

void setup() {
  Serial.begin(9600);

  // Inicializa o BME280
  if (!bme.begin(BME280_ADDRESS)) {
    Serial.println("BME280 não encontrado");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Erro: BME280");
    while (1);  // Para o código se não conectar
  }

  // Inicializa o LCD
  lcd.init();  // Inicializa o LCD
  lcd.backlight();  // Liga a luz de fundo do LCD

  // Configuração dos pinos
  pinMode(buzzer, OUTPUT);
  pinMode(ledOn, OUTPUT);
  pinMode(ledTemp, OUTPUT);
  pinMode(botao, INPUT_PULLUP);

  // Inicializa os LEDs e o buzzer como desligados
  digitalWrite(ledOn, LOW);
  digitalWrite(ledTemp, LOW);
  noTone(buzzer);

  Wire.begin(SDA_PIN, SCL_PIN);  // Inicializa a comunicação I2C nos pinos especificados (SDA e SCL)
}

void loop() {
  // Verifica o estado do botão
  if (digitalRead(botao) == LOW && !estadoBotao) {
    cont = !cont;  // Alterna o valor de cont entre 0 e 1
    estadoBotao = true;  // Marca o botão como pressionado
  } else if (digitalRead(botao) == HIGH) {
    estadoBotao = false;  // Libera o botão quando ele for solto
  }

  // O sistema está ligado quando cont = 1
  if (cont == 1) {
    digitalWrite(ledOn, HIGH);  // Liga o LED indicador de sistema ligado

    // Obter o tempo atual
    unsigned long currentMillis = millis();

    // Atualiza a cada 2 segundos
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;

      sistema.temperature = lerTemperatura();
      sistema.distancia = ultrasonic.read();

      // Leitura dos dados de aceleração do MMA845X
      Wire.beginTransmission(MMA845X_ADDR);
      Wire.write(0x01); // Endereço do registro de saída de dados X-MSB
      Wire.endTransmission(false);
      Wire.requestFrom(MMA845X_ADDR, 6);

      float ax = 0, ay = 0, az = 0;
      if (Wire.available() == 6) {
        int16_t x = (Wire.read() << 8 | Wire.read()) >> 2;
        int16_t y = (Wire.read() << 8 | Wire.read()) >> 2;
        int16_t z = (Wire.read() << 8 | Wire.read()) >> 2;
        ax = x / 4096.0;
        ay = y / 4096.0;
        az = z / 4096.0;
      } else {
        Serial.println("Erro ao ler dados do MMA845X.");
      }

      exibirInformacoes(sistema.temperature, sistema.distancia, ax, ay, az);
      delay(1000); // Espera 1 segundo antes da próxima leitura
    }
  } else {
    digitalWrite(ledOn, LOW);
    sistema.desativarAlarmes();
    Serial.println('Sistema desligado');
  }
}
