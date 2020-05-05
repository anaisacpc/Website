/* Trabalho3Funcionamento1.c
 * Created: 07/12/2018 
 * Author : anaisa
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

typedef struct flag{
	unsigned char sw_free : 1;
	}flag_t;
	
flag_t flag={0};

unsigned char valdisp[] = {0xC0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xBF,0xFF};
unsigned char sw;
volatile unsigned int flag500=0;
volatile char buffer[5];
volatile uint8_t dispID=0;

int vel=0; //inicia a variavel de velocidade do motor a 0
int velm=15; //inicia a variavel para o TC0 a 15 
volatile int conta500=0;

unsigned char aux[]="-";


void display (void);
void pausa500ms (void);

void init_io(void)       //inicializaçoes da placa i/o
{
	DDRA = 0b11000000;   //configura o porto A, como saidas para o bit 7 e 6, e os restantes como entradas
	PORTA = 0b11000000;  //seleciona o display mais a direita (pino 7 e 6) desactiva os pull-ups internos nos restantes
	DDRB = 0b11100000;   //configura o porto B como saida de dados (o motor)
	PORTB = 0b00100000;  //seleciona a rotaçao do motor no sentido dos ponteiros do relogio
	//PORTB=0b01000000; //counter-clockwise
	DDRC = 0b11111111;   //configura o porto C como saida de dados (o display)
	PORTC = 0b00000000;  //o display começa como estando com todos os leds apagados
		
	TCCR0 = 0b00000110;									// configuracao TC0 modo CTC 5ms, prescaler 1024
	OCR0 = 77;
	
	TCCR2 =	0b01100011;									// TC2 PWM phase correct 500Hz - prescaler 64, OC2 non-inverting
	OCR2 = 0;											// inicia o motor parado
	
	TIMSK |= 0b00000010;								// activa interrupcao TC0 CTC
	
	SREG |= 0x80;        //ativaçao de interrupções globais 
}

ISR(TIMER0_COMP_vect)										// interrupcao, timer 0
{
	
	conta500++;
	if (conta500==1000)
	{
		flag500 = 1;
		conta500=0;
	}
	
	display();
}

int main(void)
{
    init_io();	
	sprintf(buffer, "1234");
    while (1)
    {
		sw = PINA & 0b00111111;  //bits que representam os switches	
		switch (sw)
		{
		case  0b00111110:			 //se for selecionado o sw1
				if ((vel==100))			 //se a velocidade ja for a maxima nao faz nada
				{
					velm=255;
					OCR2=velm;
					break;
				}
					
				else                     //se ainda for a velocidade maxima 
				{
					if(flag.sw_free==0)
					{
						if(vel>=0)
							vel=vel+5;
						else
							vel=vel-5;           //incrementa a velocidade do motor em 5 para display
						velm=velm+12;		 //incrementa 12 pq é o referente de OCR2 para 5%
						OCR2=velm;		
						flag.sw_free=1;
						break;
					}	
				}
					
			case 0b00111101:			 //se for selecionado o sw2
				if (vel==0)              //verifica se a velocidade ja é 0
				{
					velm=0;
					OCR2=velm;
					break;
				}
					
				else                     //nao sendo zero
				{
					if(flag.sw_free==0)
					{
						if(vel>0)
							vel=vel-5;           //decrementa a velocidade do motor em 5 para display
						else
							vel=vel+5;
						velm=velm-12;		 //decrementa 12 pq é o referente de OCR2 para 5%
						OCR2=velm;
						flag.sw_free=1;
						break;
					}	
				}				
			case 0b00111011:			//se for selecionado o sw3
				if (vel==25)
					break;
				else
				{
					if(flag.sw_free==0)
					{
						vel=25;				//mostra display 25%
						velm=75;
						OCR2=velm;			//representa os 25% das contas para OCR2
						flag.sw_free=1;	
						break;
					}	
				}
			case 0b00110111:			//se for selecionado o sw4
				if (vel==50)
				break;
			else
			{
				if(flag.sw_free==0)
				{
					vel=50;
					velm=135;
					OCR2=velm;
					flag.sw_free=1;
					break;
				}
			}	
			case 0b00101111:  //se for selecionado o sw5
			if (flag.sw_free==0)
			{
			
				if (flag500==0)
					{
						PORTB=0b01100000;
						pausa500ms();
						PORTB=0b01000000; //counter-clockwise
						vel=-vel;
						flag.sw_free=1;	
						flag500=1;
						break;
					}
				else 
					{
						PORTB=0b01100000;
						pausa500ms();
						PORTB=0b00100000; //clockwise
						vel=-vel;
						flag.sw_free=1;
						flag500=0;
						break;
					}	
			}
			break;
				
			case 0b00011111:  //se for selecionado o sw6
			if(flag.sw_free==0)
			{
				vel=0;
				velm=15;
				OCR2=velm;
				flag.sw_free=1;
				break;
			}
			
		case (0b00111111):
				flag.sw_free=0;
				break;
				
		}//fim da funçao de switch entre botoes
		sprintf(buffer, "%d",vel); 
    }//fim da funçao while
}// fim da funçao main


void display (void)
{
	uint8_t temp;
	uint8_t len=0;

	PORTA = dispID;
	temp=(4-strlen(buffer));
	for (len=0; len<temp; len++)
	{
		buffer[4]='\0';
		buffer[3]=buffer[2];
		buffer[2]=buffer[1];
		buffer[1]=buffer[0];
		buffer[0]=11;
	}
	
	if(buffer[dispID>>6]=='-')
	{
		PORTC= valdisp[10];
		dispID += 0x40;
	}	
	else
		if((buffer[dispID>>6])==11)
		{
			PORTC= valdisp[11];
			dispID += 0x40;
		}
		else
		{
			PORTC=valdisp[buffer[dispID>>6]-0x30];
			dispID += 0x40;
		}
}





void pausa500ms (void)
{
	conta500 = 0;											// faz reset ao contador
	flag500 = 0;										// limpa flag dos 500ms
	while (flag500 == 0);						// agurda que a flag ag volte a 1
}