// Inclusão das bibliotecas necessárias
#include <Wire.h>              // Inclusão da biblioteca Wire
#include <LiquidCrystal_I2C.h> // Inclusão da biblioteca LiquidCrystal_I2C
#include <Adafruit_BME280.h>   // Inclusão da biblioteca BME280
#include <Ultrasonic.h>        // Inclusão da biblioteca Ultrasonic
#include <WiFi.h>              // Inclusão da biblioteca WiFi
#include <PubSubClient.h>      // Inclusão da biblioteca MQTT
#include <ESPmDNS.h>           // Inclusão da biblioteca mDNS
#include "Adafruit_ZeroFFT.h"  // Inclusão da biblioteca de FFT
#include "signal.h"            // Inclusão da biblioteca de sinais
#include "Sistema.h"           // Inclusão da classe Sistema

Sistema sistema;

unsigned long previousMillis = 0; // Armazena o último tempo em que ocorreu a leitura
uint8_t lastDebounce = 0;         // Último debounce do botão
bool cont = 0;                    // Contador para ligar/desligar o sistema
bool estadoBotao = false;         // Estado anterior do botão
bool interacao = false;           // Flag para alternar entre as interações

void setup()
{
    Serial.begin(9600); // Inicialização da comunicação serial

    // Configuração de rede e transmissão para o HiveMQ
    sistema.setupWifi(); // Configuração do WiFi
    sistema.setupMQTT(); // Configuração do MQTT
    sistema.reconnect(); // Reconecta ao MQTT

    // Inicialização do funcionamento
    sistema.systemOff();                        // Desliga o sistema
    Wire.begin(sistema.SDApin, sistema.SCLpin); // Inicialização do I2C

    // Inicialização do BME280
    while (!bme.begin(0x76)) // Inicialização do BME280
    {
        Serial.print("Erro ao inicializar BME280"); // Erro ao inicializar o BME280
        delay(100);
    }

    // Inicialização do LCD
    lcd.init();      // Inicializa o LCD
    lcd.backlight(); // Liga a luz de fundo do LCD

    // Verifica se o MMA845X está presente
    Wire.beginTransmission(0x1C);    // Inicialização do MMA845X
    if (Wire.endTransmission() != 0) // Erro ao inicializar o MMA845X
    {
        Serial.println("Erro ao inicializar MMA845X"); // Erro ao inicializar o MMA845X
    }

    // Configuração dos MMA845X
    Wire.beginTransmission(0x1C); // Inicialização do MMA845X
    Wire.write(0x2A);             // Configuração do registrador de controle
    Wire.write(0x01);             // Configuração do modo de medição
    Wire.endTransmission();       // Configuração do modo de medição

    // Configuração dos pinos
    pinMode(sistema.buzzer, OUTPUT);      // Buzzer
    pinMode(sistema.ledOn, OUTPUT);       // LED indicador de sistema ligado
    pinMode(sistema.ledTemp, OUTPUT);     // LED de temperatura
    pinMode(sistema.botao, INPUT_PULLUP); // Botão de controle do sistema

    digitalWrite(sistema.ledOn, LOW);   // Desliga o LED indicador de sistema ligado
    digitalWrite(sistema.ledTemp, LOW); // Desliga o LED de temperatura
    noTone(sistema.buzzer);             // Desliga o buzzer
};

void loop() // Loop principal
{
    // Tratamento do botão
    if (digitalRead(sistema.botao) == LOW && !estadoBotao && // Botão pressionado e debounce
        (millis() - lastDebounce > sistema.buttonDebounce))  // Botão pressionado
    {
        cont = !cont;                // Alterna o valor de cont entre 0 e 1
        estadoBotao = true;          // Marca o botão como pressionado
        sistema.systemStatus = true; // Ativa o sistema
        lastDebounce = millis();     // Atualiza o tempo do último debounce
    }
    else if (digitalRead(sistema.botao) == HIGH) // Botão solto
    {
        estadoBotao = false; // Libera o botão quando ele for solto
    }

    if (cont)
    {
        digitalWrite(sistema.ledOn, HIGH);      // Liga o LED indicador de sistema ligado
        unsigned long currentMillis = millis(); // Obtém o tempo atual
        sistema.reconnect();                    // Reconecta ao MQTT

        // Coleta contínua de amostras para FFT quando aceleração está ativa
        if (sistema.isAcceleration)
        {
            sistema.collectAccelSample(); // Coleta amostras continuamente
        }

        // Execução das verificações periódicas
        if (currentMillis - previousMillis >= sistema.interval)
        {
            previousMillis = currentMillis; // Atualiza o tempo da última leitura
            sistema.publishUsageTime(); // Publica o tempo de uso

            if (!interacao) // Verifica a interação
            {
                sistema.printTemperature(); // Exibe a temperatura
                sistema.publishTemperature(); // Publica a temperatura
                interacao = true; // Ativa a interação
            }
            else if (sistema.isAcceleration && interacao) // Verifica a aceleração
            {
                sistema.printVibration(); // Exibe a vibração
                sistema.publishVibration(); // Publica a vibração
                sistema.MachineOperationTime(); // Atualiza o tempo de operação
                interacao = false; // Desativa a interação
            }
            else if (sistema.isDistance && interacao) // Verifica a distância
            {
                sistema.printDistance(); // Exibe a distância
                sistema.publishDistance(); // Publica a distância
                interacao = false; // Desativa a interação
            }
        }
    }
    else if (!cont && sistema.systemStatus) // Sistema desligado
    {
        // Desliga o sistema
        sistema.systemOff(); // Desliga o sistema
        digitalWrite(sistema.ledOn, LOW); // Desliga o LED indicador de sistema ligado
        sistema.systemStatus = false; // Desativa o sistema
    }

    // Mantém a conexão MQTT ativa
    MQTT.loop();
}