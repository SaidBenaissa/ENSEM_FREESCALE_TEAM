#include "mbed.h"
#include "TFC.h"
#include "Direction.h"

//This macro is to maintain compatibility with Codewarrior version of the sample.   This version uses the MBED libraries for serial port access
Serial PC(USBTX,USBRX);

#define TERMINAL_PRINTF     PC.printf
#define sensibilite 500

//This ticker code is used to maintain compability with the Codewarrior version of the sample.   This code uses an MBED Ticker for background timing.

#define NUM_TFC_TICKERS 4


#define VMAX 0.4
#define VMIN 0.2



Ticker TFC_TickerObj;
int old_indice;
volatile uint32_t TFC_Ticker[NUM_TFC_TICKERS];

void TFC_TickerUpdate()
{
    int i;

    for(i=0; i<NUM_TFC_TICKERS; i++) {
        if(TFC_Ticker[i]<0xFFFFFFFF) {
            TFC_Ticker[i]++;
        }
    }
}


int tab_min(void)
{
    double min=0;
    int indice=0;
    //min du tableau
    min=TFC_LineScanImage1[64];
    TERMINAL_PRINTF("\n---");
    for(int i=20; i<108; i++) {
        if(TFC_LineScanImage1[i]<sensibilite) {
            TERMINAL_PRINTF("0;");
        } else {
            TERMINAL_PRINTF("1;");
        }
        if(TFC_LineScanImage1[i]<=min) {
            min=TFC_LineScanImage1[i];
            indice=i;
        }
    }

    if(min<sensibilite) {
        old_indice=indice;
        return indice;
    } else {
        return indice;
    }

}



void TURN_LEFT(int value)
{
    TFC_Ticker[3]=0;
    float servo=0.33-0.00717*value;
    float vitesse1=((VMAX-VMIN)/0.33)*servo+VMIN;
    TFC_SetServo(0,servo);
    TFC_SetMotorPWM(vitesse1,vitesse1-0.2);

    while(TFC_Ticker[3]<100) {
    }

}
//appele de la fonction: Direction();

void TURN_RIGHT(int value)
{
    TFC_Ticker[3]=0;

    float servo=0.33+0.00925*value;
    float vitesse1=-((VMAX-VMIN)/0.37)*servo+VMAX;
    TFC_SetServo(0,servo);
    TFC_SetMotorPWM(vitesse1-0.2,vitesse1);

    while(TFC_Ticker[3]<100) {
        
    }
}

void FORWARD()
{
    TFC_SetServo(0,0.33);

}


void stop()
{
    TFC_SetServo(0,0.33);


}
int main()
{
    uint32_t t = 0;

    PC.baud(9600);
    TFC_TickerObj.attach_us(&TFC_TickerUpdate,2000);

    int state=0;
    TFC_Init();
    int old_indice=64;
    for(;;) {
        int indice=0;
        int erreur;

        if(TFC_PUSH_BUTTON_0_PRESSED) {
            state=1;

        }

        if(TFC_PUSH_BUTTON_1_PRESSED) {
            state=0;

        }

        switch(state) {
            case 0:
                //Let's put a pattern on the LEDs
                if(TFC_Ticker[1] >125) {
                    TFC_Ticker[1] = 0;

                    if(t==4) {
                        t=0;
                    } else
                        t=4;
                    TFC_SetBatteryLED_Level(t);
                    stop();
                }

                break;
            case 1:
                if(TFC_Ticker[0]>100 && TFC_LineScanImageReady>0) {
                    TFC_Ticker[0] = 0;
                    TFC_LineScanImageReady=0;
                   
                    //Let's put a pattern on the LEDs
                    if(t==0)
                        t=4;
                    else
                        t--;
                    TFC_SetBatteryLED_Level(t);

                    //traitment d'image de la camera
                    indice=tab_min();

                    TERMINAL_PRINTF("\n ----%D ----- \n",indice);
                    if(indice < old_indice+10 || indice > old_indice-10) {
                        if(indice>=64) {
                            erreur=indice-64;
                            TERMINAL_PRINTF("\n %D RIGHT \n",erreur);
                            TURN_RIGHT(erreur);


                        } else if(indice<64) {
                            erreur=64-indice;
                            TERMINAL_PRINTF("\n%D LEFT\n",erreur);

                            TURN_LEFT(erreur);
                        } 
                        old_indice=indice;
                    }
                }
                break;
        }
    }


}

