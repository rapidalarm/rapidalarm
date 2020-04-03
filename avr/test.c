#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>
#include <stdint.h>

#define SAMPLE_RATE 133
// period at which to sample ADCs
#define SAMPLE_TIME 1.0 / SAMPLE_RATE * 1000000

// convert cm H20 to ADC reading
#define CM2ADC(x) (uint16_t)(((x / 10.197 * 0.09) + 0.04) * ADC_LEVELS)

#define NOMINAL_RATIO 2.4
#define ALARM_TIME 10
#define ALARM_MAX CM2ADC(50)
#define ALARM_MIN CM2ADC(10)
#define ALARM_RATIO 1.1
#define ALARM_DIFF CM2ADC(3)

// ADC values
#define ADC_LEVELS 1024

const uint32_t COEFF_MAX = 1001;
const uint32_t ATTACK_COEFF = 1;
const uint32_t RELEASE_COEFF = 1000;
/* const uint16_t ATTACK_COEFF = 1; */
/* const uint16_t RELEASE_COEFF = 62; */
const uint16_t ALARM_SAMPLES = SAMPLE_RATE * ALARM_TIME;

// whether data is available from ADC
volatile bool sample_ready = false;


void handle_alarm(int sig) {
    sample_ready = true;
}


int main() {
    signal(SIGALRM, handle_alarm); // Install handler first,
    /* ualarm(SAMPLE_TIME, SAMPLE_TIME); // before scheduling it to be called. */
    ualarm(SAMPLE_TIME, 100); // before scheduling it to be called.

    FILE *f;
    f = fopen("../data/source_disconnect", "r");
    int buflen = 100;
    char line[buflen];

    // number of samples since last min/max
    uint16_t last_min, last_max;
    last_min = last_max = 0;

    int alarm_raised = 0;
    bool alarm_disabled = true;
    int t = 0;


    // current pressure readings
    uint32_t p1;
    uint32_t p2;
    // recursive filter max/min pressure readings
    uint32_t p_max = 0;
    uint32_t p_min = UINT32_MAX;

    uint32_t foo = 0;
    uint32_t bar = 0;

    if (f == NULL) {
        printf("No such file\n");
        return EXIT_FAILURE;
    }
    while(true) {
        if (sample_ready) {
            sample_ready = false;
            if (fgets(line, buflen, f) == 0) {
                return 0;
            }
            sscanf(line, "%d %d", &p2, &p1);
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
            if (alarm_disabled) {
                t++;
            }
            else {
                if (last_max > ALARM_SAMPLES)
                    alarm_raised = 1;
                if (last_min > ALARM_SAMPLES)
                    alarm_raised = 2;
                if (p_max < (p_min * ALARM_RATIO))
                    alarm_raised = 3;
                if ((p_max - p_min) < ALARM_DIFF)
                    alarm_raised = 4;
                if (p_max > ALARM_MAX)
                    alarm_raised = 5;
                if (p_min < ALARM_MIN)
                    alarm_raised = 6;
            }
            if (t > 5 * SAMPLE_RATE)
                alarm_disabled = false;

            printf("%d %d %d %d\n", p1 / 256, p_max / 256, p_min / 256, 100 * alarm_raised);

        }
        sleep(.001);
    }
}
