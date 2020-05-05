/* Trabalho3Funcionamento2.c
 * Author : anaisa
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdint.h>	
#include <ctype.h>							//biblioteca para permitir converter variavel recolhida da usart em minusculas

unsigned char valdisp[] = {0xC0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xBF,0xFF};
unsigned char teclado;
volatile char buffer[5];
volatile uint8_t dispID=0;
volatile unsigned int flag500=0;

int vel=0; //inicia a variavel de velocidade do motor a 0
int velm=15; //inicia a variavel para o TC0 a 15
int conta500=0;
int flag_roda=0;

void display (void);
void pausa500ms (void);
void mostrapc(char *buffer);


struct USART{
	uint8_t estado;
	uint8_t rx_buffer;
	char tx_buffer[50];
} usart;


void init_io(void)       //inicializaçoes da placa i/o
{
	DDRA = 0b11000000;   //configura o porto A, como saidas para o bit 7 e 6, e os restantes como entradas
	PORTA = 0b11000000;  //seleciona o display mais a direita (pino 7 e 6) desactiva os pull-ups internos nos restantes
	DDRB = 0b11100000;   //configura o porto B como saida de dados (o motor)
	PORTB = 0b00100000;  //seleciona a rotaçao do motor no sentido dos ponteiros do relogio
	DDRC = 0b11111111;   //configura o porto C como saida de dados (o display)
	PORTC = 0b00000000;  //o display começa como estando com todos os leds apagados
	
	TCCR0 = 0b00000110;									// configuracao TC0 modo CTC 5ms, prescaler 1024
	OCR0 = 77;
	
	TCCR2 =	0b01100011;									// TC2 PWM phase correct 500Hz - prescaler 64, OC2 non-inverting
	OCR2 = 0;											// inicia o motor parado
	
	TIMSK |= 0b00000010;								// activa interrupcao TC0 CTC
	
	//Configurações da usart1
	UBRR1H = 0;											//significa usart1
	UBRR1L = 103;										//representa os 19200bps
	UCSR1A = (1<<U2X1);									//BAUD em modo assincrono com dobro da velocidade 
	UCSR1B = (1<<RXCIE1)|(1<<RXEN1)|(1<<TXEN1);			//set da transmissao e recepcao e respectiva interrupcao
	UCSR1C = (1<<UCSZ11)|(1<<UCSZ10);					// modo assincrono, sem paridade, 1 stop bit + 8 dados
	
	SREG |= 0x80; //ativaçao de interrupções globais
	
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

ISR(USART1_RX_vect)										// interrupcao, timer 0
{
teclado=UDR1; //coloca os valores do registo da comunicaçao na variavel teclado (teclado envia para pino rx que guarda no registo da UDR1)
}

int main(void)
{
   init_io();	
    while (1)
    {
	
		switch (teclado)
		{
			case 'P':
			case 'p':
				vel=0;
				velm=15;
				OCR2=velm;
				teclado='0';          //apenas envia 1x o "carregar" de 1 tecla do teclado do pc
				break;
			
			case '1':
			if (vel==25){
				teclado='0';
				break;
			}
			else
			{
				
					vel=25;				//mostra display 25%
					velm=75;
					OCR2=velm;			//representa os 25% das contas para OCR2
					teclado='0';
					break;				
			}
			case '2':
			if (vel==50)
			{
				teclado='0';
				break;
			}
			else
			{
				vel=50;
				velm=135;
				OCR2=velm;
				teclado='0';
				break;
			}
			break;
			case '+':
			if ((vel==100))			 //se a velocidade ja for a maxima nao faz nada
			{
				velm=255;
				OCR2=velm;
				teclado='0';
				break;
			}
			
			else                     //se ainda for a velocidade maxima
			{
				if(vel>=0)
				vel=vel+5;
				else
				vel=vel-5;           //incrementa a velocidade do motor em 5 para display
				velm=velm+12;		 //incrementa 12 pq é o referente de OCR2 para 5%
				OCR2=velm;
				teclado='0';
				break;
			}
			break;

			case '-':
			if (vel==0)              //verifica se a velocidade ja é 0
			{
				velm=0;
				OCR2=velm;
				teclado='0';
				break;
			}
			
			else                     //nao sendo zero
			{
					if(vel>0)
					vel=vel-5;           //decrementa a velocidade do motor em 5 para display
					else
					vel=vel+5;
					velm=velm-12;		 //decrementa 12 pq é o referente de OCR2 para 5%
					OCR2=velm;
					teclado='0';
					break;
			}
			break;
			case 'I':
			case 'i':
			if(flag_roda==0){
					PORTB=0b01100000;
					pausa500ms();
					PORTB=0b01000000;  //counter-clockwise
					flag_roda=1;
					teclado='0';
					break;
			}
			if(flag_roda==1){
					PORTB=0b01100000;
					pausa500ms();
					PORTB=0b00100000; //clockwise
					flag_roda=0;
					teclado='0';
					break;
			}

			break;
			case 'C':
			case 'c':
				sprintf(usart.tx_buffer,"\n O motor tem um PWM de: %d %% \n\r", vel);   //guarda no buffer o que para imprimir
				mostrapc(usart.tx_buffer);           //na funçao vai passar como parametro o valor anterior do buffer (frase)
				teclado='0';
				break;
		}//fim da funçao de switch entre botoes
		sprintf(buffer, "%.4d",vel); 
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

void mostrapc(char *buffer)         //funçao para imprimir para o pc o valor do buffer
{
	uint8_t i = 0;
	while(buffer[i] != '\0')
	{
		while((UCSR1A & 1<<UDRE1) == 0);
		UDR1 = buffer[i];     //envio de caracter a caracter para o registo da comunicaçao serie
		i++;
	}
}