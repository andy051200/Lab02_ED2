/* 
 * File: LCD.h  
 * Se utiliz� y se adaptaron las librer�as de Ligo George 
 * de la p�gina www.electrosome.com
 * Enlace: https://electrosome.com/lcd-pic-mplab-xc8/
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef LCD_H
#define	LCD_H

#ifndef _XTAL_FREQ
#define _XTAL_FREQ 8000000
#endif

#ifndef rs
#define rs RC0
#endif

#ifndef rw
#define rw RC1
#endif

#ifndef en
#define en PORTCbits.RC2
#endif

#include <xc.h> // include processor files - each processor file is guarded.  

/*-----------------------------------------------------------------------------
----------- prototipo de funcion de configuracion de displays -----------------
-----------------------------------------------------------------------------*/
void lcd_init();                //funcion de inicialización
void cmd(unsigned char a);      //funcion de control de comandos
void dat(unsigned char b);      //funcion para mandar datos
void show(unsigned char *s);    //funcion para mandar informacion ASCII
void lcd_linea(char a, char b); //funcion para poner cursor
void lcd_mov_derecha(void);     //funcion para mover a la derecha
void lcd_mov_izquierda(void);   //funcion para mover a la izquierda

#endif	/* LCD_H */

