void setup(){
    pinMode("A6", INPUT);
    Serial.begin(9600);
}

void loop(){
    Serial.print(analogRead(6));
    Serial.print("\t");
    Serial.println(analogRead(7));
}
