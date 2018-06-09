#include<Servo.h>

Servo servo[5];

void setupMotor(){
  servo[1].attach(4); //canal 1 Yaw
  servo[2].attach(5); //canal 2 Pitch
  servo[3].attach(6); //canal 3 Throttle
  servo[4].attach(7); //canal 4 Roll
  servo[1].writeMicroseconds(PWMin);
  servo[2].writeMicroseconds(PWMin);
  servo[3].writeMicroseconds(PWMin);
  servo[4].writeMicroseconds(PWMin);
  delay(2000);
}

void loopMotor() {
  for(int i=1;i<5;i++){
    Serial.print(esc[i]);
    Serial.print(" ");
    servo[i].writeMicroseconds(esc[i]);
  }
  Serial.println("");
}



