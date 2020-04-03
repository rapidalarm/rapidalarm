#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdint.h>

#define SAMPLE_RATE 133
// period at which to sample ADCs
#define SAMPLE_TIME 1.0 / SAMPLE_RATE * 1000

// convert cm H20 to ADC reading
#define CM2ADC(x) (uint16_t)(((x / 10.197 * 0.09) + 0.04) * ADC_LEVELS)

#define NOMINAL_RATIO 2.4
#define ALARM_TIME 10
/* #define ALARM_MAX 493 */
uint32_t ALARM_MAX = 583;
#define ALARM_MIN 131
#define ALARM_RATIO 1.1
#define ALARM_DIFF 68

// ADC values
#define ADC_LEVELS 1024

const uint32_t COEFF_MAX = 1001;
const uint32_t ATTACK_COEFF = 1;
const uint32_t RELEASE_COEFF = 1000;
/* const uint16_t ATTACK_COEFF = 1; */
/* const uint16_t RELEASE_COEFF = 62; */
const uint16_t ALARM_SAMPLES = SAMPLE_RATE * ALARM_TIME;

void setup() {
    Serial.begin(9600);
    pinMode("A6", INPUT);
    pinMode("A7", INPUT);
    pinMode(2, OUTPUT);
    digitalWrite(2, LOW);
}

void loop() {

    // number of samples since last min/max
    uint16_t last_min, last_max;
    last_min = last_max = 0;

    int alarm_raised = 0;
    bool alarm_disabled = true;
    int t = 0;

    // whether data is available from ADC
    int last_sample = millis();

    // current pressure readings
    uint32_t p1;
    uint32_t p2;
    // recursive filter max/min pressure readings
    uint32_t p_max = 0;
    uint32_t p_min = UINT32_MAX;

    uint32_t foo = 0;
    uint32_t bar = 0;

    while(true) {
        if (millis() - last_sample > SAMPLE_TIME) {
            last_sample = millis();
            p1 = analogRead(6);
            p2 = analogRead(7);
            p1 *= 256;
            p2 *= 256;
            if (p1 > p_max) {
                p_max = (uint32_t)(
                    (
                        (uint32_t)p_max * ATTACK_COEFF +
                        (uint32_t)p1 * (COEFF_MAX - ATTACK_COEFF)
                    ) / COEFF_MAX
                );
                last_max = 0;
            }
            else {
                p_max = (uint32_t)(
                    (
                        (uint32_t)p_max * RELEASE_COEFF +
                        (uint32_t)p1 * (COEFF_MAX - RELEASE_COEFF)
                    ) / COEFF_MAX
                );
                last_max++;
            }
            if (p1 < p_min) {
                p_min = (ATTACK_COEFF * p_min + (COEFF_MAX - ATTACK_COEFF) * p1) / COEFF_MAX;
                last_min = 0;
            }
            else {
                p_min = (RELEASE_COEFF * p_min + (COEFF_MAX - RELEASE_COEFF) * p1) / COEFF_MAX;
                last_min++;
            }

            // ----- Alarm conditions -----
            if (!alarm_disabled) {
                if (last_max > ALARM_SAMPLES)
                    alarm_raised = 1;
                if (last_min > ALARM_SAMPLES)
                    alarm_raised = 2;
                if (p_max < (p_min * ALARM_RATIO))
                    alarm_raised = 3;
                if ((p_max - p_min) < 256 * ALARM_DIFF)
                    alarm_raised = 4;
                if (p_max > 256 * ALARM_MAX)
                    alarm_raised = 5;
                if (p_min < 256 * ALARM_MIN)
                    alarm_raised = 6;
                digitalWrite(2, HIGH);
            }
            if (t > 5 * SAMPLE_RATE)
                alarm_disabled = false;

            t++;

            Serial.print(p_max);
            Serial.print("\t");
            Serial.print(256 * ALARM_MAX);
            Serial.print("\t");
            Serial.println(alarm_raised);
            /* printf("%d %d %d %d\n", p1 / 256, p_max / 256, p_min / 256, 100 * alarm_raised); */

        }
    }
}
