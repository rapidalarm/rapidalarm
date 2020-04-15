// Ventmon - Ventilator Monitor
// Evan Widloski, Brian Ricconi
// 2020-04-09

#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include "display.h"

// should divide 1000
#define SAMPLE_RATE 100

#include "algorithm.h"

// convert cm H20 to ADC reading
/* #define CM2ADC(x) ((x / 10.197 * 0.09) + 0.04) * 1023 */
// convert ADC reading to cm H20 to ADC reading
#define ADC2CM(x) (((x / 1023) - 0.04) / 0.09) * 10.197

// state for pending button action. reset when handled
bool mode_pressed = false;
bool up_pressed = false;
bool down_pressed = false;

// ----- pins -----

const uint8_t PIN_MODE = 2;
const uint8_t PIN_UP = 0;
const uint8_t PIN_DOWN = 4;
const uint8_t PIN_BUZZ = 3;
const uint8_t PIN_PRES = A5;

const uint16_t debounce_time = 200;
unsigned long last_debounce_time = 0;

// debounce all buttons
bool debounce() {
    if (millis() - last_debounce_time > debounce_time) {
        last_debounce_time = millis();
        return true;
    }
    else {
        return false;
    }
}

// handle pin change interrupt for buttons
ISR (PCINT2_vect)
{
    // if PD0 low
    if ((PIND & (1 << PD2)) == 0 && debounce()) {
        mode_pressed = true;
    }
    // if PD2 low
    if ((PIND & (1 << PD4)) == 0 && debounce()) {
        up_pressed = true;
    }
    // if PD4 low
    if ((PIND & (1 << PD0)) == 0 && debounce()) {
        down_pressed = true;
    }
}

void pciSetup(byte pin)
{
    *digitalPinToPCMSK(pin) |= bit (digitalPinToPCMSKbit(pin));  // enable pin
    PCIFR  |= bit (digitalPinToPCICRbit(pin)); // clear any outstanding interrupt
    PCICR  |= bit (digitalPinToPCICRbit(pin)); // enable interrupt for the group
}

void setup() {
    Serial.begin(115200);
    Serial.println("start");
    // pressure input
    pinMode(A6, INPUT);
    pinMode(A7, INPUT);
    // button 1
    pinMode(PIN_MODE, INPUT);
    digitalWrite(PIN_MODE, HIGH);
    pciSetup(PIN_MODE);
    // button 2
    pinMode(PIN_UP, INPUT);
    digitalWrite(PIN_UP, HIGH);
    pciSetup(PIN_UP);
    // button 3
    pinMode(PIN_DOWN, INPUT);
    digitalWrite(PIN_DOWN, HIGH);
    pciSetup(PIN_DOWN);
    // buzzer
    pinMode(PIN_BUZZ, OUTPUT);
    analogWrite(PIN_BUZZ, 0);

    TCCR2B = TCCR2B & 0b11111000 | 0b00000010;

    for(byte i = 0; i < sizeof(digitPins); i++) {
        pinMode(digitPins[i], OUTPUT);
    }
    for(byte i = 0; i < sizeof(segmentPins); i++) {
        pinMode(segmentPins[i], OUTPUT);
    }

}

// time of last ADC sample
unsigned long last_sample = millis();
// adc pressure reading
float p;

// counter for cycling through displayed metrics and flashing display
uint16_t display_counter = 0;
// 8 second display cycle
const uint16_t display_counter_max = 8 * SAMPLE_RATE;

// user interface mode, cycled by mode button
uint8_t display_mode = 0;

void loop() {

    if (mode_pressed) {
        Serial.println("MODE pressed");
        display_mode += 1;
        display_mode %= 3;
        mode_pressed = false;
    }
    if (millis() - last_sample >= (1000 / SAMPLE_RATE)) {
        last_sample = millis();

        if (display_mode == 0) {
            p = ADC2CM((float) analogRead(PIN_PRES));

            run_algorithm(p);

            if(alarm_raised) {
                analogWrite(PIN_BUZZ, 0);
            }

            // change displayed metric every few seconds
            if (display_counter < display_counter_max / 3) {
                // PIP
                sprintf(displayedValue, "PI%2d", (uint8_t) limit(p_max, 0, 99));
            } else if (display_counter < 2 * display_counter_max / 3) {
                // PEEP
                sprintf(displayedValue, "PE%2d", (uint8_t) limit(p_min, 0, 99));
            } else {
                // rate
                sprintf(displayedValue, "rr%2d", (uint8_t) limit(respiration_rate, 0, 99));
            }

        }
        // edit PIP
        else if (display_mode == 1) {
            // change alarm setting
            if (up_pressed) {
                ALARM_MAX = limit(ALARM_MAX + 2, 0, 99);
                up_pressed = false;
            }
            else if (down_pressed) {
                ALARM_MAX = limit(ALARM_MAX - 2, 0, 99);
                down_pressed = false;
            }
            // display alarm setting
            if (display_counter % (SAMPLE_RATE / 2) > SAMPLE_RATE / 8) {
                sprintf(displayedValue, "PI%2d", ALARM_MAX);
            } else {
                sprintf(displayedValue, "PI  ");
            }
        }
        // edit PEEP
        else if (display_mode == 2) {
            // change alarm setting
            if (up_pressed) {
                ALARM_MIN = limit(ALARM_MIN + 2, 0, 99);
                up_pressed = false;
            }
            else if (down_pressed) {
                ALARM_MIN = limit(ALARM_MIN - 2, 0, 99);
                down_pressed = false;
            }
            // display alarm setting
            if (display_counter % (SAMPLE_RATE / 2) > SAMPLE_RATE / 8) {
                sprintf(displayedValue, "PE%2d", ALARM_MIN);
            } else {
                sprintf(displayedValue, "PE  ");
            }
        }

        display_counter++;
        display_counter %= display_counter_max;
    }

    update_display();

}
