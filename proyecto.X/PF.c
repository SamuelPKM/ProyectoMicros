/*
 * File:   PF.c
 * Author: blasg
 *
 * Created on 28 de octubre de 2022, 08:12 AM
 */
#pragma config FOSC = INTOSCIO //Selecciona oscilador interno
#pragma config WDTEN = OFF //Se deshabilita el perro guardian
#pragma config MCLRE = ON //Se habilita el pin 1 como reset externo
#pragma config LVP = OFF //Se deshabilita la programación por bajo voltaje
#pragma config ICPRT = OFF //Se deshabilita el pin para programación y depuración

#include <xc.h>
#include <stdio.h>
#define _XTAL_FREQ 8000000

void Ultrasonic_Init(void){
    TRISBbits.RB2 = 0;
    TRISBbits.RB3 = 1;
    LATBbits.LB2  = 0;

}

float ObtenerDistancia(void)
{
    //* distancia = (Tiempo * velocidad)/2
    //* velocidad del sonido "340m/s" o "0.034cm/us"
    //* Entonces la formula es: Distancia=TMR0*0.034/2
    float distancia;
    float tiempo;
    
    LATBbits.LB2=1;
    __delay_us(10);
    LATBbits.LB2=0;
    TMR0=0;
    
    while(PORTBbits.RB3==0);
    TMR0=0;
    while(PORTBbits.RB3==1);
    tiempo=TMR0;  
    
     
    //distancia = (float)(tiempo*0.034/2);
    distancia = 9;
    return distancia;
}

void TMR0_Init(void){ 
    T0CON = 0x88;//configuro los registros del TMR0 como contador
    TMR0 = 0;  
}

void Configuracion(void)
{
    ADCON1 = 0x0F;    //todos los pines del A,B como digitales
    OSCCON = 0x72;    //oscilador interno a 8MHz
    T0CON = 0x88;//configuro los registros del TMR0 como contador
    TMR0 = 0;  

    TRISBbits.RB0 = 0;
    TRISBbits.RB1 = 0;
    LATBbits.LB0 = 0;
    LATBbits.LB1 = 0;
    
    TRISD=0;	//Salidas digitales
    ANSELD=0;
    ANSELB=0;

}

void putch(char data) {
    char Activa;
    Activa = data & 0xF0;
    LATD = Activa | 0x05; //0bxxxx0101
    __delay_us(10);
    LATD = Activa | 0x01; //0bxxxx0001
    __delay_ms(1);
    Activa = data << 4;
    LATD = Activa | 0x05;
    __delay_us(10);
    LATD = Activa | 0x01;    
}

void putcm(char data) {
    char Activa;
    Activa = data & 0xF0; //0bxxxx0100
    LATD = Activa | 0x04;
    __delay_us(10);
    LATD = Activa;
    __delay_ms(1);
    Activa = data << 4;
    LATD = Activa | 0x04;
    __delay_us(10);
    LATD = Activa;
}

void LCD_Init(void)
{
    __delay_ms(30);
    putcm(0x02);    // Inicializa en modo 4 bits
    __delay_ms(1);

    putcm(0x28);    // Inicializa en 2 líneas 5x7
    __delay_ms(1);

    putcm(0x2C);
    __delay_ms(1);

    putcm(0x0C);
    __delay_ms(1);

    putcm(0x06);
    __delay_ms(1);

    putcm(0x80); //Posiciona el curson en 1,1
    __delay_ms(1);
}

void main(void) {
    LCD_Init();
    __delay_ms(100);
    float distancia;
    Configuracion();
    TMR0_Init();
    Ultrasonic_Init();

 
    while (1) {
        distancia = ObtenerDistancia();
        printf(" Distancia");
        putcm(0xC2);
        printf("%f cm", distancia);
        if(distancia < 10)
        {
            LATBbits.LB0 = 0;
            LATBbits.LB1 = 1;
        }
        else
        {
            LATBbits.LB0 = 1;
            LATBbits.LB1 = 0;
        }
        __delay_ms(200);

    }

    return;
}
