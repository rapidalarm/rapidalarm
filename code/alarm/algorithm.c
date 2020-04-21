#include "algorithm.h"
#include <math.h>

// ----- Output -----

// whether there is an alarm condition
alarm_t alarm_raised = ALARM_NONE;
// display code for raised alarm
char *alarm_code = "  ";
// value that caused alarm to be raised
float alarm_value = 0;
// peak inspiratory pressure (cm H20)
float pip = 25;
// positive end expiratory pressure (cm H20)
float peep = 10;
// breath respiration_rate (breaths/min)
uint16_t respiration_rate = 15;

// ----- Algorithm State -----

// disable alarm for N seconds when system turned on
bool alarm_disabled = true;
// recursive filter max/min pressure readings
float p_max = 0;
float p_min = 100;
// number of samples since last PIP attack sample
uint16_t last_pip = 0;
// number of samples since last PEEP attack sample
uint16_t last_peep = 0;
// store previous pip, peep output for smoothing filter
float last_pip_value = 25;
float last_peep_value = 10;
// whether current sample is on inhale or exhale
bool inhalation = false;

uint16_t alarm_counter = 0;
uint16_t time_since_last_breath = 0;
uint16_t new_breath_time;

// ----- Config -----

// attack time constant
#define ATTACK_TIME (float)0.1
// release time constant
#define RELEASE_TIME (float)15

const float ATTACK_COEFF = pow(M_E, -1 / (SAMPLE_RATE * ATTACK_TIME));
const float RELEASE_COEFF = pow(M_E, -1 / (SAMPLE_RATE * RELEASE_TIME));
const float SMOOTH_COEFF = 0.5;

// alarm condition thresholds
uint16_t THRESH_NC = 10; // s
uint16_t THRESH_LP = 2; // cm H20
uint16_t THRESH_HP = 40; // cm H20
uint16_t THRESH_LR = 6; // breaths/min
uint16_t THRESH_HR = 35; // breaths/min
const float THRESH_RATIO = 1.5;
const float THRESH_DIFF = 3;

// ----- Functions -----

// take in single sample at SAMPLE_RATE and update algorithm state
void run_algorithm(float p) {

    time_since_last_breath++;

    // if current sample is PIP attack
    if (p > p_max) {
        p_max = p_max * ATTACK_COEFF + p * (1 - ATTACK_COEFF);
        last_pip = 0;
        last_pip_value = p;

        if (!inhalation) {
            inhalation= true;

            // smooth peep display
            peep = SMOOTH_COEFF * peep + (1 - SMOOTH_COEFF) * last_peep_value;
        }
    }
    // if current sample is PIP release
    else {
        p_max = p_max * RELEASE_COEFF + p * (1 - RELEASE_COEFF);
        last_pip++;
    }
    // if current sample is PEEP attack
    if (p < p_min) {
        p_min = p_min * ATTACK_COEFF + p * (1 - ATTACK_COEFF);
        last_peep = 0;
        last_peep_value = p;

        if (inhalation && time_since_last_breath > 1) {
            inhalation = false;

            // smooth pip display
            pip = SMOOTH_COEFF * pip + (1 - SMOOTH_COEFF) * last_pip_value;

            // calculate breathing rate
            new_breath_time = time_since_last_breath - last_pip;
            time_since_last_breath = last_pip;

            // cap breath time to keep average in reasonable range during non-cycling
            if (new_breath_time > (SAMPLE_RATE * THRESH_NC)) {
                new_breath_time = SAMPLE_RATE * THRESH_NC;
            }

        }
    }
    // if current sample is PEEP release
    else {
        p_min = p_min * RELEASE_COEFF + p * (1 - RELEASE_COEFF);
        last_peep++;
    }

    // ----- Alarm conditions -----

    if (!alarm_disabled) {
        // non-cycling alarm
        if (
            last_pip > SAMPLE_RATE * THRESH_NC ||
            last_peep > SAMPLE_RATE * THRESH_NC ||
            p_max / p_min < THRESH_RATIO ||
            p_max - p_min < THRESH_DIFF
            ){
            alarm_raised = ALARM_NC;
            alarm_code = "nc";
            // spaces
            alarm_value = 0;
        }
        // low-pressure alarm
        else if (p_min < THRESH_LP){
            alarm_raised = ALARM_LP;
            alarm_code = "LP";
            alarm_value = p_min;
        }
        // high-pressure alarm
        else if (p_max > THRESH_HP){
            alarm_raised = ALARM_HP;
            alarm_code = "HP";
            alarm_value = p_max;
        }
        // high-rate alarm
        else if (respiration_rate > THRESH_HR) {
            alarm_raised = ALARM_HR;
            alarm_code = "Hr";
            alarm_value = respiration_rate;
        }
        // low-rate alarm
        else if (respiration_rate < THRESH_LR) {
            alarm_raised = ALARM_LR;
            alarm_code = "Lr";
            alarm_value = respiration_rate;
        }
    }

    // enable alarms after 5 seconds
    if (alarm_disabled && alarm_counter > 5 * SAMPLE_RATE) {
        alarm_disabled = false;
        alarm_counter = 0;
    } else {
        alarm_counter++;
    }
}
