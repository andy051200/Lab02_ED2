/* 
 * File: LCD.c  
 * Se utiliz� y se adaptaron las librer�as de Ligo George 
 * de la p�gina www.electrosome.com
 * Enlace: https://electrosome.com/lcd-pic-mplab-xc8/
 * Revision history: 
 */

/* 
 * File:   LCD.h
 * Author: Andy Bonilla (hice una Frankenstein)
 *
 * Created on 24 de julio de 2021, 11:32 PM
 */


//LCD Functions Developed by electroSome y modificadas por yo
/*-----------------------------------------------------------------------------
 ----------------------------LIBRERIAS-----------------------------------------
 -----------------------------------------------------------------------------*/
#include "LCD.h"

/*-----------------------------------------------------------------------------
-------------------------------- FUNCIONES -----------------------------------
-----------------------------------------------------------------------------*/

//--------funcion para inicializar el lcd
void lcd_init()
{
	cmd(0x38);
	cmd(0x0c);
	cmd(0x06);
	cmd(0x80);
}

//--------funcion para configuracion de comandos
void cmd(unsigned char a)
{
	PORTB=a;
	rs=0;
	rw=0;
	en=1;
    __delay_ms(1);
	en=0;
}

//--------funcion para envio de datos
void dat(unsigned char b)
{
	PORTB=b;
	rs=1;
	rw=0;
	en=1;
	__delay_ms(1);
	en=0;
}

//--------funcion para envio de informacion con punteros
void show(unsigned char *s)
{
	while(*s) {
		dat(*s++);
	}
}

//--------funcion para seleccion de linea en lcd
void lcd_linea(char a, char b) {
    char temp, z;
    if (a == 1) {
        temp = 0x80 + b - 1;
        z = temp;       //como es de 8bits se usa toda la variable
        cmd(z);     //se manda valor al puerto
        
    } else if (a == 2) {
        temp = 0xC0 + b - 1;
        z = temp;     //como es de 8bits se usa toda la variables
        cmd(z);   //se manda valor al puerto
        
    }
}

//--------funcion para mover a la derecha
void lcd_mov_derecha(void) {
    cmd(0x1c);      //se agrupan los dos nibbles en un byte
}

//--------funcion para mover a la izquierda
void lcd_mov_izquierda(void) {
    cmd(0x18);      //se agrupan los dos nibbles en un byte
}