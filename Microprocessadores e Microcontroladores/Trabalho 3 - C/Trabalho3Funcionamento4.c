/* Trabalho3Funcionamento4.c
 * Author : anaisa
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdint.h>
#include <ctype.h>							//biblioteca  usart
#include <util/twi.h>						// biblioteca  utilizacao da interface TWI


typedef struct flag{
	unsigned char sw_free : 1;
}flag_t;

flag_t flag={0};


unsigned char valdisp[] = {0xC0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xBF,0xFF};
unsigned char teclado;
unsigned char sw;
volatile char buffer[5];
volatile uint8_t dispID=0;
volatile unsigned int flag500=0;

int vel=0; //inicia a variavel de velocidade do motor a 0
int velm=15; //inicia a variavel para o TC0 a 15
int conta500=0;
int flag_roda=0;
unsigned int valor_ler_ad=0;
unsigned int medAd=0;
unsigned char modo=0xFF;
int16_t eixoX;


void send_message(char *data);
void display (void);
void pausa500ms (void);
void mostrapc(char *buffer);
void CasoBotao(void);
void CasoTeclado(void);
void CasoAcelerometro(void);
int LerDadosAdC (void);
void LerPot (void);
void twi_func(void);
void ler_eixo_x (void);


struct USART{
	uint8_t estado;
	uint8_t rx_buffer;
	char tx_buffer[50];
} usart;


void init_io(void)       //inicializa�oes da placa i/o
{
	DDRA = 0b11000000;   //configura o porto A, como saidas para o bit 7 e 6, e os restantes como entradas
	PORTA = 0b11000000;  //seleciona o display mais a direita (pino 7 e 6) desactiva os pull-ups internos nos restantes
	DDRB = 0b11100000;   //configura o porto B como saida de dados (o motor)
	PORTB = 0b00100000;  //seleciona a rota�ao do motor no sentido dos ponteiros do relogio
	DDRC = 0b11111111;   //configura o porto C como saida de dados (o display)
	PORTC = 0b00000000;  //o display come�a como estando com todos os leds apagados
	
	TCCR0 = 0b00000110;									// configuracao TC0 modo CTC 5ms, prescaler 1024
	OCR0 = 77;
	
	TCCR2 =	0b01100011;									// TC2 PWM phase correct 500Hz - prescaler 64, OC2 non-inverting
	OCR2 = 0;											// inicia o motor parado
	
	TIMSK |= 0b00000010;								// activa interrupcao TC0 CTC
	
	//Configura��es da usart1
	UBRR1H = 0;											//significa usart1
	UBRR1L = 103;										//representa os 19200bps
	UCSR1A = (1<<U2X1);									//BAUD em modo assincrono com dobro da velocidade
	UCSR1B = (1<<RXCIE1)|(1<<RXEN1)|(1<<TXEN1);			//set da transmissao e recepcao e respectiva interrupcao
	UCSR1C = (1<<UCSZ11)|(1<<UCSZ10);					// modo assincrono, sem paridade, 1 stop bit + 8 dados
	
	//configura�ao ADC
	ADCSRA = 0b10000111;								// ativar o ADC, com factor 128 (f=125kHz), sem interrupcao, leitura singular
	ADMUX = 0b00100000;									// Tensao ARef, leitura justificada a esquerda (leitura de 8 bits), canal 0
	
	
	//configura��o acelerometro
	TWSR &= 0b11111100;                               //prescaler 0
	TWBR=72;										  // frequencia de 100kbit/s
	
	SREG |= 0x80; //ativa�ao de interrup��es globais
	
	
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
	teclado=UDR1; //coloca os valores do registo da comunica�ao na variavel teclado (teclado envia para pino rx que guarda no registo da UDR1)
}

int main(void)
{
	init_io();
	while (1)
	{
		LerPot();
	}
	
}

void CasoBotao (void)
{
	modo=0x00;
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
				velm=velm+12;		 //incrementa 12 pq � o referente de OCR2 para 5%
				OCR2=velm;
				flag.sw_free=1;
				break;
			}
		}
		
		case 0b00111101:			 //se for selecionado o sw2
		if (vel==0)              //verifica se a velocidade ja � 0
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
				velm=velm-12;		 //decrementa 12 pq � o referente de OCR2 para 5%
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
		
	}//fim da fun�ao de switch entre botoes
	sprintf(buffer, "%d",vel);

}

void CasoTeclado(void)
{
	modo=0x8C;
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
			velm=velm+12;		 //incrementa 12 pq � o referente de OCR2 para 5%
			OCR2=velm;
			teclado='0';
			break;
		}
		break;

		case '-':
		if (vel==0)              //verifica se a velocidade ja � 0
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
			velm=velm-12;		 //decrementa 12 pq � o referente de OCR2 para 5%
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
		mostrapc(usart.tx_buffer);           //na fun�ao vai passar como parametro o valor anterior do buffer (frase)
		teclado='0';
		break;
	}//fim da fun�ao de switch entre botoes
	sprintf(buffer, "%.4d",vel);

}

void CasoAcelerometro (void)
{
	modo=0x88;
	ler_eixo_x();
	eixoX = (eixoX*255)/32767;
	
	if ((eixoX >= -3) & (eixoX <= 3))
	{
		OCR2 = 0;						// motor parado
	}
	else
	{
		if (eixoX >= 3)
		{
			PORTB=0b01100000;
			pausa500ms();
			PORTB=0b00100000;  //clockwise
			OCR2=eixoX;
			
		}
		if (eixoX <= -3)
		{
			PORTB=0b01100000;
			pausa500ms();
			PORTB=0b01000000; //counter-clockwise
			OCR2=eixoX;
		}
	}
	
	
}

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


void mostrapc(char *buffer)         //fun�ao para imprimir para o pc o valor do buffer
{
	uint8_t i = 0;
	while(buffer[i] != '\0')
	{
		while((UCSR1A & 1<<UDRE1) == 0);
		UDR1 = buffer[i];     //envio de caracter a caracter para o registo da comunica�ao serie
		i++;
	}
}


int LerDadosAdC (void)
{
	unsigned char leituraH;
	
	ADCSRA = ADCSRA | 0b01000000;
	while((ADCSRA & (1<<ADSC)) !=0 );
	
	leituraH= ADCH;
	return leituraH;
}

void LerPot (void)
{
	unsigned char i=0;
	valor_ler_ad=0;
	while(i<4)
	{
		valor_ler_ad=valor_ler_ad + LerDadosAdC();
		i++;
	}

	medAd=(valor_ler_ad/4);

	if((medAd>=0) && (medAd<=51))
	{
		CasoBotao();
	}
	else if ((medAd>=102) && (medAd<=152))
	{
		CasoTeclado();
	}
	else if ((medAd>=204) && (medAd<=255))
	{
		CasoAcelerometro();
	}

}

void ler_eixo_x(void)
{
	TWCR=(1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
	
	while (!(TWCR & (1<<TWINT)));
	if((TWSR & 0xF8) != TW_START)
	//send_message("erro");
	
	TWDR=0xD2;
	TWCR=(1<<TWINT)|(1<<TWEN);
	
	while (!(TWCR & (1<<TWINT)));
	if((TWSR & 0xf8) != TW_MT_SLA_ACK)
	//send_message("erro");

	TWDR=0x3B;
	TWCR=(1<<TWINT)|(1<<TWEN);
	
	while (!(TWCR & (1<<TWINT)));
	if((TWSR & 0xf8) != TW_MT_DATA_ACK)
	//send_message("erro");
	
	TWCR=(1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
	
	while (!(TWCR & (1<<TWINT)));
	if((TWSR & 0xf8) != TW_REP_START)
	//send_message("erro");
	
	TWDR=0xD3;
	TWCR=(1<<TWINT)|(1<<TWEN);
	
	while (!(TWCR & (1<<TWINT)));
	if((TWSR & 0xf8) != TW_MR_SLA_ACK)
	//send_message("erro");
	
	TWCR=(1<<TWINT)|(1<<TWEN)|(1<<TWEA);
	
	while (!(TWCR & (1<<TWINT)));
	if((TWSR & 0xf8) != TW_MR_DATA_ACK)
	//send_message("erro");
	
	eixoX=TWDR;
	
	TWCR=(1<<TWINT)|(1<<TWEN);
	
	while (!(TWCR & (1<<TWINT)));
	if((TWSR & 0xf8) != TW_MR_DATA_NACK)
	//send_message("erro");
	
	
	TWCR=(1<<TWINT)|(1<<TWSTO)|(1<<TWEN);
	//return;
}


void twi_func(void)
{
	TWCR=(1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
	
	while ( ! (TWCR & (1<<TWINT)));
	if((TWSR & 0xf8) != TW_START)
	send_message("erro");
	
	TWDR=0xD2;
	TWCR=(1<<TWINT)|(1<<TWEN);
	
	while (!(TWCR & (1<<TWINT)));
	if((TWSR & 0xf8) != TW_MT_SLA_ACK)
	send_message("erro");
	
	TWDR=0x6B;
	TWCR=(1<<TWINT)|(1<<TWEN);
	
	while (!(TWCR&(1<<TWINT)));
	if((TWSR & 0xf8) != TW_MT_DATA_ACK)
	send_message("erro");
	
	TWDR=0x00;
	TWCR=(1<<TWINT)|(1<<TWEN);
	
	while (!(TWCR & (1<<TWINT)));
	if((TWSR & 0xf8) != TW_MT_DATA_ACK)
	send_message("erro");
	
	TWCR=(1<<TWINT)|(1<<TWSTO)|(1<<TWEN);
}


