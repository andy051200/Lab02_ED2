/* 
 * File:   uart_config.h
 * Author: Andy Bonilla
 *
 * Created on 17 de julio de 2021, 11:32 PM
 */

/*-----------------------------------------------------------------------------
 ----------------------------LIBRERIAS-----------------------------------------
 -----------------------------------------------------------------------------*/
#include <xc.h>                 //se incluye libreria del compilador
#include "uart_config.h"

/*-----------------------------------------------------------------------------
-------------------------- FUNCION DE LIBRERIA --------------------------------
-----------------------------------------------------------------------------*/

void uart_config(void)
{
    //CONFIGURACION DE UART
    //transmision
    TXSTAbits.SYNC = 0;
    TXSTAbits.BRGH = 1;
    //braudeaje
    BAUDCTLbits.BRG16 = 1;
    SPBRG = 25;         // SPBRGH:SPBRG = 25
    SPBRGH = 0;
    //recepcion
    RCSTAbits.SPEN = 1;
    RCSTAbits.RX9 = 0;
    //se encienden modulos
    RCSTAbits.CREN = 1;
    TXSTAbits.TXEN = 1;
    return;
}


