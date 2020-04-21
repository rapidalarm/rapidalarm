// Ventmon - Ventilator Monitor
// Evan Widloski, Brian Ricconi
// 2020-04-09

#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>

// ----- Config -----

// pressure sensor sample rate, should divide 1000 evenly
#define SAMPLE_RATE 100

// pin configuration
const uint8_t PIN_MODE = 2;
const uint8_t PIN_UP = 0;
const uint8_t PIN_DOWN = 4;
const uint8_t PIN_BUZZ = 3;
const uint8_t PIN_PRES = A5;
const byte digit_pins[] = {A4, A2, A0, 11, 10};
const byte segment_pins[] = {6, 12, 7, A1, A3, 8, 9, 5};

// buzzer volume
const uint8_t volume = 5;

// button debounce time, milliseconds
const uint16_t debounce_time = 200;

#include "algorithm.h"
#include "display.h"
#include "alarm.h"

// ----- Main -----

// handle pin change interrupt for buttons
ISR (PCINT2_vect)
{
    // mode button press
    if ((PIND & (1 << PD0)) == 0 && debounce()) {
        mode_pressed = true;
    }
    // down button press
    if ((PIND & (1 << PD4)) == 0 && debounce()) {
        up_pressed = true;
    }
    // up button press
    if ((PIND & (1 << PD2)) == 0 && debounce()) {
        down_pressed = true;
    }
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
    pci_setup(PIN_MODE);
    // button 2
    pinMode(PIN_UP, INPUT);
    digitalWrite(PIN_UP, HIGH);
    pci_setup(PIN_UP);
    // button 3
    pinMode(PIN_DOWN, INPUT);
    digitalWrite(PIN_DOWN, HIGH);
    pci_setup(PIN_DOWN);
    // buzzer
    pinMode(PIN_BUZZ, OUTPUT);
    analogWrite(PIN_BUZZ, 0);

    // set buzzer frequency to ~4kHz
    TCCR2B = TCCR2B & 0b11111000 | 0b00000010;

    // display pin initialization
    for(byte i = 0; i < sizeof(digit_pins); i++) {
        pinMode(digit_pins[i], OUTPUT);
    }
    for(byte i = 0; i < sizeof(segment_pins); i++) {
        pinMode(segment_pins[i], OUTPUT);
    }

}


void loop() {

    if (mode_pressed) {

        Serial.println("MODE pressed");
        mode_pressed = false;

        if (mode == MODE_ALARM) {
            // disable alarm
            alarm_raised = ALARM_NONE;
            analogWrite(PIN_BUZZ, 0);
            alarm_disabled = true;
            mode = MODE_DISPLAY;
        }
        else {
            mode = (mode + 1) % total_modes;
        }
    }
    if (millis() - last_sample >= (1000 / SAMPLE_RATE)) {
        last_sample = millis();

        if (mode == 0) {
            p = ADC2CM((float) analogRead(PIN_PRES));

            run_algorithm(p);

            if(alarm_raised) {
                mode = MODE_ALARM;
            }

            // change displayed metric every few seconds
            if (counter < counter_max / 3) {
                // PIP
                sprintf(displayedValue, "PI:%2d", (uint8_t) limit(pip, 0, 99));
            }
            else if (counter < 2 * counter_max / 3) {
                // PEEP
                sprintf(displayedValue, "PE:%2d", (uint8_t) limit(peep, 0, 99));
            }
            else {
                // rate
                sprintf(
                    displayedValue,
                    "rr:%2d",
                    (uint8_t) limit(respiration_rate, 0, 99)
                );
            }

        }
        // edit non-cycling alarm condition
        else if (mode == MODE_SET_THRESH_NC) {
            // change alarm setting
            if (up_pressed) {
                THRESH_NC = limit(THRESH_NC, 0, 25) + 5;
                up_pressed = false;
            }
            else if (down_pressed) {
                THRESH_NC = limit(THRESH_NC, 5, 35) - 5;
                down_pressed = false;
            }
            // flash alarm setting
            if (counter % (SAMPLE_RATE / 2) > SAMPLE_RATE / 8) {
                sprintf(displayedValue, "NC:%2d", THRESH_NC);
            } else {
                sprintf(displayedValue, "NC:  ");
            }
        }
        // edit low-pressure alarm condition
        else if (mode == MODE_SET_THRESH_LP) {
            // change alarm setting
            if (up_pressed) {
                THRESH_LP = limit(THRESH_LP, 1, 19) + 1;
                up_pressed = false;
            }
            else if (down_pressed) {
                THRESH_LP = limit(THRESH_LP, 3, 21) - 1;
                down_pressed = false;
            }
            // flash alarm setting
            if (counter % (SAMPLE_RATE / 2) > SAMPLE_RATE / 8) {
                sprintf(displayedValue, "LP:%2d", THRESH_LP);
            } else {
                sprintf(displayedValue, "LP:  ");
            }
        }
        // edit high-pressure alarm condition
        else if (mode == MODE_SET_THRESH_HP) {
            // change alarm setting
            if (up_pressed) {
                THRESH_HP = limit(THRESH_HP, 25, 85) + 5;
                up_pressed = false;
            }
            else if (down_pressed) {
                THRESH_HP = limit(THRESH_HP, 35, 95) - 5;
                down_pressed = false;
            }
            // flash alarm setting
            if (counter % (SAMPLE_RATE / 2) > SAMPLE_RATE / 8) {
                sprintf(displayedValue, "HP:%2d", THRESH_HP);
            } else {
                sprintf(displayedValue, "HP:  ");
            }
        }
        // edit low-rate alarm condition
        else if (mode == MODE_SET_THRESH_LR) {
            // change alarm setting
            if (up_pressed) {
                THRESH_LP = limit(THRESH_LR, 4, 14) + 1;
                up_pressed = false;
            }
            else if (down_pressed) {
                THRESH_LP = limit(THRESH_LR, 6, 16) - 1;
                down_pressed = false;
            }
            // flash alarm setting
            if (counter % (SAMPLE_RATE / 2) > SAMPLE_RATE / 8) {
                sprintf(displayedValue, "Lr:%2d", THRESH_LR);
            } else {
                sprintf(displayedValue, "Lr:  ");
            }
        }
        // edit high-rate alarm condition
        else if (mode == MODE_SET_THRESH_HR) {
            // change alarm setting
            if (up_pressed) {
                THRESH_HR = limit(THRESH_HR, 10, 55) + 5;
                up_pressed = false;
            }
            else if (down_pressed) {
                THRESH_HR = limit(THRESH_HR, 20, 65) - 5;
                down_pressed = false;
            }
            // flash alarm setting
            if (counter % (SAMPLE_RATE / 2) > SAMPLE_RATE / 8) {
                sprintf(displayedValue, "Hr:%2d", THRESH_HR);
            } else {
                sprintf(displayedValue, "Hr:  ");
            }
        }
        // alarm mode
        else if (mode == MODE_ALARM) {
            // beep alarm on-off
            if (counter % SAMPLE_RATE > SAMPLE_RATE / 2) {
                analogWrite(PIN_BUZZ, volume);
            }
            else {
                analogWrite(PIN_BUZZ, 0);
            }
            // flash alarm code
            if (counter % (SAMPLE_RATE / 2) > SAMPLE_RATE / 8) {
                sprintf(
                    displayedValue,
                    "%s:%2d",
                    alarm_code,
                    (uint8_t) limit(alarm_value, 0, 99)
                );
            }
            else {
                sprintf(displayedValue, "     ");
            }
        }

        counter++;
        counter %= counter_max;
    }

    update_display();

}
