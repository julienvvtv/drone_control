#include<Servo.h>

#include "Motor.h"

int moyenne[5];
int canal1_max, canal2_max, canal3_max, canal4_max, canal1_min, canal2_min, canal3_min, canal4_min, pin=8;

int receiver_input[5];

int esc[5], temp[5]; 

void setup(){
  Serial.begin(9600);
  setupMotor();
  pinMode(pin,INPUT);
  calibration();
}

void loop(){

  if(pulseIn(pin,HIGH)>8000){
      for (int i=1;i<5;i++){
        receiver_input[i]=pulseIn(pin,HIGH);
        receiver_input[i]=map(receiver_input[i],550,1400,PWMin,PWMax);
        Serial.print(receiver_input[i]);
        Serial.print("  ");
      }
      Serial.println("");
   }
  
  temp[1]=receiver_input[1]-moyenne[1];
  temp[2]=receiver_input[2]-moyenne[2];
  temp[3]=-(receiver_input[3]-canal3_min);
  temp[4]=receiver_input[4]-moyenne[4];  

  esc[1]=temp[3]+temp[1]-temp[4]+temp[2];
  esc[2]=temp[3]-temp[1]+temp[4]+temp[2];
  esc[3]=temp[3]+temp[1]+temp[4]-temp[2];
  esc[4]=temp[3]-temp[1]-temp[4]-temp[2];

  test();

  for (int i=1;i<5;i++){
    esc[i]=map(esc[i],ESCMin,ESCMax,PWMin,PWMax);
  }

  loopMotor();
}
