/*------------------------------------------------------------------------------
Archivo: mainsproject.s
Microcontrolador: PIC16F887
Autor: Andy Bonilla
Compilador: pic-as (v2.30), MPLABX v5.45
    
Programa: laboratorio 2
Hardware: PIC16F887
    
Creado: 23 de julio de 2021    
Descripcion: un laboratoria bien fumado tbh pero chilero
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
#include "LCD.h"                //se incluye libreria de LCD
#include "uart_config.h"

/*-----------------------------------------------------------------------------
 ------------------------DIRECTIVAS DE COMPILADOR------------------------------
 -----------------------------------------------------------------------------*/
#define _XTAL_FREQ 8000000

/*-----------------------------------------------------------------------------
 ------------------------ PROTOTIPOS DE FUNCIONES ------------------------------
 -----------------------------------------------------------------------------*/
void setup(void);       //prototipo de funcion de configuracion
void toggle_adc(void);  //prototipo de funcion de toggle de canales ADC
void recepcion_uart(void);   //prototipo de funcion para recepcion uart
char datos_ascii(uint8_t numero);
void conversiones(void);
char* lcd_ascii();

/*-----------------------------------------------------------------------------
 ----------------------- VARIABLES A IMPLEMTENTAR------------------------------
 -----------------------------------------------------------------------------*/
unsigned char conversion1;  //variable que almacena potenciometro 1
unsigned char conversion2;  //variable que almacena potenciometro 2
unsigned char dato_recibido;    //variable para sumas y restas, uart
unsigned char cuenta_uart;  //variable para cuenta de suma y resta, uart

unsigned char centenas1;
unsigned char decenas1;
unsigned char unidades1;

unsigned char centenas2;
unsigned char decenas2;
unsigned char unidades2;

unsigned char centenas3;
unsigned char decenas3;
unsigned char unidades3;

unsigned char suma1;
unsigned char suma2;
unsigned char suma3;

/*-----------------------------------------------------------------------------
 ---------------------------- INTERRUPCIONES ----------------------------------
 -----------------------------------------------------------------------------*/
void __interrupt() isr(void) //funcion de interrupciones
{
    //------interrupcion por recepcion uart
     if(PIR1bits.RCIF)
    {
        RCREG=dato_recibido;    //se almacena dato recibido en variable
        PIR1bits.RCIF=0;
    }
   
}

/*-----------------------------------------------------------------------------
 ----------------------------- MAIN LOOP --------------------------------------
 -----------------------------------------------------------------------------*/
