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
unsigned char datos_ascii(uint8_t numero);
void conversiones(void);
uint8_t lcd_ascii();
void deliver(void);
/*-----------------------------------------------------------------------------
 ----------------------- VARIABLES A IMPLEMTENTAR------------------------------
 -----------------------------------------------------------------------------*/
unsigned char conversion1;  //variable que almacena potenciometro 1
unsigned char conversion2;  //variable que almacena potenciometro 2
unsigned char dato_recibido;    //variable para sumas y restas, uart
unsigned char cuenta_uart;  //variable para cuenta de suma y resta, uart
/*
unsigned char centenas1;
unsigned char decenas1;
unsigned char unidades1;

unsigned char centenas2;
unsigned char decenas2;
unsigned char unidades2;*/

unsigned char centenas3;
unsigned char decenas3;
unsigned char unidades3;

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
	cmd(0x90);          //invocao la funcion de configurcion de comandos lc
    
    while(1)
    {
        //-------seccion para llamr varias funciones
        toggle_adc();               //se llama funcion para cambiar canales
        recepcion_uart();           //se llama funcion para cuenta uart
        
        
        //-------seccion para mandar texto al lcd
        lcd_linea(1,1);             //selecciono la linea 1 para escribir
        show(" S1   S2   S3 ");//mensaje a enviar linea 1
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
    ANSEL=0;                //sin entradas anal??gicas
    ANSELH=0;               //sin entradas analogicas
    ANSELbits.ANS0=1;       //An0 como entrada analogica, potenciometro 1
    ANSELbits.ANS1=1;       //An1 como entrada analogica, potenciometro 2
    
    //CONFIGURACION DE IN-OUT DE PUERTOS
    TRISB=0;
    
    //TRISC=0;
    TRISDbits.TRISD5=0;
    TRISDbits.TRISD6=0;
    TRISDbits.TRISD7=0;
    //TRISE=0;
    
    PORTB=0;
    PORTD=0;
    //PORTE=0;
    
    //CONFIGURACION DE RELOJ
    OSCCONbits.IRCF = 0b111; //Fosc 8MHz
    OSCCONbits.SCS = 1;      //configuracion de reloj interno
    
    //IMPORTAR FUNCION DEL ADC DESDE LIBRERIA
    adc_config();               //se llama funcion de la libreria
    //IMPORTAR FUNCION DEL ADC DESDE LIBRERIA
    uart_config();              //se llama funcion de la libreria
    
    //CONFIGURACION DE INTERRUPCIONES
    INTCONbits.GIE=1;           //se habilitan las interrupciones globales
    PIE1bits.RCIE=1;            //se habilita interrupcion por recepcion uart
    PIR1bits.RCIF=0;            //se apaga bandera de recepcion uart
    //return;
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
                ADCON0bits.GO=1;                //se inicia otra conversion ADC
                break;
                    
            case(1):
                conversion2=ADRESH;         //potenciometro 2
                __delay_us(100);            //delay para cargar capacitor
                ADCON0bits.CHS=0;           //switch de canal
                ADCON0bits.GO=1;                //se inicia otra conversion ADC
                break;
            
            __delay_us(100);                //delay para carga de capacitor
            ADCON0bits.GO=1;                //se inicia otra conversion ADC
        }
    }
}

//--------funcion para recepcion de datos via uart
void recepcion_uart(void)
{
    switch(dato_recibido)        //se analiza el dato mandado por la pc
    {
        case('1'):                //si recibe un 1 desde la pc
            cuenta_uart++;      //se suma la variable de control
            break;
            
        case('2'):                //si recibe un 2 desde la pc
            cuenta_uart--;      //se resta la variable de control
            break;
            
        default:                //si no se manda nada desde la pc
            cuenta_uart=cuenta_uart; //se queda como 09 la variable control
            break;
    }
}

//--------funcion para conversion de valores ascii
unsigned char datos_ascii(uint8_t numero)    //funcion para convertir a valores ascii
{
    switch(numero)
    {
        default:
            return 0x30;        //retorna 0 en ascii
            break;
        case(0):
            return 0x30;        //retorna 0 en ascii
            break;
            
        case(1):
            return 0x31;        //retorna 1 en ascii
            break;
            
        case(2):
            return 0x32;        //retorna 2 en ascii
            break;
            
        case(3):
            return 0x33;        //retorna 3 en ascii
            break;
            
        case(4):
            return 0x34;        //retorna 4 en ascii
            break;
            
        case(5):
            return 0x35;        //retorna 5 en ascii
            break;
            
        case(6):
            return 0x36;        //retorna 6 en ascii
            break;
            
        case(7):
            return 0x37;        //retorna 7 en ascii
            break;
            
        case(8):
            return 0x38;        //retorna 8 en ascii
            break;
            
        case(9):
            return 0x39;        //retorna 9 en ascii
            break;
            
    }
    
}
//--------funcion para conversion de valores ascii
/*void conversiones()
{
    centenas1=(((2*conversion1)/100)%10) ;  // centenas de potenciometro1
    decenas1=(((2*conversion1)/10)%10) ;    // decenas de potenciometro1
    unidades1=((2*conversion1)%10) ;        // unidades de portenciometro1
    
    centenas1=(((2*conversion2)/100)%10) ;  // centenas de potenciometro2
    decenas1=(((2*conversion2)/10)%10) ;    // decenas de potenciometro2
    unidades1=((2*conversion2)%10) ;        // unidades de portenciometro2
    
    centenas1=(((2*cuenta_uart)/100)%10) ;  // centenas de valores uart
    decenas1=(((2*cuenta_uart)/10)%10) ;    // decenas de valores uart
    unidades1=((2*cuenta_uart)%10) ;        // unidades de valores uart
    return;
}*/


uint8_t lcd_ascii()
{
    uint8_t random[16];                    //se usa set de 16bits
    random[0]=datos_ascii(((2*(conversion1)/100)%10));   //centenas de potenciometro 1
    random[1]=0x2E;                     //punto decimal
    random[2]=datos_ascii(((2*(conversion1)/100)%10));    //decenas de potenciometro 1
    random[3]=datos_ascii((2*conversion1)%10);   //unidades de potenciometro 1
    random[4]=32;                       //se deja espacio 
    random[5]=datos_ascii(((2*(conversion2)/100)%10));   //centenas de potenciometro 2
    random[6]=0x2E;                     //punto decimal
    random[7]=datos_ascii(((2*(conversion2)/100)%10));    //decenas de potenciometro 2
    random[8]=datos_ascii((2*conversion2)%10);   //unidades de potenciometro 2
    random[9]=32;                       //se deja espacio
    random[10]=datos_ascii(cuenta_uart);  //centenas de cuenta uart
    random[11]=0x2E;                    //punto decimal
    random[12]=32;//datos_ascii(decenas3);   //decenas de cuenta uart
    random[13]=32;//datos_ascii(unidades3);  //unidades de cuenta uart
    random[14]=32;                      //se deja espacio
    random[15]=32;                      //se deja espacio
    return random;                      //se retorna el valor para el lcd
}

