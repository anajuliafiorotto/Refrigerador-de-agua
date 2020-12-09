#include "pic18f4520.h"
#include "config.h"
#include "lcd.h"
#include "teclado.h"
#include "pwm.h"
#include "delay.h"

//lê o valor escolhido pelo usuário
unsigned char leTeclado(){         
    unsigned char tecla = 255;
    
    TRISB = 0xF8;                  //config da porta B como Entrada
    TRISD = 0x0F;                  //config da porta D como Entrada
    
    do{
        tecla = tc_tecla(0);       //le teclado
    }while(tecla == 255);
    
    return tecla;
}

//exibe no display o valor escolhido pelo usuário
void exibeDisplay(unsigned char aux){
    int values7seg[10] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x67};
    
    TRISA = 0x00;                  //config da porta A como Saída
    BitClr(INTCON2, 7);            //liga pull up 
    ADCON1 = 0x0E;                 //config AD 
    TRISD = 0x00;                  //config da porta D como Saída
    PORTD = 0x00;                  //config da porta D como Saida
    
    PORTD = values7seg[aux];
    
    BitSet(PORTA, 5);              //liga 
    BitClr(PORTA, 4);              //desliga
    BitClr(PORTA, 3);              //desliga
    BitClr(PORTA, 2);              //desliga

    atraso_ms(50);

    BitClr(PORTA, 5);              //desliga
}

//liga o led correspondente ao valor escolhido pelo usuário
void ligaLED(int aux){
    int valuesLED[8] = {0b00000001, 0b00000010, 0b00000100,  0b00001000, 0b00010000, 0b00100000, 0b01000000, 0b10000000};
    
    TRISB = 0x00;                  //config da porta B como Saída
    PORTB = valuesLED[aux];        //liga o led correspondente a tecla pressionada
    
    atraso_ms(50);
    
    PORTB = 0x0000000000;          //desliga todos os leds
}

//auxilia na escolha do nível de refrigeração
unsigned char nivelLCD(){
    unsigned char aux;
    int i;
    
    ADCON1 = 0x06;                 //config AD
    TRISB = 0x01;                  //config da porta B como Entrada
    TRISD = 0x00;                  //config da porta D como Saída
    TRISE = 0x00;                  //config da porta E como Saída
    
    lcd_init();
    
    do {
        lcd_cmd(L_CLR);
        lcd_cmd(L_L1);
        lcd_str("Escolha o nivel");
        lcd_cmd(L_L2);
        lcd_str("de refrigeracao:");
        lcd_cmd(L_L3);
        lcd_str("(1)   (2)   (3)");
        lcd_cmd(L_L4);
        lcd_str("   (4)   (5)");
        atraso_ms(100);
    
        aux = leTeclado();
        
        if((aux < 1) || (aux > 5)){
            ADCON1 = 0x06;         //config AD
            TRISB = 0x01;          //config da porta B
            TRISD = 0x00;          //config da porta D como Saída
            TRISE = 0x00;          //config da porta E como Saída

            lcd_init();
            
            lcd_cmd(L_CLR);
            lcd_cmd(L_L1);
            lcd_str("Valor invalido");
            atraso_ms(100);
            
            for(i = 0; i < 20; i++){
                exibeDisplay(aux);
                ligaLED(aux);
            }
        }
    } while((aux < 1) || (aux > 5));
    
    return aux;
}

//auxilia na escolha do estado de saída da água utilizando o mesmo para
//ligar os reles e o cooler
unsigned char estadoLCD(unsigned char n){
    unsigned char aux;
    int i;
    
    ADCON1 = 0x06;                 //config AD
    TRISB = 0x01;                  //config da porta B como Entrada
    TRISD = 0x00;                  //config da porta D como Saída
    TRISE = 0x00;                  //config da porta E como Saída
    
    lcd_init();
    
    do {
        lcd_cmd(L_CLR);
        lcd_cmd(L_L1);
        lcd_str("Pressione 1 para");
        lcd_cmd(L_L2);
        lcd_str("sair agua normal");
        lcd_cmd(L_L3);
        lcd_str("Pressione 2 para");
        lcd_cmd(L_L4);
        lcd_str("sair agua gelada");
        atraso_ms(100);
        
        aux = leTeclado();
        
        if(aux == 1){
            TRISC = 0x00;          //config da porta C como Saida
            PORTC = 0x01;          //liga rele 1
        }
        
        else if(aux == 2){
            TRISC = 0x00;          //config da porta C como Saida
            TRISE = 0x00;          //config da porta E como Saida
            PORTE = 0x01;          //liga rele 2
            
            pwmInit();
            
            pwmSet1((n*20)-1);     //liga o cooler com a intensidade de acordo com o nivel
        }
        
        else if((aux < 1) || (aux > 2)){
            ADCON1 = 0x06;         //config AD
            TRISB = 0x01;          //config da porta B como Entrada
            TRISD = 0x00;          //config da porta D como Saída
            TRISE = 0x00;          //config da porta E como Saída

            lcd_init();
            
            lcd_cmd(L_CLR);
            lcd_cmd(L_L1);
            lcd_str("Valor invalido");
            atraso_ms(100);
            
            for(i = 0; i < 20; i++){
                exibeDisplay(aux);
                ligaLED(aux);
            }
        }
    } while((aux < 1) || (aux > 2));
    
    return aux;
}

