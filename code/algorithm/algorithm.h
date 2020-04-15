// ----- Types -----

// alarm types
typedef enum {
    // no alarm
    NONE,
    // ventilator not cycling
    NON_CYCLING,
    // PEEP below threshold
    LOW_PRESSURE,
    // PIP above threshold
    HIGH_PRESSURE,
    // respiratory rate below threshold
    LOW_RATE,
    // respiratory rate above threshold
    HIGH_RATE
} ALARM;

// ----- Algorithm State -----

// whether there is an alarm condition
ALARM alarm_raised = NONE;
// number of samples since last PEEP attack sample
uint16_t last_peep = 0;
// number of samples since last PIP attack sample
uint16_t last_pip = 0;
// disable alarm for N seconds when system turned on
bool alarm_disabled = true;
uint16_t alarm_counter = 0;
bool reached_min = false;
uint16_t last_first_max = 0;
// breath respiration_rate (breaths/min)
uint16_t respiration_rate = 0;
// recursive filter max/min pressure readings
float p_max = 0;
float p_min = 100;

// ----- Config -----

const float ATTACK_COEFF = .9;
const float RELEASE_COEFF = .999;

const uint16_t ALARM_TIME = 10;
uint16_t ALARM_MAX = 60;
uint16_t ALARM_MIN = 10;
const uint16_t ALARM_RATIO = 1.5;
const uint16_t ALARM_DIFF = 86;

// ----- Functions -----

// take in single sample at SAMPLE_RATE and update algorithm state
void run_algorithm(float p) {

    // if current sample is PIP attack
    if (p > p_max) {
        p_max = p_max * ATTACK_COEFF + p * (1 - ATTACK_COEFF);
        last_pip = 0;
        if (reached_min) {
            respiration_rate = 60 * SAMPLE_RATE / last_first_max;

            last_first_max = 0;
            reached_min = false;
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
        reached_min = true;
    }
    // if current sample is PEEP release
    else {
        p_min = p_min * RELEASE_COEFF + p * (1 - RELEASE_COEFF);
        last_peep++;
    }

    // ----- Alarm conditions -----

    if (!alarm_disabled) {
        if (last_pip > SAMPLE_RATE * ALARM_TIME){
            alarm_raised = NON_CYCLING;
        }
        if (last_peep > SAMPLE_RATE * ALARM_TIME){
            alarm_raised = NON_CYCLING;
        }
        if (p_max < (p_min * ALARM_RATIO)){
            // FIXME - add ratio alarm condition
            alarm_raised = NON_CYCLING;
        }
        if ((p_max - p_min) < ALARM_DIFF){
            // FIXME - add diff alarm condition
            alarm_raised = NON_CYCLING;
        }
        if (p_max > ALARM_MAX){
            alarm_raised = HIGH_PRESSURE;
        }
        if (p_min < ALARM_MIN){
            alarm_raised = LOW_PRESSURE;
        }
    }

    // enable alarms after 5 seconds
    if (alarm_counter > 5 * SAMPLE_RATE) {
        alarm_disabled = false;
    } else {
        alarm_counter++;
    }

    last_first_max++;
}
