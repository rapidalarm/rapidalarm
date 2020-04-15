/* displayedValue should not begin with a .
 * It should have at most five characters
 */
char displayedValue[] = "--.--";

byte display_index = 0;
byte string_index = 0;
unsigned long nextDisplayUpdate = 0;

// map character to 7 segment
const byte digit_map[] = {
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
  0b00000000, //.
  0b00000010, // -
  0b00000000, // space, may want to add letters
  0b11001110, // P
  0b10011110, // E
  0b00001100, // I
  0b00001010, // r
  0b00000000, // .
  0b00011100, // L
  0b01101110, // H
  0b11000000, // :
};

// cycle through display digits
void update_display() {

  unsigned long now = millis();
  if(now < nextDisplayUpdate) {
    return;
  }
  nextDisplayUpdate = 4 + now;
  digitalWrite(digit_pins[display_index], LOW);
  display_index++;
  string_index++;
  if(display_index >= sizeof(digit_pins)) {
    display_index = string_index = 0;
  }
  byte dp = (displayedValue[string_index+1] == '.') & 1;
  byte segmentMapIndex;
  switch(displayedValue[string_index]) {
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
    case 'L':     segmentMapIndex = 18;
        break;
    case 'H':     segmentMapIndex = 19;
        break;
    case ':':     segmentMapIndex = 20;
        break;
    default:      segmentMapIndex = displayedValue[string_index] - '0';
  }
  for(byte n = 0; n < sizeof(segment_pins); n++) {
    digitalWrite(segment_pins[n], !((digit_map[segmentMapIndex] | dp) & (128 >> n) ) );
  }
  digitalWrite(digit_pins[display_index], HIGH);
  if(dp) string_index++;
}