void main(void)
{
    setup();            //invoco la funcion de configuracion de registros
    lcd_init();         //invoco la funcion de inicializacion de la lcd
	cmd(0x90);          //invocao la funcion de configurcion de comandos lcd
	
    
    while(1)
    {
        //-------seccion para llamr varias funciones
        toggle_adc();               //se llama funcion para cambiar canales
        recepcion_uart();           //se llama funcion para cuenta uart
        conversiones();             //se llama funcion de conversiones
        //PORTD=conversion1;
        //PORTE=conversion2;
        
        //-------seccion para mandar texto al lcd
        lcd_linea(1,1);             //selecciono la linea 1 para escribir
        show(" Sen1 Sen2 Sen3 ");//mensaje a enviar linea 1
        lcd_linea(2,1);             //selecciono la linea 2 para escibrir
        show(lcd_ascii()); //mensaje a enviar linea 2
        
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
    ANSELbits.ANS0=1;       //An0 como entrada analogica, potenciometro 1
    ANSELbits.ANS1=1;       //An1 como entrada analogica, potenciometro 2
    
    //CONFIGURACION DE IN-OUT DE PUERTOS
    TRISB=0;
    //TRISC=0;
    /*TRISCbits.TRISC0=0;
    TRISCbits.TRISC1=0;
    TRISCbits.TRISC2=0; 
    */
    TRISDbits.TRISD5=0;
    TRISDbits.TRISD6=0;
    TRISDbits.TRISD7=0;
    TRISE=0;
    
    PORTB=0;
    //PORTC=0;
    PORTD=0;
    PORTE=0;
    
    //CONFIGURACION DE RELOJ
    OSCCONbits.IRCF = 0b111; //Fosc 8MHz
    OSCCONbits.SCS = 1;      //configuracion de reloj interno
    
    //IMPORTAR FUNCION DEL ADC DESDE LIBRERIA
    adc_config();               //se llama funcion de la libreria
    //IMPORTAR FUNCION DEL ADC DESDE LIBRERIA
    uart_config();              //se llama funcion de la libreria
    
    //CONFIGURACION DE INTERRUPCIONES
    INTCONbits.GIE=1;           //se habilitan las interrupciones globales
    INTCONbits.T0IE=0;          // enable bit de int timer0
    INTCONbits.T0IF=0;        //se apaga la bandera de int timer0
    INTCONbits.RBIE=0;          // se habilita IntOnChange
    INTCONbits.RBIF=0;          // se apaga la bandera de IntOnChangeB  
    //CONFIGURACION INTERRUPCION DEL ADC
    PIE1bits.ADIE = 0 ; //se prende interrupcion por ADC
    PIR1bits.ADIF = 0; // se baja bandera de conversion
    //INTERRUPT ON CHANGE PORT B
    /*IOCBbits.IOCB0=1;           //se habilita IOCB RB0
    IOCBbits.IOCB1=1;           //se habilita IOCB RB1*/
    return;
}

/*-----------------------------------------------------------------------------
 --------------------------------- FUNCIONES ----------------------------------
 -----------------------------------------------------------------------------*/

//--------funcion para swticheo de canales de adc
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

//--------funcion para recepcion de datos via uart
void recepcion_uart(void)
{
    switch(dato_recibido)
    {
        case(1):                //si recibe un 1 desde la pc
            cuenta_uart++;      //se suma la variable de control
            break;
            
        case(2):                //si recibe un 2 desde la pc
            cuenta_uart--;      //se resta la variable de control
            break;
            
        default:                //si no se manda nada desde la pc
            cuenta_uart=0;      //se queda como 09 la variable control
            break;
    }
}

//--------funcion para conversion de valores ascii
char datos_ascii(uint8_t numero)    //funcion para convertir a valores ascii
{
    switch(numero)
    {
        case(0):
            return 48;
            break;
            
        case(1):
            return 49;
            break;
            
        case(2):
            return 50;
            break;
            
        case(3):
            return 51;
            break;
            
        case(4):
            return 52;
            break;
            
        case(5):
            return 53;
            break;
            
        case(6):
            return 54;
            break;
            
        case(7):
            return 55;
            break;
            
        case(8):
            return 56;
            break;
            
        case(9):
            return 57;
            break;
            
    }
}
//--------funcion para conversion de valores ascii

void conversiones()
{
    centenas1=((conversion1/100)%10) ;  // centenas de potenciometro1
    decenas1=((conversion1/10)%10) ;    // decenas de potenciometro1
    unidades1=(conversion1%10) ;        // unidades de portenciometro1
    
    centenas1=((conversion2/100)%10) ;  // centenas de potenciometro2
    decenas1=((conversion2/10)%10) ;    // decenas de potenciometro2
    unidades1=(conversion2%10) ;        // unidades de portenciometro2
    
    centenas1=((cuenta_uart/100)%10) ;  // centenas de valores uart
    decenas1=((cuenta_uart/10)%10) ;    // decenas de valores uart
    unidades1=(cuenta_uart%10) ;        // unidades de valores uart
}


char* lcd_ascii()
{
    char random[16];    //se usa set de 16bits
    random[0]=datos_ascii(centenas1);   //centenas de potenciometro 1
    random[1]=datos_ascii(decenas1);    //decenas de potenciometro 1
    random[2]=datos_ascii(unidades1);   //unidades de potenciometro 1
    random[3]=32;                       //se deja espacio 
    random[4]=32;                       //se deja espacio
    random[5]=datos_ascii(centenas2);   //centenas de potenciometro 2
    random[6]=datos_ascii(decenas2);    //decenas de potenciometro 2
    random[7]=datos_ascii(unidades2);   //unidades de potenciometro 2
    random[8]=32;                       //se deja espacio
    random[9]=32;                       //se deja espacio
    random[10]=datos_ascii(centenas3);  //centenas de cuenta uart
    random[11]=datos_ascii(decenas3);   //decenas de cuenta uart
    random[12]=datos_ascii(unidades3);  //unidades de cuenta uart
    random[13]=32;                      //se deja espacio
    random[14]=32;                      //se deja espacio
    random[15]=32;                      //se deja espacio
    return random;                      //se retorna el valor para el lcd
}