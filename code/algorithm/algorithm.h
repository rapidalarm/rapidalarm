// ----- Types -----

// alarm types
typedef enum {
    // no alarm
    ALARM_NONE,
    // ventilator not cycling
    ALARM_NC,
    // PEEP below threshold
    ALARM_LP,
    // PIP above threshold
    ALARM_HP,
    // respiratory rate below threshold
    ALARM_LR,
    // respiratory rate above threshold
    ALARM_HR
} alarm_t;

// ----- Algorithm State -----

// number of samples since last PEEP attack sample
uint16_t last_peep = 0;
float last_peep_value = 10.0;
// number of samples since last PIP attack sample
uint16_t last_pip = 0;
float last_pip_value = 25.0;
// disable alarm for N seconds when system turned on
bool alarm_disabled = true;
uint16_t alarm_counter = 0;
bool breath_state = false;
uint16_t time_since_last_breath = 0;
float avg_breath_time = 400.0; // samples
// recursive filter max/min pressure readings
float p_max = 25;
float p_min = 10;

// ---- Outputs -----
// whether there is an alarm condition
alarm_t alarm_raised = ALARM_NONE;
// display code for raised alarm
char *alarm_code = "  ";
float alarm_value = 0;
// PIP and PEEP
float display_pip = 25;
float display_peep = 10;
// breath respiration_rate (breaths/min)
uint16_t respiration_rate = 15;

// ----- Config -----

const float ATTACK_COEFF = .9;
const float RELEASE_COEFF = .999;
const float DISPLAY_SMOOTH = 0.5;
const float ALARM_RATIO = 1.3;
const float ALARM_DIFF = 3;

// alarm condition thresholds
uint16_t THRESH_NC = 10; // s
uint16_t THRESH_LP = 2; // cm H20
uint16_t THRESH_HP = 40; // cm H20
uint16_t THRESH_LR = 6; // breaths/min
uint16_t THRESH_HR = 35; // breaths/min

// ----- Functions -----

// take in single sample at SAMPLE_RATE and update algorithm state
void run_algorithm(float p) {
    uint16_t new_breath_time;

    time_since_last_breath++;

    // if current sample is PIP attack
    if (p > p_max) {
        p_max = p_max * ATTACK_COEFF + p * (1 - ATTACK_COEFF);
        last_pip = 0;
        last_pip_value = p;
        if (!breath_state) {
            // Inhalation state
            breath_state = true;

            // Update displayed PEEP based on last peak
            display_peep = DISPLAY_SMOOTH*display_peep + (1-DISPLAY_SMOOTH)*last_peep_value;
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
        if (breath_state && (time_since_last_breath > 1)) {
          // Exhalation state
          breath_state = false;
          
          // Update displayed PIP based on previous max value
          display_pip = DISPLAY_SMOOTH*display_pip + (1-DISPLAY_SMOOTH)*last_pip_value;

          // Calculate breathing rate
          new_breath_time = time_since_last_breath - last_pip;
          time_since_last_breath = last_pip;
          if (new_breath_time > (SAMPLE_RATE*THRESH_NC)) { // Cap breath time to keep average in reasonable range when non-cycling
            new_breath_time = SAMPLE_RATE*THRESH_NC; // min(new_breath_time,max allowable breath time)
          }
          avg_breath_time = DISPLAY_SMOOTH*avg_breath_time + (1-DISPLAY_SMOOTH)*new_breath_time;
          respiration_rate = 60*SAMPLE_RATE/avg_breath_time;
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
            last_peep > SAMPLE_RATE * THRESH_NC
            ){
            alarm_raised = ALARM_NC;
            alarm_code = "nc";
            alarm_value = (SAMPLE_RATE * THRESH_NC) / 1000;
        }
        else if (p_max < (p_min * ALARM_RATIO)){
             alarm_raised = ALARM_NC;
             alarm_code = "nc";
             alarm_value = 11;
         }
        else if ((p_max - p_min) < ALARM_DIFF){
             alarm_raised = ALARM_NC;
             alarm_code = "nc";
             alarm_value = 11;
        }
        // low-pressure alarm
        else if (p_min < THRESH_LP){
            alarm_raised = ALARM_LP;
            alarm_code = "LP";
            alarm_value = p_min;
        }
        else if (p_max > THRESH_HP){
            alarm_raised = ALARM_HP;
            alarm_code = "HP";
            alarm_value = p_max;
        }
        else if (respiration_rate > THRESH_HR) {
            alarm_raised = ALARM_HR;
            alarm_code = "Hr";
            alarm_value = respiration_rate;
        }
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