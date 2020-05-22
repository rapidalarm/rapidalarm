// RapidAlarm - Evan Widloski, Ryan Corey
// algorithm.c and algorithm.h are designed to be platform agnostic
// Call init_algorithm(samplerate) once to set the sample rate,
// then call run_algorithm(p) at this sample rate


// hack for compiling on Windows
#define _USE_MATH_DEFINES

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
float respiration_rate = 15;

// ----- Algorithm State -----

// disable alarm for N seconds when system turned on
bool alarm_disabled = true;
// recursive filter max/min pressure readings
float v_high = 25;
float v_low = 10;
// number of samples since last PIP attack sample
uint16_t t_high = 0;
// number of samples since last PEEP attack sample
uint16_t t_low = 0;
// envelope min/max values in each cycle
float v_max = 25;
float v_min = 10;
// whether current sample is on inhale or exhale
bool inhalation = false;

uint16_t alarm_counter = 0;
uint16_t t_peak = 0;
uint16_t new_breath_time;
float avg_breath_time;

// ----- Config -----

// attack time constant (s)
#define ATTACK_TIME (float)0.1
// release time constant (s)
#define RELEASE_TIME (float)15

uint8_t SAMPLE_RATE;
float ATTACK_COEFF;
float RELEASE_COEFF;
const float SMOOTH_COEFF = 0.5;

// alarm condition thresholds
uint16_t THRESH_NC = 10; // s
uint16_t THRESH_LP = 2; // cm H20
uint16_t THRESH_HP = 50; // cm H20
uint16_t THRESH_LR = 6; // breaths/min
uint16_t THRESH_HR = 35; // breaths/min
const float THRESH_RATIO = 1.5;
const float THRESH_DIFF = 3;

// ----- Functions -----

// compute samplerate dependent weights
void init_algorithm(uint8_t sample_rate) {
    SAMPLE_RATE = sample_rate;
    ATTACK_COEFF = pow(M_E, -1 / (SAMPLE_RATE * ATTACK_TIME));
    RELEASE_COEFF = pow(M_E, -1 / (SAMPLE_RATE * RELEASE_TIME));
}

// take in single sample at SAMPLE_RATE and update algorithm state
// p is pressure in cmh20
void run_algorithm(float p) {

    t_peak++;

    // if current sample is PIP attack
    if (p > v_high) {
        v_high = ATTACK_COEFF * v_high + (1 - ATTACK_COEFF) * p;
        v_max = p;
        t_high = 0;

        if (!inhalation) {
            inhalation= true;

            // smooth peep display
            peep = SMOOTH_COEFF * peep + (1 - SMOOTH_COEFF) * v_min;
        }
    }
    // if current sample is PIP release
    else {
        v_high = RELEASE_COEFF * v_high + (1 - RELEASE_COEFF) * p;
        t_high++;
    }
    // if current sample is PEEP attack
    if (p < v_low) {
        v_low = ATTACK_COEFF * v_low + (1 - ATTACK_COEFF) * p;
        v_min = p;
        t_low = 0;

        if (inhalation && (t_peak > 1)) {
            inhalation = false;

            // smooth pip display
            pip = SMOOTH_COEFF * pip + (1 - SMOOTH_COEFF) * v_max;

            // calculate breathing rate
            new_breath_time = t_peak - t_high;
            t_peak = t_high;

            // cap breath time to keep average in reasonable range during non-cycling
            if (new_breath_time > (SAMPLE_RATE * THRESH_NC)) {
                new_breath_time = SAMPLE_RATE * THRESH_NC;
            }
            avg_breath_time = SMOOTH_COEFF * avg_breath_time + (1 - SMOOTH_COEFF) * new_breath_time;
            respiration_rate = 60 * SAMPLE_RATE / avg_breath_time;
        }
    }
    // if current sample is PEEP release
    else {
        v_low = RELEASE_COEFF * v_low + (1 - RELEASE_COEFF) * p;
        t_low++;
    }

    // ----- Alarm conditions -----

    if (!alarm_disabled) {
        // non-cycling alarm
        if (
            t_high > SAMPLE_RATE * THRESH_NC ||
            t_low > SAMPLE_RATE * THRESH_NC ||
            v_high / v_low < THRESH_RATIO ||
            v_high - v_low < THRESH_DIFF
            ){
            alarm_raised = ALARM_NC;
            alarm_code = "nc";
            // spaces
            alarm_value = 0;
        }
        // low-pressure alarm
        else if (v_low < THRESH_LP){
            alarm_raised = ALARM_LP;
            alarm_code = "LP";
            alarm_value = v_low;
        }
        // high-pressure alarm
        else if (v_high > THRESH_HP){
            alarm_raised = ALARM_HP;
            alarm_code = "HP";
            alarm_value = v_high;
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
