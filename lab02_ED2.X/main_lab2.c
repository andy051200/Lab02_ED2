/*------------------------------------------------------------------------------
Archivo: mainsproject.s
Microcontrolador: PIC16F887
Autor: Andy Bonilla
Compilador: pic-as (v2.30), MPLABX v5.45
    
Programa: laboratorio 1
Hardware: PIC16F887
    
Creado: 16 de julio de 2021    
Descripcion: 
------------------------------------------------------------------------------*/

// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT   //configuracion de oscilador interno
#pragma config WDTE = OFF       // Watchdog Timer Enable bit
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // master clear off 
#pragma config CP = OFF         // Code Protection bit off
#pragma config CPD = OFF        // Data Code Protection bit off
#pragma config BOREN = OFF      // Brown Out Reset Selection bits off
#pragma config IESO = OFF       // Internal External Switchover bit off
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit off 
#pragma config LVP = OFF        // Low Voltage Programming Enable bit off 

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit 
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits 

/*-----------------------------------------------------------------------------
 ----------------------------LIBRERIAS-----------------------------------------
 -----------------------------------------------------------------------------*/
#include <xc.h>                 //se incluye libreria del compilador
#include <stdint.h>             //se incluye libreria
#include <pic16f887.h>          //se incluye libreria del pic
#include "adc_config.h"         //se incluye libreria del adc
//#include "multiplexada.h"       //se incluye libreria de multiplexada*/

/*-----------------------------------------------------------------------------
 ------------------------DIRECTIVAS DE COMPILADOR------------------------------
 -----------------------------------------------------------------------------*/
#define _XTAL_FREQ 8000000

/*-----------------------------------------------------------------------------
 ------------------------ PROTOTIPOS DE FUNCIONES ------------------------------
 -----------------------------------------------------------------------------*/
void setup(void);       //prototipo de funcion de configuracion
void toggle_adc(void);  //prototipo de funcion de toggle de canales ADC

/*-----------------------------------------------------------------------------
 ----------------------- VARIABLES A IMPLEMTENTAR------------------------------
 -----------------------------------------------------------------------------*/
unsigned char conversion1;  //variable que almacena potenciometro 1
unsigned char conversion2;  //variable que almacena potenciometro 2

/*-----------------------------------------------------------------------------
 ---------------------------- INTERRUPCIONES ----------------------------------
 -----------------------------------------------------------------------------*/
void __interrupt() isr(void) //funcion de interrupciones
{
    //------interrupcion 
    
   
}

/*-----------------------------------------------------------------------------
 ----------------------------- MAIN LOOP --------------------------------------
 -----------------------------------------------------------------------------*/
void main(void)
{
    setup();
        
    while(1)
    {
        
        
    }
}
/*-----------------------------------------------------------------------------
 ---------------------------------- SET UP -----------------------------------
 -----------------------------------------------------------------------------*/
void setup(void)
{
    //CONFIGURACION DE ENTRDAS ANALOGICAS
    ANSEL=0;                //sin entradas analógicas
    ANSELH=0;               //sin entradas analogicas
    ANSELbits.ANS0=1;       //An0 como entrada analogica, potenciometro
    
    //CONFIGURACION DE IN-OUT DE PUERTOS
    TRISAbits.TRISA0=1;     //AN0 como entrada de potanciometro
    TRISAbits.TRISA0=0;     //AN1 como salida de led de alerta
    
    TRISBbits.TRISB0=1;     //RB0 como entrada, boton SUMA
    TRISBbits.TRISB1=1;     //RB1 como entrada, boton RESTA

    TRISC=0;                //PortC como salida, contador LEDS
    TRISD=0;                //PortD como salida, 7 segmentos
    TRISE=0;                //PortE como salida, multiplexada 7 segmentos
    
    PORTA=0;             //se limpia PortA
    PORTB=0;             //se limpia PortA
    PORTC=0;             //se limpia PortE
    PORTD=0;             //se limpia PortE
    PORTE=0;             //se limpia PortE
    
    //CONFIGURACION DE RELOJ
    OSCCONbits.IRCF = 0b111; //Fosc 8MHz
    OSCCONbits.SCS = 1;      //configuracion de reloj interno
    
    //CONFIGURACION DEL TIMER0
    OPTION_REGbits.T0CS = 0;  // bit 5  TMR0 Clock Source Select bit...0 = Internal Clock (CLKO) 1 = Transition on T0CKI pin
    OPTION_REGbits.T0SE = 0;  // bit 4 TMR0 Source Edge Select bit 0 = low/high 1 = high/low
    OPTION_REGbits.PSA = 0;   // bit 3  Prescaler Assignment bit...0 = Prescaler is assigned to the Timer0
    OPTION_REGbits.PS2 = 1;   // bits 2-0  PS2:PS0: Prescaler Rate Select bits
    OPTION_REGbits.PS1 = 1;
    OPTION_REGbits.PS0 = 0;
    TMR0 = 255;             // preset for timer register

    //WEAK PULL UPs PORTB
    OPTION_REGbits.nRBPU = 0;   // enable Individual pull-ups
    WPUBbits.WPUB0 = 1;         // enable Pull-Up de RB0 
    WPUBbits.WPUB1 = 1;         // enable Pull-Up de RB1 
    WPUBbits.WPUB2 = 1;         // enable Pull-Up de RB2 

    //IMPORTAR FUNCION DEL ADC DESDE LIBRERIA
    adc_config();               //se llama funcion
    
    //CONFIGURACION DE INTERRUPCIONES
    INTCONbits.GIE=1;           //se habilitan las interrupciones globales
    INTCONbits.T0IE=1;          // enable bit de int timer0
    INTCONbits.T0IF=0;        //se apaga la bandera de int timer0
    INTCONbits.RBIE=1;          // se habilita IntOnChange
    INTCONbits.RBIF=0;          // se apaga la bandera de IntOnChangeB  
    //CONFIGURACION INTERRUPCION DEL ADC
    PIE1bits.ADIE = 1 ; //se prende interrupcion por ADC
    PIR1bits.ADIF = 0; // se baja bandera de conversion
    //INTERRUPT ON CHANGE PORT B
    IOCBbits.IOCB0=1;           //se habilita IOCB RB0
    IOCBbits.IOCB1=1;           //se habilita IOCB RB1
    return;
}

/*-----------------------------------------------------------------------------
 --------------------------------- FUNCIONES ----------------------------------
 -----------------------------------------------------------------------------*/
void toggle_adc(void)
{
    if (ADCON0bits.GO==0)
    {
        switch(ADCON0bits.CHS)
        {
            case(0):
                conversion1=ADRESH;         //potenciometro 1
                __delay_us(100);            //delay para cargar capacitor          
                ADCON0bits.CHS=1;           //switch de canal
                break;
                    
            case(1):
                conversion2=ADRESH;         //potenciometro 2
                __delay_us(100);            //delay para cargar capacitor
                ADCON0bits.CHS=0;           //switch de canal
                break;
            }
            __delay_us(100);                //delay para carga de capacitor
            ADCON0bits.GO=1;                //se inicia otra conversion ADC
        }
    return;
}