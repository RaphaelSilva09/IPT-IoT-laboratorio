// Inclusão das bibliotecas necessárias
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_BME280.h>
#include <Ultrasonic.h>
#include "Sistema.h"

Sistema sistema;

unsigned long previousMillis = 0;
bool cont = 0;
bool estadoBotao = false;
bool interacao = false;

void setup() {
    sistema.systemOff();
    sistema.setAtributes(false, true);
    Wire.begin(sistema.SDApin, sistema.SCLpin);

    // Inicialização do BME280
    while (!bme.begin(0x76)) {
      Serial.print("Erro ao inicializar BME280");
      delay(100);
    }

    // Inicialização do LCD
    lcd.init();
    lcd.backlight();

    // Verifica se o MMA845X está presente
    Wire.beginTransmission(0x1C);
    while (Wire.endTransmission() != 0) {
        Serial.println("Erro ao inicializar MMA845X");
        delay(100);
    }

    // Configuração dos pinos
    pinMode(sistema.buzzer, OUTPUT);
    pinMode(sistema.ledOn, OUTPUT);
    pinMode(sistema.ledTemp, OUTPUT);
    pinMode(sistema.botao, INPUT_PULLUP);

    digitalWrite(sistema.ledOn, LOW);
    digitalWrite(sistema.ledTemp, LOW);
    noTone(sistema.buzzer);
};

void loop() {
  if (digitalRead(sistema.botao) == LOW && !estadoBotao) {
      cont = !cont;
      estadoBotao = true;
  } else if (digitalRead(sistema.botao) == HIGH) {
      estadoBotao = false;
  }

  if (cont == 1) {
      digitalWrite(sistema.ledOn, HIGH);
      unsigned long currentMillis = millis();

      if (currentMillis - previousMillis >= sistema.interval) {
          previousMillis = currentMillis;
          if (!interacao) {
              sistema.printTemperature();
              interacao = true;
          } else if (sistema.isDistance) {
              sistema.printAcceleration();
              interacao = false;
          } else if (sistema.isAcceleration) {
              sistema.printDistance();
              interacao = false;
          }
      }
  } else if (cont == 0) sistema.systemOff();
};