#include "Motor.h"

void test(){
  for(int i=1;i<5;i++){
    if(esc[i]<ESCMin) esc[i]=ESCMin;
    else if(esc[i]>ESCMax) esc[i]=ESCMax;
  }
}


