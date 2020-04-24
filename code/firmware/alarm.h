// ----- State Variables -----

// time of last debounced button press
unsigned long last_debounce_time = 0;

// state for pending button action. reset when handled
bool mode_pressed = false;
bool up_pressed = false;
bool down_pressed = false;

// time of last ADC sample
unsigned long last_sample = millis();
// adc pressure reading
float p;

// counter for cycling through displayed metrics and flashing display
uint16_t counter = 0;
// 8 second display cycle
const uint16_t counter_max = 8 * SAMPLE_RATE;

// user interface mode, cycled by mode button
typedef enum {
    // cycle through displayed metrics
    MODE_DISPLAY,
    // set alarm conditions
    MODE_SET_THRESH_LP,
    MODE_SET_THRESH_HP,
    MODE_SET_THRESH_LR,
    MODE_SET_THRESH_HR,
    MODE_SET_THRESH_NC,
    // display raised alarm
    MODE_ALARM
} mode_t;
// button cycles through first 5 modes only
const uint8_t total_modes = 5;
// current mode
uint8_t mode = MODE_DISPLAY;

// ----- Helper functions -----

// convert cm H20 to ADC reading
/* #define CM2ADC(x) ((x / 10.197 * 0.09) + 0.04) * 1023 */
// convert ADC reading to cm H20 to ADC reading
#define ADC2CM(x) (((x / 1023) - 0.04) / 0.09) * 10.197

// helper function to constrain float to limits
float limit(float x, float lo, float hi) {
    if (x > hi) {
        return hi;
    }
    else if (x < lo) {
        return lo;
    }
    else {
        return x;
    }
}

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

// pin interrupt setup
void pci_setup(byte pin)
{
    *digitalPinToPCMSK(pin) |= bit (digitalPinToPCMSKbit(pin));  // enable pin
    PCIFR  |= bit (digitalPinToPCICRbit(pin)); // clear any outstanding interrupt
    PCICR  |= bit (digitalPinToPCICRbit(pin)); // enable interrupt for the group
}
