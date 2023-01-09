#include <Arduino.h>

const unsigned char PINA_TEMPERATURE = A0;
const unsigned char PIND_BUTTON = 12;
const unsigned char PIND_PIEZO = 11;
const float TEMPERATURE_ANALOG_TO_CELSIUS = (5.0F / 1024.0F) * 100;
const int MAX_CELSIUS = 40;
const int MIN_CELSIUS = 0;
const int LED_PINS_LENGTH = 4;
const unsigned char LED_PINS[LED_PINS_LENGTH] = {3, 5, 9, 10 };
unsigned int LED_PINS_CELSIUS_TRIGGERS[LED_PINS_LENGTH + 1] = {};

bool isOn = false;
bool wasOn = false;
bool wasButtonPressed = false;

void turnOffLights();
void adjustLights(int);

void setup() {
    Serial.begin(9600);

    int diff = MAX_CELSIUS - MIN_CELSIUS;
    int diff_d_ledPinsLength = diff / LED_PINS_LENGTH;

    for (int i = 0; i < LED_PINS_LENGTH + 1; i++) {
        LED_PINS_CELSIUS_TRIGGERS[i] = i * diff_d_ledPinsLength + MIN_CELSIUS;
        Serial.println(LED_PINS_CELSIUS_TRIGGERS[i]);

        if (i < LED_PINS_LENGTH) {
            analogWrite(LED_PINS[i], 255);
            //tone(PIND_PIEZO, (i + 1) * 500, 250);
            delay(250);
            analogWrite(LED_PINS[i], 0);
        }
    }
}

void loop() {
    bool isButtonPressed = digitalRead(PIND_BUTTON);

    // Button released
    if (!isButtonPressed && wasButtonPressed) {
        isOn = !isOn;
    }

    if (isOn) {
        adjustLights(analogRead(PINA_TEMPERATURE));
    } else if (wasOn) {
        turnOffLights();
    }

    wasOn = isOn;
    wasButtonPressed = isButtonPressed;
}

void turnOffLights() {
    for (unsigned char i : LED_PINS) {
        analogWrite(i, 0);
    }
}

void adjustLights(int temperatureAnalog) {
    float celsius = (temperatureAnalog * TEMPERATURE_ANALOG_TO_CELSIUS) - 50.0F;

    for (int i = 0; i < LED_PINS_LENGTH; i++) {
        if (celsius >= LED_PINS_CELSIUS_TRIGGERS[i]) {
            if (i < LED_PINS_LENGTH - 1 && celsius >= LED_PINS_CELSIUS_TRIGGERS[i + 1]) {
                analogWrite(LED_PINS[i], 255);
                continue;
            }

            float analog = (celsius - LED_PINS_CELSIUS_TRIGGERS[i]) * 255 / (LED_PINS_CELSIUS_TRIGGERS[i + 1] - LED_PINS_CELSIUS_TRIGGERS[i]);
            Serial.print(i);
            Serial.print(": ");
            Serial.print(celsius);
            Serial.print(" ");
            Serial.println(analog);
            analogWrite(LED_PINS[i], analog);
        } else {
            analogWrite(LED_PINS[i], 0);
        }
    }
}