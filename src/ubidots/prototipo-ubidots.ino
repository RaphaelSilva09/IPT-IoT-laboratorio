// Inclusão das bibliotecas necessárias
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_BME280.h>
#include <Ultrasonic.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ESPmDNS.h>
#include "Adafruit_ZeroFFT.h"
#include "signal.h"
#include "Sistema.h"
#include "UbidotsEsp32Mqtt.h"

Sistema sistema;
Sistema* Sistema::instance = nullptr;

unsigned long previousMillis = 0;
uint8_t lastDebounce = 0;
bool cont = 0;
bool estadoBotao = false;
bool interacao = false;

void setup()
{
    Serial.begin(9600);

    // Configuração de rede e transmissão para o HiveMQ
    sistema.setupWifi();
    sistema.setupMQTT();
    sistema.reconnect();

    // Inicialização do funcionamento
    sistema.systemOff();
    Wire.begin(sistema.SDApin, sistema.SCLpin);

    // Inicialização do BME280
    while (!bme.begin(0x76))
    {
        Serial.print("Erro ao inicializar BME280");
        delay(100);
    }

    // Inicialização do LCD
    lcd.init();
    lcd.backlight();

    // Verifica se o MMA845X está presente
    Wire.beginTransmission(0x1C);
    if (Wire.endTransmission() != 0)
    {
        Serial.println("Erro ao inicializar MMA845X");
    }

    // Configuração dos MMA845X
    Wire.beginTransmission(0x1C);
    Wire.write(0x2A);
    Wire.write(0x01);
    Wire.endTransmission();

    // Configuração dos pinos
    pinMode(sistema.buzzer, OUTPUT);
    pinMode(sistema.ledOn, OUTPUT);
    pinMode(sistema.ledTemp, OUTPUT);
    pinMode(sistema.botao, INPUT_PULLUP);

    digitalWrite(sistema.ledOn, LOW);
    digitalWrite(sistema.ledTemp, LOW);
    noTone(sistema.buzzer);
};

void loop()
{
    // Tratamento do botão
    if (digitalRead(sistema.botao) == LOW && !estadoBotao &&
        (millis() - lastDebounce > sistema.buttonDebounce))
    {
        cont = !cont;
        estadoBotao = true;
        sistema.systemStatus = true;
        lastDebounce = millis();
    }
    else if (digitalRead(sistema.botao) == HIGH)
    {
        estadoBotao = false;
    }

    if (cont)
    {
        digitalWrite(sistema.ledOn, HIGH);
        unsigned long currentMillis = millis();
        sistema.reconnect();

        // Coleta contínua de amostras para FFT quando aceleração está ativa
        if (sistema.isacceleration)
        {
            sistema.collectAccelSample(); // Coleta amostras continuamente
        }

        // Execução das verificações periódicas
        if (currentMillis - previousMillis >= sistema.interval)
        {
            previousMillis = currentMillis;
            sistema.publishUsageTime();

            if (!interacao)
            {
                sistema.printTemperature();
                sistema.publishTemperature();
                interacao = true;
            }
            else if (sistema.isacceleration && interacao)
            {
                sistema.printVibration();
                sistema.publishVibration();
                sistema.MachineOperationTime();
                interacao = false;
            }
            else if (sistema.isdistance && interacao)
            {
                sistema.printDistance();
                sistema.publishDistance();
                interacao = false;
            }
        }
    }
    else if (!cont && sistema.systemStatus)
    {
        // Desliga o sistema
        sistema.systemOff();
        digitalWrite(sistema.ledOn, LOW);
        sistema.systemStatus = false;
    }

    // Mantém a conexão MQTT ativa
    ubidots.loop();
}