//-----------------------------------------------------
//------------- Programme Decodage PPM ----------------
//-----------------------------------------------------
//-- Projet: Decodage PPM          	                 --
//-- Date:   26/10/2017                              --
//-- Progr:  decod_PPM_4550							 --
//-- Auteurs: Daniel Binon              			 --
//-----------------------------------------------------
//-- Lecture des 10 canaux PPM d'un recepteur du     --
//-- type R6DS (Radiolink). Donne le temps en us	 --
//-- de chaque canal du recepteur.				     --
//-- Les 4 premiers canaux sont affiches sur le LCD  --
//-- La valeurs des 10 canaux est envoye sur le port --
//-- serie.    			         				     --
//-- Dans cet example, il y a deux possibilites de   --
//-- mesure, soit avec int. sur entree CPP1, soit    --
//-- avec int. sur entree EXT2.					     --
//--           				       				     --
//-- 											     --
//--         			         				     --
//----------------------------------------------------- 
#include "887_sv3_std.h"
#include <string.h>
#include <stdlib.h>
#include "drv887_SERVO_v4.c"
#include "LCD420_S3.c"


#use rs232(baud=115200,parity=N,xmit=PIN_C6,rcv=PIN_C7,bits=8,stream=RS232)


//-------------------------E/S------------------------
#define pwm_out1_on		output_high(PIN_C1)		// moteur 1 sens direct
#define pwm_out1_off	output_low(PIN_C1)
#define pwm_out2_on		output_high(PIN_D1)		// Moteur 1 sens inverse
#define pwm_out2_off	output_low(PIN_D1)
#define pwm_out3_on		output_high(PIN_C2)		// moteur 2 sens direct
#define pwm_out3_off	output_low(PIN_C2)
#define pwm_out4_on		output_high(PIN_D2)		// Moteur 2 sens inverse
#define pwm_out4_off	output_low(PIN_D2)
byte pt,ptc,dc1,dc2,dc3,dc4;
#define cycle_t0 0xFF36   						// réglage du timer pour 50 µsec
#define pwm1 100		 						// Nombre d'itération dans l'interruption (50 µsec x 100 = 5 msec = 200 Hz)

#define pulse_low 625 // partie a retirer - pulse_low = 500us -> 500us / 0.8us (avant 0.664us) (Timer1/4) = 625 (avant 753)
signed int32 pulse_width;
signed int16 tbl[11] = {0,0,0,0,0,0,0,0,0,0 };			// Table pour les valeurs des 10 canaux
signed int16 moteur_gauche, moteur_droit;
signed int16 ppmc[11];
signed int32 first,second; 
signed byte cp;

#INT_EXT								// Interruption sur entree INT
void ext_isr(){
	first = get_timer1();				// lecture du temps sur le 1er flanc descendant
	pulse_width = first-second;			// mesure du temps en deux flancs descendant consecutif 
	cp++;								// Canal suivant
	if (pulse_width < 3125){			// Detection interval entre deux séries (synchonisation) > 2.5ms / 0.8us (avant 0.664us) = 3125 (avant 3765) 
		tbl[cp-1] = (pulse_width - pulse_low + 12) * 0.8; // (avant 0.664)	  // transformation en us (12-> nbr d'instruction de l'int.)
		ppmc[cp-1]=(tbl[cp-1]-1000);
		second = first;					// mesure du temps de l'interval
	}else{								// si le temps long (entre deux series) -> synchronisation
		cp=0;							// compteur remis a 0
		second = 0;						// init des valeurs
		set_timer1(0);					// relancer le timer 0
	}
}

#int_timer0
void pwm_isr() {
    set_timer0(cycle_t0);                		// adjust timer0 pour 50 µsec
	if (pt == dc1) {							// compteur = pointeur PWM Moteur
		pwm_out1_off;
	}
 	if (pt == dc2) {
		pwm_out2_off;
	}
 	if (pt == dc3) {
		pwm_out3_off;
	}
 	if (pt == dc4) {
		pwm_out4_off;
	}
	if (pt == pwm1) {
		if (dc1!=0) pwm_out1_on;
		if (dc2!=0) pwm_out2_on;
		if (dc3!=0) pwm_out3_on;
		if (dc4!=0) pwm_out4_on;
		pt=255;									// remise à zero du pointeur (instruc. suiv. = pt++)
		ptc++;								
	}
	if (ptc == 20) {
		ptc=0;									// remise à zéro du pointeur de detection d'obstacle
	}										
	pt++;
}


void motors(signed byte speedd,signed byte speedg) {
	if (speedg >= 0){					
		 dc1=0;									// direction moteur gauche = avant
		 dc2=(speedg * 2);						// reglage de la vitesse
	}
	else {
		dc2=0;									// direction moteur gauche = arriere
		dc1= (~speedg+1)*2;						// reglage de la vitesse absolue (complement a 2 pour enlever le signe moins)
	}
	if (speedd >= 0){
		 dc3=0;									// direction moteur droit = avant
		 dc4=(speedd * 2);						// reglage de la vitesse
	}
	else {
		dc4=0;									// direction moteur droit = arriere
		dc3= (~speedd+1)*2;						// reglage de la vitesse absolue (complement a 2 pour enlever le signe moins)
	}
}

void initialisation() {
	setup_timer_1(T1_INTERNAL|T1_DIV_BY_4);		// Reglage timer 1 sur une résolution de 5MHz / 4 -> 0.8us (avant 0,664 us)
	setup_timer_0(RTCC_INTERNAL);
	//init_servo_tim();
	lcd_init();									// init LCD
	cursor(0);
	lcd_putc("\fMesure PPM");
	lcd_putc("\n(C) UMONS - 2017");
	delay_ms(1500);
	cp=0;
	second=0;								// init variable
	enable_interrupts(INT_TIMER0);
	enable_interrupts(INT_EXT_H2L);			// Regle interruption INT EXT2 sur flanc montant
   	enable_interrupts(GLOBAL);					// autorise les interruptions
}

void display(){
	lcd_gotoxy(4,1);
	printf(lcd_putc,"%4Lu",ppmc[0]);
	lcd_gotoxy(12,1);
	printf(lcd_putc,"%4Lu",ppmc[1]);
	lcd_gotoxy(4,2);
	printf(lcd_putc,"%4Lu",ppmc[2]);
	lcd_gotoxy(12,2);
	printf(lcd_putc,"%4Lu",ppmc[3]);
	for(int i=0;i<5;i++){
		printf("%Lu ",ppmc[i]);
	}
	printf("\n\r");
		delay_ms(200);
}

void main(){

	initialisation();
	lcd_putc("\fC1:     C2:     ");
    lcd_putc("\nC3:     C4:     ");
   	while(TRUE) {
		moteur_droit=-ppmc[2]-ppmc[3];
		moteur_gauche=-ppmc[2]+ppmc[3];
		moteur_droit=moteur_droit/4;
		moteur_gauche=moteur_gauche/4;
		motors(moteur_droit,moteur_gauche);
		//printf("%5ld %5ld %5ld %5ld %5ld %5ld\n",tbl[2],tbl[3], ppmc[2], ppmc[3], moteur_gauche, moteur_droit);
		//delay_ms(100);
	}
}