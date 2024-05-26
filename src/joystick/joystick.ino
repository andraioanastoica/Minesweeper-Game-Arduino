#define joyX A0
#define joyY A1
 
int xMap, yMap, xValue, yValue;
 
void setup() {
  Serial.begin(115200);
}
 
void loop() {
  // put your main code here, to run repeatedly:
  xValue = analogRead(joyX);
  yValue = analogRead(joyY);
  xMap = map(xValue, 0,1023, 0, 7);
  yMap = map(yValue,0,1023,7,0);
 
  Serial.print(xValue);
  Serial.print("\t");
  Serial.println(yValue);
}