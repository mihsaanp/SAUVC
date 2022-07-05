#include <Servo.h>
Servo FR;
Servo FL;
Servo BR;
Servo BL;
Servo CR;
Servo CL;

void setup() {
  // put your setup code here, to run once:
  FR.attach(8,1000,2000);
  FL.attach(9,1000,2000);
  BR.attach(10,1000,2000);
  BL.attach(11,1000,2000);
  CR.attach(12,1000,2000);
  CL.attach(13,1000,2000);
  FR.writeMicroseconds(1500);
  FL.writeMicroseconds(1500);
  BR.writeMicroseconds(1500);
  BL.writeMicroseconds(1500);
  CR.writeMicroseconds(1500);
  CL.writeMicroseconds(1500);
  delay(1000);
}

void loop() {
  // put your main code here, to run repeatedly:
  FR.writeMicroseconds(1500);
  FL.writeMicroseconds(1500);
  BR.writeMicroseconds(1500);
  BL.writeMicroseconds(1500);
  CR.writeMicroseconds(1500);
  CL.writeMicroseconds(1500);
}
