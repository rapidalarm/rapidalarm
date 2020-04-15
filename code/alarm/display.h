const byte digitPins[] = {A4, A2, 11, 10};
const byte segmentPins[] = {6, 12, 7, A1, A3, 8, 9, 5};
/* displayedValue should not begin with a .
 * It should have at most five characters
 */
char displayedValue[] = "--.--";

byte currentDisplayIndex = 0;
byte currentStringIndex = 0;
unsigned long nextDisplayUpdate = 0;

// map character to 7 segment
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

// constrain input to limits
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

// cycle through display digits
void update_display() {

  unsigned long now = millis();
  if(now < nextDisplayUpdate) {
    return;
  }
  nextDisplayUpdate = 4 + now;
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
