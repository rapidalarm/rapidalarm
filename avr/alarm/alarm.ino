#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>

#define SAMPLE_RATE 100
// period at which to sample ADCs
#define SAMPLE_TIME 1.0 / SAMPLE_RATE * 1000

// convert cm H20 to ADC reading
#define CM2ADC(x) (uint16_t)(((x / 10.197 * 0.09) + 0.04) * ADC_LEVELS)

#define NOMINAL_RATIO 2.4
#define ALARM_TIME 10
/* #define ALARM_MAX 493 */
uint32_t ALARM_MAX = 583;
uint32_t ALARM_MIN = 80;
#define ALARM_RATIO 1.5
#define ALARM_DIFF 86

// ADC values
#define ADC_LEVELS 1024

const uint32_t COEFF_MAX = 1001;
const uint32_t ATTACK_COEFF = 1;
const uint32_t RELEASE_COEFF = 1000;
/* const uint16_t ATTACK_COEFF = 1; */
/* const uint16_t RELEASE_COEFF = 62; */
const uint16_t ALARM_SAMPLES = SAMPLE_RATE * ALARM_TIME;
const uint16_t display_counter_max = 8 * SAMPLE_RATE;



/* Global variables for seven segment display */
const byte charSegmentMap[] = {
  0b11111100, //0
  0b01100000, //1
  0b11011010, //2
  0b11110010, //3
  0b01100110, //4
  0b10110110, //5
  0b10111110, //6
  0b11100000, //7
  0b11111110, //8
  0b11100110, //9
  0b00000001, //.
  0b00000010, // -
  0b00000000, // space, may want to add letters
  0b11001110, // P
  0b10011111, // E
  0b00001101, // I
  0b00001010, // r
  0b00000001, // r
};

const byte digitPins[] = {A4, A2, 11, 10};
const byte segmentPins[] = {6, 12, 7, A1, A3, 8, 9, 5};
/* displayedValue should not begin with a .
 * It should have at most five characters
 */
char displayedValue[] = "--.--";

byte currentDisplayIndex = 0;
byte currentStringIndex = 0;
unsigned long nextDisplayUpdate = 0;
void UpdateDisplay() {
  unsigned long now = millis();
  if(now < nextDisplayUpdate) {
    return;
  }
  nextDisplayUpdate = 3 + now;
  digitalWrite(digitPins[currentDisplayIndex], LOW);
  currentDisplayIndex++;
  currentStringIndex++;
  if(currentDisplayIndex >= sizeof(digitPins)) {
    currentDisplayIndex = currentStringIndex = 0;
  }
  byte dp = (displayedValue[currentStringIndex+1] == '.') & 1;
  byte segmentMapIndex;
  switch(displayedValue[currentStringIndex]) {
    case ' ':     segmentMapIndex = 12;
                  break;
    case '-':     segmentMapIndex = 11;
                  break;
    case 'P':     segmentMapIndex = 13;
                  break;
    case 'E':     segmentMapIndex = 14;
                  break;
    case 'I':     segmentMapIndex = 15;
                  break;
    case 'r':     segmentMapIndex = 16;
                  break;
    case '.':     segmentMapIndex = 17;
                  break;
    default:      segmentMapIndex = displayedValue[currentStringIndex] - '0';
  }
  for(byte n = 0; n < sizeof(segmentPins); n++) {
    digitalWrite(segmentPins[n], !((charSegmentMap[segmentMapIndex] | dp) & (128 >> n) ) );
  }
  digitalWrite(digitPins[currentDisplayIndex], HIGH);
  if(dp) currentStringIndex++;
}


void setup() {
    Serial.begin(115200);
    pinMode(A6, INPUT);
    pinMode(A7, INPUT);
    pinMode(3, OUTPUT);
    analogWrite(3, 0);
    TCCR2B = TCCR2B & B11111000 | B00000010;
    for(byte i = 0; i < sizeof(digitPins); i++) {
        pinMode(digitPins[i], OUTPUT);
    }
    for(byte i = 0; i < sizeof(segmentPins); i++) {
        pinMode(segmentPins[i], OUTPUT);
    }

}


void loop() {

    // number of samples since last min/max
    static uint16_t last_min=0, last_max=0;

    static int alarm_raised = 0;
    static bool alarm_disabled = true;
    static int t = 0;

    // whether data is available from ADC
    static uint32_t last_sample = millis();

    bool reached_min = false;
    uint32_t last_first_max = 0;
    uint32_t display_counter = 0;

    // current pressure readings
    uint32_t p1;
    uint32_t p2;
    // breath period
    uint32_t period = 0;
    // recursive filter max/min pressure readings
    static uint32_t p_max = 0;
    static uint32_t p_min = UINT32_MAX;

    while (true) {
        if (millis() - last_sample >= SAMPLE_TIME) {
            last_sample = millis();
            p1 = 256 * (uint32_t) analogRead(6);
            p2 = 256 * (uint32_t) analogRead(7);
            if (p1 > p_max) {
                p_max = (uint32_t)(
                    (
                        (uint32_t)p_max * ATTACK_COEFF +
                        (uint32_t)p1 * (COEFF_MAX - ATTACK_COEFF)
                    ) / COEFF_MAX
                );
                last_max = 0;
                if (reached_min) {
                    period = last_first_max;
                    last_first_max = 0;
                    reached_min = false;
                }
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
                reached_min = true;
            }
            else {
                p_min = (RELEASE_COEFF * p_min + (COEFF_MAX - RELEASE_COEFF) * p1) / COEFF_MAX;
                last_min++;
            }

            // ----- Alarm conditions -----
            if (!alarm_disabled) {
                if (last_max > ALARM_SAMPLES){
                    alarm_raised = 1;
                }
                if (last_min > ALARM_SAMPLES){
                    alarm_raised = 2;
                }
                if (p_max < (p_min * ALARM_RATIO)){
                    alarm_raised = 3;
                }
                if ((p_max - p_min) < 256 * ALARM_DIFF){
                    alarm_raised = 4;
                }
                if (p_max > 256 * ALARM_MAX){
                    alarm_raised = 5;
                }
                if (p_min < 256 * ALARM_MIN){
                    alarm_raised = 6;
                }
                if(alarm_raised) analogWrite(3, 50);
            }
            if (t > 5 * SAMPLE_RATE) {
                alarm_disabled = false;
            } else {
            t++;
            }
            UpdateDisplay();
            Serial.print(period);
            Serial.print("\t");
            Serial.println(alarm_raised);
            /* printf("%d %d %d %d\n", p1 / 256, p_max / 256, p_min / 256, 100 * alarm_raised); */
            /* sprintf(displayedValue, "PE%2d", p_max / 2568); */

            if (display_counter < display_counter_max / 3) {
                // PIP
                sprintf(displayedValue, "PI%2d", p_max / 2568);
            } else if (display_counter < 2 * display_counter_max / 3) {
                // PEEP
                sprintf(displayedValue, "PE%2d", p_min / 2568);
            } else {
                // rate
                sprintf(displayedValue, "r %2d", (60 * SAMPLE_RATE) / period);
            }
            // breaths per minute
            display_counter++;
            display_counter %= display_counter_max;

            last_first_max++;
        }

        UpdateDisplay();
    }
}
