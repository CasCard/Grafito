// limit switch test code

const byte limitSwitchPin = 15;

void setup() {
  pinMode(limitSwitchPin, INPUT_PULLUP);
  Serial.begin(9600);
}

void loop() {
  Serial.println(digitalRead(limitSwitchPin));
  delay(100);
}