// Definição de bibliotecas
#include <Wire.h> 
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <LiquidCrystal_I2C.h>

// Definições
#define SEALEVELPRESSURE_HPA (1013.25)  // Pressão ao nível do mar em hPa

// Inicialização de objetos
Adafruit_BME280 bme; // Inicializa o sensor BME280
LiquidCrystal_I2C lcd(0x27, 16, 2); // Endereço I2C do LCD 16x2

// Definição dos pinos
int buzzer = 8;  // Pino do buzzer
int ledOn = 13;  // LED indicador de ligado
int ledTemp = 12; // LED de alerta de temperatura alta
int botao = 7;   // Pino do botão

unsigned long previousMillis = 0;  // Armazena o último tempo em que ocorreu a leitura
const long interval = 2000;        // Intervalo de 2 segundos para a leitura

int cont = 0;  // Contador para ligar/desligar o sistema
bool estadoBotao = false;  // Estado anterior do botão

// Classe para gerenciar o sistema de monitoramento
class SistemaMonitoramento {
public:
  float temperature;

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
  return bme.readTemperature(); // Lê a temperatura do sensor BME280
}

// Função para exibir as informações no LCD
void exibirInformacoes(float temperatura) {
  lcd.clear();  
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");  
  lcd.print(temperatura, 1);  
  lcd.print(" C");  

  if (temperatura > 25.0) {
    lcd.setCursor(0, 1);
    lcd.print("ALERTA: Temp Alta");
  } else {
    lcd.setCursor(0, 1);
    lcd.print("Temp Normal");
  }
}

void setup() {
  Serial.begin(9600);

  // Inicializa o LCD
  lcd.init();  // Inicializa o LCD 
  lcd.backlight();  // Liga a luz de fundo do LCD

  // Inicializa o sensor BME280
  if (!bme.begin(0x76)) {
    Serial.println("Erro ao inicializar o sensor BME280!");
    while (1); 
  }

  // Configuração dos pinos
  pinMode(buzzer, OUTPUT);
  pinMode(ledOn, OUTPUT);
  pinMode(ledTemp, OUTPUT);
  pinMode(botao, INPUT_PULLUP);  

  // Inicializa os LEDs e o buzzer como desligados
  digitalWrite(ledOn, LOW); 
  digitalWrite(ledTemp, LOW);
  noTone(buzzer);
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
      // Atualiza o tempo anterior
      previousMillis = currentMillis;

      // Lê a temperatura
      sistema.temperature = lerTemperatura();

      // Verificar se a leitura do sensor está correta
      if (isnan(sistema.temperature)) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Erro na leitura");
        return;
      }

      // Exibe as informações no LCD
      exibirInformacoes(sistema.temperature);

      // Verifica se a temperatura é maior que 25°C
      if (sistema.temperature > 25.0) {
        sistema.ativarAlarmes();
      } else {
        sistema.desativarAlarmes();
      }
    }
  } else {
    // O sistema é delisgado quando cont = 0
    digitalWrite(ledOn, LOW);  // Desliga o LED indicador de sistema ligado
    sistema.desativarAlarmes();  // Garante que os alarmes estão desligados
    lcd.clear();  // Limpa o LCD
    lcd.setCursor(0, 0);
    lcd.print("Sistema");
    lcd.setCursor(0, 1);
    lcd.print("desligado");
  }
}
