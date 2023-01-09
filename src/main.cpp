#include <Arduino.h>

const unsigned char PINA_TEMPERATURE = A0;
const unsigned char PIND_BUTTON = 12;
const unsigned char PIND_PIEZO = 11;
const float TEMPERATURE_ANALOG_TO_CELSIUS = (5.0F / 1024.0F) * 100;
const int MAX_CELSIUS = 40;
const int MIN_CELSIUS = 0;
const unsigned long DURATION_BETWEEN_TONES = 200;
const int LED_PINS_LENGTH = 4;
const unsigned char LED_PINS[LED_PINS_LENGTH] = {3, 5, 9, 10 };
int LED_PINS_CELSIUS_TRIGGERS[LED_PINS_LENGTH + 1] = {};

bool isOn = false;
bool wasOn = false;
bool wasButtonPressed = false;

void turnOffLEDs();
// Adjusts all the LEDs depending on the analog out temperature.
// The first LEDs will be turned on if the Celsius is higher than MIN_CELSIUS
void adjustLEDs(int temperatureAnalog);

void setup() {
    Serial.begin(9600);

    int diff = MAX_CELSIUS - MIN_CELSIUS;
    int diff_d_ledPinsLength = diff / LED_PINS_LENGTH;

    for (int i = 0; i < LED_PINS_LENGTH + 1; i++) {
        LED_PINS_CELSIUS_TRIGGERS[i] = i * diff_d_ledPinsLength + MIN_CELSIUS;
        Serial.println(LED_PINS_CELSIUS_TRIGGERS[i]);

        if (i < LED_PINS_LENGTH) {
            analogWrite(LED_PINS[i], 255);
            tone(PIND_PIEZO, (i + 1) * 500, DURATION_BETWEEN_TONES);
            delay(DURATION_BETWEEN_TONES);
            analogWrite(LED_PINS[i], 0);
        }
    }

    for (int i = LED_PINS_LENGTH - 1; i >= 0; i--) {
        analogWrite(LED_PINS[i], 255);
        tone(PIND_PIEZO, (i + 1) * 500, DURATION_BETWEEN_TONES);
        delay(DURATION_BETWEEN_TONES);
        analogWrite(LED_PINS[i], 0);
    }
}

void loop() {
    bool isButtonPressed = digitalRead(PIND_BUTTON);

    // Button released
    if (!isButtonPressed && wasButtonPressed) {
        isOn = !isOn;
    }

    if (isOn) {
        adjustLEDs(analogRead(PINA_TEMPERATURE));
    } else if (wasOn) {
        turnOffLEDs();
    }

    wasOn = isOn;
    wasButtonPressed = isButtonPressed;
}

float temperatureAnalogToCelsius(int temperatureAnalog) {
    return (temperatureAnalog * TEMPERATURE_ANALOG_TO_CELSIUS) - 50.0F;
}

int ledPWM(float celsius, int ledIndex) {
    if ((ledIndex < LED_PINS_LENGTH - 1) && (celsius >= LED_PINS_CELSIUS_TRIGGERS[ledIndex + 1])) {
        return 255;
    }

    int triggerDiff = LED_PINS_CELSIUS_TRIGGERS[ledIndex + 1] - LED_PINS_CELSIUS_TRIGGERS[ledIndex];

    return ((celsius - LED_PINS_CELSIUS_TRIGGERS[ledIndex]) * 255.0F) / triggerDiff;
}

void turnOffLEDs() {
    for (unsigned char i : LED_PINS) {
        analogWrite(i, 0);
    }
}

void adjustLEDs(int temperatureAnalog) {
    float celsius = temperatureAnalogToCelsius(temperatureAnalog);

    for (int i = 0; i < LED_PINS_LENGTH; i++) {
        if (celsius >= LED_PINS_CELSIUS_TRIGGERS[i]) {
            analogWrite(LED_PINS[i], ledPWM(celsius, i));
        } else {
            analogWrite(LED_PINS[i], 0);
        }
    }
}