//auxilia no interrupmento da saída de água utilizando o mesmo para
//desligar os reles e o cooler
unsigned char valvulaLCD(unsigned char e){
    unsigned char aux;
    int i;
    
    ADCON1 = 0x06;                 //config AD
    TRISB = 0x01;                  //config da porta B como Entrada
    TRISD = 0x00;                  //config da porta D como Saída
    TRISE = 0x00;                  //config da porta E como Saída
    
    lcd_init();
    
    do {
        if(e == 1){
            do {
                lcd_cmd(L_CLR);
                lcd_cmd(L_L1);
                lcd_str("Pressione 1 para");
                lcd_cmd(L_L2);
                lcd_str("interromper a");
                lcd_cmd(L_L3);
                lcd_str("saida de agua");
                atraso_ms(100);

                aux = leTeclado();
                
                if(aux == 1){
                    PORTC = 0x00;  //desliga o rele 1
                }
                
                else {
                    ADCON1 = 0x06; //config AD
                    TRISB = 0x01;  //config da porta B como Entrada
                    TRISD = 0x00;  //config da porta D como Saída
                    TRISE = 0x00;  //config da porta E como Saída

                    lcd_init();

                    lcd_cmd(L_CLR);
                    lcd_cmd(L_L1);
                    lcd_str("Valor invalido");
                    atraso_ms(100);

                    for(i = 0; i < 20; i++){
                        exibeDisplay(aux);
                        ligaLED(aux);
                    }
                }
            } while(aux != 1);
        }

        else if(e == 2){
            do {
                lcd_cmd(L_CLR);
                lcd_cmd(L_L1);
                lcd_str("Pressione 2 para");
                lcd_cmd(L_L2);
                lcd_str("interromper a");
                lcd_cmd(L_L3);
                lcd_str("saida de agua");
                atraso_ms(100);

                aux = leTeclado();
                
                if(aux == 2){
                    pwmSet1(0);     //desliga o cooler
                    PORTE = 0x00;   //desliga o rele 2
                }
                
                else {
                    ADCON1 = 0x06;  //config AD
                    TRISB = 0x01;   //config da porta B como Entrada
                    TRISD = 0x00;   //config da porta D como Saída
                    TRISE = 0x00;   //config da porta E como Saída

                    lcd_init();

                    lcd_cmd(L_CLR);
                    lcd_cmd(L_L1);
                    lcd_str("Valor invalido");
                    atraso_ms(100);

                    for(i = 0; i < 20; i++){
                        exibeDisplay(aux);
                        ligaLED(aux);
                    }
                }
            } while(aux != 2);
        }
    } while((aux < 1) || (aux > 2));
    
    return aux;
}

//garante que o usuário possa reiniciar todos os passos do programa 
unsigned char reiniciaLCD(){
    unsigned char n;
    unsigned char e;
    unsigned char v;
    unsigned char aux;
    int i;
    
    ADCON1 = 0x06;             //config AD
    TRISB = 0x01;              //config da porta B como Entrada
    TRISD = 0x00;              //config da porta D como Saída
    TRISE = 0x00;              //config da porta E como Saída
    
    lcd_init();
    
    do {
        lcd_cmd(L_CLR);
        lcd_cmd(L_L1);
        lcd_str("Pressione 1 para");
        lcd_cmd(L_L2);
        lcd_str("novo nivel");
        lcd_cmd(L_L3);
        lcd_str("Pressione 2 para");
        lcd_cmd(L_L4);
        lcd_str("novo estado");
        atraso_ms(100);
        
        aux = leTeclado();
        
        for(i = 0; i < 20; i++){
            exibeDisplay(aux);
            ligaLED(aux); 
        }
        
        if(aux == 1){
            n = nivelLCD();
            for(i = 0; i < 20; i++){
                exibeDisplay(n);
                ligaLED(n);
            }
            
            e = estadoLCD(n);
            for(i = 0; i < 20; i++){
               exibeDisplay(e);
                ligaLED(e); 
            }
            
            v = valvulaLCD(e);
            for(i = 0; i < 20; i++){
                exibeDisplay(v);
                ligaLED(v);
            } 
        }
        
        else if(aux == 2){
            e = estadoLCD(n);
            for(i = 0; i < 20; i++){
                exibeDisplay(e);
                ligaLED(e);
            }
            
            v = valvulaLCD(e);
            for(i = 0; i < 20; i++){
                exibeDisplay(v);
                ligaLED(v);
            }
        }
        
        else {
            ADCON1 = 0x06;             //config AD
            TRISB = 0x01;              //config da porta B como Entrada
            TRISD = 0x00;              //config da porta D como Saída
            TRISE = 0x00;              //config da porta E como Saída
    
            lcd_init();
            
            lcd_cmd(L_CLR);
            lcd_cmd(L_L1);
            lcd_str("Valor invalido");
            atraso_ms(100);
            
            for(i = 0; i < 20; i++){
                exibeDisplay(aux);
                ligaLED(aux);
            }
        }
    } while((aux < 1) || (aux > 2));
    
    return aux;
}

void main(void) {
    unsigned char nivel;
    unsigned char estado;
    unsigned char valvula;
    unsigned char reinicia;
    int i;
    
    ADCON1 = 0x06;      //config AD
    TRISB = 0x01;       //config da porta B
    TRISD = 0x00;       //config da porta D como Saída
    TRISE = 0x00;       //config da porta E como Saída
    
    lcd_init();
    
    nivel = nivelLCD();
    for(i = 0; i < 20; i++){
        exibeDisplay(nivel);
        ligaLED(nivel);
    }
          
    estado = estadoLCD(nivel);
    for(i = 0; i < 20; i++){
       exibeDisplay(estado);
        ligaLED(estado); 
    }
             
    valvula = valvulaLCD(estado);
    for(i = 0; i < 20; i++){
        exibeDisplay(valvula);
        ligaLED(valvula);
    }
 
    for(;;){
        reinicia = reiniciaLCD();
    }
}