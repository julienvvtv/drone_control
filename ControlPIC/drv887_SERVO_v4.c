

#ifndef _drv887_PWM_4Servo_new3_C
#define _drv887_PWM_4Servo_new3_C

#DEFINE servo_out1(x) output_bit(PIN_D2,x)		// sortie par défaut
#DEFINE servo_out2(x) output_bit(PIN_D3,x)
#DEFINE servo_out3(x) output_bit(PIN_D4,x)
#DEFINE servo_out4(x) output_bit(PIN_D5,x)

#define SPEED_MAX 100

/******************************************************************** 
Pour chaque moteur x pendant 3ms:
	Sortie à 1 pendant 1ms+speed*10 pour le moteur x
	Sortie à 0 pendant 2ms-speed*10 pour le moteur x
 Aller vers le début du premier moteur à : 20ms - 4*3ms = 8ms

La vitesse va de 0 à 100 donc pour un intervalle de 1ms il faut multiplier par 10 (pour faire de 0ms à 1ms).
Puisque j'ai fait 4 moteurs et 3ms par moteur, je dois recommencer après 8ms
 
Clock: 20MHz
Instruction cycle: 5MHz = 20MHz/4
Precision:	1ms/100=10us		Counter Size:	65536
setup_timer_1(T1_INTERNAL | T1_DIV_BY_1);			Res:	0,2
min 10us=>	10	50	us real:	10,0
set_timer1(65536-50);		65486		
min 1ms=>	1000	5000	us real:	1000,0
set_timer1(65536-5000);		60536		
min + 10us=>	1010	5050	us real:	1010,0
set_timer1(65536-5050);		60486		
max 2ms=>	2000	10000	us real:	2000,0
set_timer1(65536-10000);		55536		
max - 10us=>	1990	9950	us real:	1990,0
set_timer1(65536-9950);		55586		
long jump 8ms=>	8000	4000	us real:	8000,0
set_timer1(65536-4000);		61536		
********************************************************************/
 
static byte step = 0;
static byte motor = 0;
byte const Nmotors = 1; 

int16 speed[Nmotors]; 
int16 set_on[Nmotors]; 
int16 set_off[Nmotors]; 
byte const pin[Nmotors] = {PIN_D2};
//,PIN_D3,PIN_D4,PIN_D5};

#define high 1
#define low 0 
#define TIMER1_COUNTER_SZ	65536
#define TIMER1_RES_US		0.2
/* Les limites maximum pour le servo moteur du type TowerPro SG92R sont:
	-90° (0°)  	-> 600 us
	  0° (90°) 	-> 1500 us
	+90° (180°)	-> 2400 us
*/
#define JUMP_ON(_m) 		TIMER1_COUNTER_SZ - (int16)((600 + speed[_m]*10)/ TIMER1_RES_US)	//980   
#define JUMP_OFF(_m) 		TIMER1_COUNTER_SZ - (int16)((2400 - speed[_m]*10)/ TIMER1_RES_US)	//1980
int16 const Jump_long =	TIMER1_COUNTER_SZ - (int16)((19980 - Nmotors*3500)/ TIMER1_RES_US);		//19980


#int_timer1
void servo_isr() {

	switch(step) {
		case 0: // ON state
			// jump to end of ON  |-1ms-|-speed-|	
			// set motor pin ON
			// go to next state
			set_timer1(set_on[motor]);
			output_bit(pin[motor],high); 
			step = 1;		
			break;
		case 1: // OFF state
			// jump to end of 3ms interval |-speed-|-1ms-|
			// set off current motor
			set_timer1(set_off[motor]);
			output_bit(pin[motor],low);
			// update motor number (do it for the next motor)
			motor=(motor+1)%Nmotors;
			if (motor==0) {
				// go to jump to end of period state
				step = 2;
			}					
			else {
				// go to set ON state for next motor
				step = 0;						
			} 	
			break;
		case 2:  // long jump to end of period
			// jump to end of period (20ms) => 20000us - 4 * 3000us = 8000us 
			// interval: 8000us/0,2us = 40000
			// set: 65536-40000 = 25536
			set_timer1(Jump_long); 
			// go to state start
			step = 0;
			break;
		default:
		break;
	}
}

void servo_pos(byte nm, byte sp) {

	speed[nm]=sp;  // zero speed
	//speed comprise entre 125 et 331.25

	set_on[nm]=JUMP_ON(nm);
	set_off[nm]=JUMP_OFF(nm);
}

void init_servo_tim() {

	setup_timer_1(T1_INTERNAL|T1_DIV_BY_1);			
	set_timer1(0);
	output_bit(pin[0],low); 
	/*output_bit(pin[1],low); 
	output_bit(pin[2],low); 
	output_bit(pin[3],low);*/
	
	step = 0; 			// initial step
	servo_pos(0,0);		// initialise all motor
	/*servo_pos(1,0);
	servo_pos(2,0);
	servo_pos(3,0);*/

	enable_interrupts(INT_TIMER1);					// Activé interruption du Timer 1
	//enable_interrupts(GLOBAL);						// Active interruption general
}

#endif
