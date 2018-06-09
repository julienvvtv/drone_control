#include<Servo.h>

#include "Motor.h"

int moyenne[5];
int canal1_max, canal2_max, canal3_max, canal4_max, canal1_min, canal2_min, canal3_min, canal4_min;

extern int receiver_input[5];

int esc[5], temp[5]; 

void setup(){
  Serial.begin(9600);
  setupISR();
  setupMotor();
  calibration();
}

void loop(){
  
  temp[1]=receiver_input[1]-moyenne[1];
  temp[2]=receiver_input[2]-moyenne[2];
  temp[3]=-(receiver_input[3]-canal3_min);
  temp[4]=receiver_input[4]-moyenne[4];  

  esc[1]=temp[3]+temp[1]-temp[4]+temp[2]; //FR    1 Yaw
  esc[2]=temp[3]-temp[1]+temp[4]+temp[2]; //RR    3 Throttle
  esc[3]=temp[3]+temp[1]+temp[4]-temp[2]; //RL    4 Roll
  esc[4]=temp[3]-temp[1]-temp[4]-temp[2]; //FL    2 Pitch

  test();

  esc[1]=map(esc[1],ESCMin,ESCMax,PWMin,PWMax);
  esc[2]=map(esc[2],ESCMin,ESCMax,PWMin,PWMax);
  esc[3]=map(esc[3],ESCMin,ESCMax,PWMin,PWMax);
  esc[4]=map(esc[4],ESCMin,ESCMax,PWMin,PWMax);

  loopMotor();
}


