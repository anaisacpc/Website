/*
 * Microsatelite.c
 */ 

#define F_CPU 16000000UL
#define dht22_PINPL0        //ligado ao 0 do porto L para poder ter o input capture (digital pin 49)
#define R	1
#define W	0
#define BMP 0b10001110 
#define MPU 0b00001111 


// BIBLIOTECAS UTILIZADAS
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdint.h>
#include <util/delay.h>
#include <math.h>
//#include "../dht22.h"


//DEFINIR VARIAVEIS GLOBAIS

/* Variaveis para a comunicaçao serie */
char buffer[15];


/* Variaveis para o sensor DHT22 */
volatile uint16_t tempo1 = 0;
volatile uint16_t tempo2 = 0;
volatile uint16_t overflow = 0;
volatile uint8_t flanco = 0;
volatile uint32_t OneWire_vect[45];
volatile uint8_t j=0;

float ler_temperatura_dht22 = 0;
float ler_humidade_dht22 = 0;

uint32_t dif_tempo = 0;

/* Variaveis para o sensor BMP280 */
float var1 = 0;
float var2 = 0;
float T=0;
float t_fine=0;

uint32_t p=0;

int roomTemp_offset = 21;
float Temp_sensitivity = 333.87; 

float tempauxMPU = 0;
float tempauxMPU1 = 0;
float tempauxMPU2 = 0;

typedef struct BMP280_Temperatura{
	unsigned short T1;
	short T2;
	short T3;
}BMP280_Temperatura_t;

typedef struct BMP280_Pressao{
	unsigned short P1;
	short P2;
	short P3;
	short P4;
	short P5;
	short P6;
	short P7;
	short P8;
	short P9;
}BMP280_Pressao_t;

BMP280_Temperatura_t dig_T = {};
BMP280_Pressao_t dig_P = {};
	
//INICIALIZAÇÕES DA COMUNICAÇAO SERIE
void usart_config()
{	
	UBRR0H = 0;											//Configurações da usart0
	UBRR0L = 16;										//representa os 19200bps
	UCSR0A = (1<<U2X0);									//BAUD em modo assincrono com dobro da velocidade
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);						//set da transmissao e recepcao e respectiva interrupcao
	UCSR0C = (1<<UCSZ01)|(1<<UCSZ00);					// modo assincrono, sem paridade, 1 stop bit + 8 dados	
}

void usart_gps()
{
	UBRR1H = 0;											//Configurações da usart1
	UBRR1L = 16;										//representa os 19200bps
	UCSR1A = (1<<U2X1);									//BAUD em modo assincrono com dobro da velocidade
	UCSR1B = (1<<RXEN1)|(1<<TXEN1);						//set da transmissao e recepcao e respectiva interrupcao
	UCSR1C = (1<<UCSZ11)|(1<<UCSZ10);					// modo assincrono, sem paridade, 1 stop bit + 8 dados
}

void mostrapc(char *buffer)								//funçao para imprimir para o pc o valor do buffer
{
	uint8_t i = 0;
	while(buffer[i] != '\0')
	{
		while((UCSR0A & 1<<UDRE0) == 0);
		UDR0 = buffer[i];     //envio de caracter a caracter para o registo da comunicaçao serie
		i++;
	}
}


//INICIALIZAÇÕES DO MICROCONTROLADOR (ATMEGA2560)
void init_io()
{
	
	//dht22	
	PORTL = 0b00000001;		// colocar o pino do sensor a low
	DDRL  = 0b00000000;		// PortL 1 vai receber dados do dht22

	
	//bmp280 e mpu9250
	PORTB = 0b10001111;    // PORTB onde é ligado a comunicaçao SPI e os chip select de ambos sensores
	DDRB  = 0b10000111;    //como é um sensor "slave" MOSI/SCK/CS ficam a 1
	SPCR  = 0b01010001;    //configuraçoes do micro para comunicaçao SPI, clock = 16Mhz/16 
	
	usart_config();
	
	
	
	//Inicializaçao timer 4 para uso das propriedades do input capture
	TCCR4A = 0b00000000;    //timer4A modo normal
	TCCR4B = 0b01000000;	//enable do interrupt capture no flanco ascendente com prescaler=0
	TIMSK4  = 0b00100001;	//enable da interrupçao do input capture

	sei();					//ativação das interrupçoes globais
}

//INTERRUPÇÕES


ISR(TIMER4_CAPT_vect)						//interrupçao para input capture
{
	OneWire_vect[j]=ICR4+(overflow*65536);						//guarda timer a q encontrou o flanco
	j++;
}

ISR(TIMER4_OVF_vect)
{
	overflow++;
}


ISR(USART1_RX_vect)						//interrupçao para usart1
{
	char a = UDR1;
	buffer[0] = a;
	buffer[1] = '\0';
	mostrapc(buffer);
}


//FUNÇÕES PARA O DHT22

void ImpulsoDHT22()     //dar um impulso inicial ao sensor dht22 para q este começe a leitura de dados
{
	PORTL &= 0b11111110;	// colocar o pino do sensor a low
	DDRL |=0b00000001;
	_delay_ms(1);		//tempo de espera entre estados
	PORTL |= 0b00000001;
	DDRL &= 0b11111110;
	TCCR4B = 0b10000010;	//enable do interrupt capture no flanco descendente com prescaler=8
	overflow=0;
}

void dht22()
{
	float sum_humidade;
	float sum_temperatura;
	
	uint8_t cnt_bits = 0;
	uint8_t dif_temp[45];
	uint8_t vetor_humidade_relativa[16];
	uint8_t sum_check_sum = 0;
	uint8_t sum_humidade_check1 = 0;
	uint8_t sum_temperatura_check1 = 0;
	uint8_t auxH = 0;
	uint8_t auxT = 0;
	uint8_t auxC = 0;
	
	uint16_t sum_humidade_check0 = 0;
	uint16_t sum_temperatura_check0 = 0;
	
	int8_t vetor_temperatura[16];
	int8_t vetor_check_sum[8];
	//if(dht22_on == 0){
		j=0;
		ImpulsoDHT22();							//reset ao contador de overflow
		while(j<43);
		cnt_bits = 0;
		TIMSK4  = 0b00000001;
		for (j=0;j<42;j++)
		{
			if( ((OneWire_vect[j+1]-OneWire_vect[j])/2) > 100   )
			{
				dif_temp[j]=1;
			}
			else
			{
				dif_temp[j]=0;
			}
		}
		
		for (cnt_bits= 0; cnt_bits <16 ; cnt_bits++)
		{
			vetor_humidade_relativa[cnt_bits] = dif_temp[cnt_bits+2];
			vetor_temperatura[cnt_bits] = dif_temp[cnt_bits + 18];
			if (cnt_bits<8)
			{
				vetor_check_sum[cnt_bits] = dif_temp[cnt_bits + 34];
			}
		}
			
		for (int b=0; b<8; b++)
		{
			sum_humidade_check0 +=  (vetor_humidade_relativa[b]*(uint16_t)(pow(2,(15-b))+0.5));
			sum_humidade_check1 += ( vetor_humidade_relativa[b+8]*(uint8_t)(pow(2,(7-b))+0.5));
			sum_temperatura_check0 +=  (vetor_temperatura[b]*(uint16_t)(pow(2,(15-b))+0.5));
			sum_temperatura_check1 +=  (vetor_temperatura[b+8]*(uint8_t)(pow(2,(7-b))+0.5));
			sum_check_sum +=  vetor_check_sum[b]*(uint8_t)(pow(2,(7-b))+0.5);
			
			auxH= sum_humidade_check0/256;
			auxT = sum_temperatura_check0/256;
		}
	
		sum_humidade =  (sum_humidade_check0 + sum_humidade_check1);
		sum_temperatura =  (sum_temperatura_check0 + sum_temperatura_check1);
		auxC = auxH+sum_humidade_check1+auxT+sum_temperatura_check1-sum_check_sum;

		sprintf(buffer, "\n Dados sensor DHT22 \n ");
		mostrapc(buffer);
		sprintf(buffer, "\n Humidade: %.2f , Temperatura: %.2f , soma: %u , auxSoma: %u \n "  ,(sum_humidade/10) , (sum_temperatura/10) , sum_check_sum , auxC);
		mostrapc(buffer);
		
		sum_check_sum =0; sum_humidade_check0=0; sum_humidade_check1=0; sum_temperatura_check0=0; sum_temperatura_check1=0;
		auxC =0;
		sum_humidade =0; sum_temperatura=0; //reset dos somadores antes da prox leitura
		TIMSK4  = 0b00100001;
		_delay_ms(2500);
		//dht22_on = 1;}

}

//COMUNICAÇAO SPI
void spi_com(uint8_t rw, uint8_t address, uint8_t *data, uint8_t sensor)
{
	int valor = 0;
	int valor1 = 0;
	int valor2 = 0;
	int valor3 = 0;
	
	PORTB = sensor;
	if (rw==W)
	SPDR = address & (~0x80);	//onde se vai escrever
	else
	SPDR = address | (0x80);
	
	while(!(SPSR & (1 << SPIF)));
	valor = SPSR;
	valor1= SPDR;

	if (rw==W)
	SPDR = *data;  //o q se vai escrever
	else
	SPDR = address;
	
	while(!(SPSR & (1 << SPIF)));
	valor2 = SPSR;
	valor3 = SPDR;
	
	PORTB |= 0x81;
	*data = valor3;
}

//FUNÇÕES PARA O BMP280
void config_sensor_bmp()
{	
	
	uint8_t data = 0;
	
	data=0b00100111;
	spi_com(W, 0xF4, &data, BMP);				//escrita no registo 0xf4 para ctrl_meas (osrs_t / orsr_p/mode)

	data = 0b00100100;
	spi_com(W, 0xF5, &data, BMP); //escrita no registo 0xf5 para config (t_sb / filtro/spi_3w)

}

void compensar_temperatura()
{
	uint8_t data = 0;	
	uint8_t t1_msb =0;
	uint8_t t1_lsb =0;
	uint8_t t2_msb =0;
	uint8_t t2_lsb =0;
	uint8_t t3_msb =0;
	uint8_t t3_lsb =0;	
	
	/********** LEITURA REGISTOS DA TEMPERATURA ********/
	
	spi_com(R, 0x88, &data, BMP);			//ler temperatura_lsb
	t1_lsb = data;		
	spi_com(R, 0x89, &data, BMP);			//ler temperatura_msb
	t1_msb = data;
	
	spi_com(R, 0x8A, &data, BMP);			//ler temperatura_msb
	t2_lsb = data;	
	spi_com(R, 0x8B, &data, BMP);			//ler temperatura_msb
	t2_msb = data;
	
	spi_com(R, 0x8C, &data, BMP);			//ler temperatura_msb
	t3_lsb = data;	
	spi_com(R, 0x8D, &data, BMP);			//ler temperatura_msb
	t3_msb = data;
	
	/********** COMPENSACAO ********/
	
	dig_T.T1 = (t1_msb<<8) + t1_lsb;	
	dig_T.T2 = (t2_msb<<8)+t2_lsb;
	dig_T.T3 = (t3_msb<<8) + t3_lsb;
	
}

void compensar_pressao()
{
	uint8_t data = 0;	
	uint8_t p1_msb =0;
	uint8_t p1_lsb =0;
	uint8_t p2_msb =0;
	uint8_t p2_lsb =0;
	uint8_t p3_msb =0;
	uint8_t p3_lsb =0;
	uint8_t p4_msb =0;
	uint8_t p4_lsb =0;
	uint8_t p5_msb =0;
	uint8_t p5_lsb =0;
	uint8_t p6_msb =0;
	uint8_t p6_lsb =0;
	uint8_t p7_msb =0;
	uint8_t p7_lsb =0;
	uint8_t p8_msb =0;
	uint8_t p8_lsb =0;
	uint8_t p9_msb =0;
	uint8_t p9_lsb =0;
	
	
	/********** LEITURA REGISTOS COMPENSAÇAO DA PRESSAO ********/
	
	spi_com(R, 0x8E, &data, BMP);	
	p1_lsb = data;		
	spi_com(R, 0x8F, &data, BMP);	
	p1_msb = data;	

	spi_com(R, 0x90, &data, BMP);		
	p2_lsb = data;	
	spi_com(R, 0x91, &data, BMP);			
	p2_msb = data;
	
	spi_com(R, 0x92, &data, BMP);			
	p3_lsb = data;	
	spi_com(R, 0x93, &data, BMP);		
	p3_msb = data;
	
	spi_com(R, 0x94, &data, BMP);			
	p4_lsb = data;
	spi_com(R, 0x95, &data, BMP);			
	p4_msb = data;
	
	spi_com(R, 0x96, &data, BMP);		
	p5_lsb = data;	
	spi_com(R, 0x97, &data, BMP);			
	p5_msb = data;
	
	spi_com(R, 0x98, &data, BMP);		
	p6_lsb = data;	
	spi_com(R, 0x99, &data, BMP);			
	p6_msb = data;
	
	spi_com(R, 0x9A, &data, BMP);			
	p7_lsb = data;	
	spi_com(R, 0x9B, &data, BMP);			
	p7_msb = data;
	
	spi_com(R, 0x9C, &data, BMP);			
	p8_lsb = data;	
	spi_com(R, 0x9D, &data, BMP);			
	p8_msb = data;
	
	spi_com(R, 0x9E, &data, BMP);			
	p9_lsb = data;	
	spi_com(R, 0x9F, &data, BMP);			
	p9_msb = data;
	

	/********** COMPENSACAO ********/
	
	dig_P.P1 = (p1_msb<<8) + p1_lsb;
	dig_P.P2 = (p2_msb<<8) + p2_lsb;
	dig_P.P3 = (p3_msb<<8) + p3_lsb;
	dig_P.P4 = (p4_msb<<8) + p4_lsb;
	dig_P.P5 = (p5_msb<<8) + p5_lsb;
	dig_P.P6 = (p6_msb<<8) + p6_lsb;
	dig_P.P7 = (p7_msb<<8) + p7_lsb;
	dig_P.P8 = (p8_msb<<8) + p8_lsb;
	dig_P.P9 = (p9_msb<<8) + p9_lsb;

}

void ler_temperatura()
{
	uint8_t data = 0;
	uint8_t temp_msb=0;
	uint8_t temp_lsb=0;
	uint8_t temp_xlsb=0;
	uint32_t temp = 0;
	uint32_t auxt =0;
	
	/********** LEITURA REGISTOS DA TEMPERATURA ********/
	
	spi_com(R, 0xFA, &data, BMP);			//ler temperatura_msb
	temp_msb = data;
	
	spi_com(R, 0xFB, &data, BMP);			//ler temperatura_lsb
	temp_lsb = data;
	
	spi_com(R, 0xFC, &data, BMP);			//ler temperatura_xlsb
	temp_xlsb = data;
	
	/********** CALCULO DA TEMPERATURA ********/
	
	temp= (temp_msb*65536) +(temp_lsb*256) + temp_xlsb ;
	auxt = temp>>4 ;
	
	var1 = (((double)auxt)/16384.0 - ((double)dig_T.T1)/1024.0) * ((double)dig_T.T2);
	var2 = ((((double)auxt)/131072.0-((double)dig_T.T1)/8192.0)*(((double)auxt)/131072.0-((double)dig_T.T1)/8192.0))*((double)dig_T.T3);
	t_fine = var1+var2;
	T = (long signed int)(var1+var2)/5120.0;
	
	sprintf(buffer, "\n Dados sensor BMP280 \n ");
	mostrapc(buffer);
	sprintf(buffer, "temperatura = %.2lf  ºC \n", T);
	mostrapc(buffer);
		
}

void ler_pressao()
{
	uint8_t data = 0;
	uint32_t pressao = 0;
	uint32_t auxp =0;
	int press_msb=0;
	int press_lsb=0;
	int press_xlsb=0;
	double tGA = 44330;  //temperatura nivel mar (kelvin)/constante gradiente adiabático
	double cAr = 0.19; //caracteristicas ar: 1/((gravidade*massa molar do ar)/(gas ideal * gradiente adiabático))
	double pMar = 101325; // pressao(em Pascal) ao nivel do mar, h=0
	double altitude = 0;
		
	/********** LEITURA REGISTOS DA PRESSAO ********/
	
	spi_com(R, 0xF7, &data, BMP);			//ler pressao_msb
	press_msb = data;
	
	spi_com(R, 0xF8, &data, BMP);			//ler pressao_lsb
	press_lsb = data;
	
	spi_com(R, 0xF9, &data, BMP);			//ler pressao_xlsb
	press_xlsb = data;
	
	/********** CALCULO DA PRESSAO ********/
	
	pressao= (press_msb*65536) +(press_lsb*256) + press_xlsb ;
	auxp = pressao>>4 ;

	var1 = ((double)t_fine/2.0)-64000.0;
	var2 = (var1*var1*((double)dig_P.P6)/32768.0);
	var2 = var2+var1*((double)dig_P.P5)*2.0;
	var2 = var2/4.0+(((double)dig_P.P4)*65536.0);
	var1 = (((double)dig_P.P3)*var1*var1/524288.0+((double)dig_P.P2)*var1)/524288.0;
	var1 = (1.0+var1/32768.0)*((double)dig_P.P1);
	p = 1048576-(double)auxp;
	p = (p-(var2/4096.0))*6250.0/var1;
	var1 = ((double)dig_P.P9)*p*p/2147483648.0;
	var2 = p*((double)dig_P.P8)/32768.0;	
	p = p + (var1 + var2 + ((double)dig_P.P7))/16;
		
	sprintf(buffer, " pressao: %lu  Pa\n", p );
	mostrapc(buffer);
	
	/********** CALCULO DA ALTITUDE ********/ 
	
	altitude = tGA * (1- pow((p/pMar),cAr));
	
	sprintf(buffer, " altitude: %.2lf  m\n", altitude );
	mostrapc(buffer);
	
}

void bmp()
{
	//SPCR  = 0b01011101;	   //configuraçoes do micro para comunicaçao SPI, clock = 16Mhz/16 (bmp)
		config_sensor_bmp();	
		compensar_temperatura();
		compensar_pressao();
		ler_temperatura();
		ler_pressao();
	//SPCR  = 0b01010001;	   //configuraçoes do micro para comunicaçao SPI, clock = 16Mhz/16 = 1Mhz
}

void config_sensor_mpu()
{
	uint8_t data = 0;
	data=0x01;
	spi_com(W, 0x6B, &data, MPU);				//escrita pwr_magmt_1
}

void mpu()
{
	int valor = 0;
	int valor1 = 0;
	int valor2 = 0;
	int valor3 = 0;
	
	uint8_t data = 0;
	
	uint16_t acl_sensibility = 16386;	
	uint16_t acl_x_aux = 0;
	uint16_t acl_y_aux = 0;
	uint16_t acl_z_aux = 0;
	int16_t acl_x =0;
	int16_t acl_y =0;
	int16_t acl_z =0;
	
	int16_t temp_mpu =0;

	uint8_t  gir_sensibility = 131;
	int16_t gir_x =0;
	int16_t gir_y =0;
	int16_t gir_z =0;

	
	int16_t mag_x =0;
	int16_t mag_y =0;
	int16_t mag_z =0;
	uint8_t mag_s_x = 0;
	uint8_t mag_s_y = 0;
	uint8_t mag_s_z = 0;
	
	config_sensor_mpu();
			
		
		//ler acelerometro
		spi_com(R, 0x3B, &data, MPU);
		acl_x= data;
		spi_com(R, 0x3C, &data, MPU);
		acl_x = (acl_x<<8) + data;
			
		spi_com(R, 0x3D, &data, MPU);
		acl_y= data;
		spi_com(R, 0x3E, &data, MPU);
		acl_y = (acl_y<<8) + data;
			
		spi_com(R, 0x3F, &data, MPU);
		acl_z= data;
		spi_com(R, 0x40, &data, MPU);
		acl_z = (acl_z<<8) + data;
					
		//valores finais em m/s^2
		acl_x_aux = (uint16_t)(((float)(acl_x/32768.0))*acl_sensibility);
		acl_y_aux = (uint16_t)(((float)(acl_y/32768.0))*acl_sensibility);
		acl_z_aux = (uint16_t)(((float)(acl_z/32768.0))*acl_sensibility);
					
		_delay_ms(10);
		
		//ler temperatura		
		spi_com(R, 0x41, &data, MPU);
		temp_mpu= data;
		tempauxMPU = (temp_mpu-roomTemp_offset)/Temp_sensitivity;
		
		spi_com(R, 0x42, &data, MPU);
		temp_mpu= data;
		tempauxMPU1 =((temp_mpu-roomTemp_offset)/Temp_sensitivity); 
		
		tempauxMPU2 = tempauxMPU+ tempauxMPU1 + 21;
		
		_delay_ms(100);
		
		//ler giroscopio	
		spi_com(R, 0x43, &data, MPU);
		gir_x= data;	
		spi_com(R, 0x44, &data, MPU);
		gir_x = (gir_x<<8) + data;	
			
		spi_com(R, 0x45, &data, MPU);
		gir_y= data;
		spi_com(R, 0x46, &data, MPU);
		gir_y = (gir_y<<8) + data;
			
		spi_com(R, 0x47, &data, MPU);
		gir_z= data;
		spi_com(R, 0x48, &data, MPU);
		gir_z = (gir_z<<8) + data;
		
		//valores finais em rad/s
		gir_x = (uint16_t)(((float)(gir_x/32768.0))*gir_sensibility);
		gir_y = (uint16_t)(((float)(gir_y/32768.0))*gir_sensibility);
		gir_z = (uint16_t)(((float)(gir_z/32768.0))*gir_sensibility);
		
		
		/* ACEDER AO MAGNETOMETRO */	
		//enable ao acesso do magnetometro
		data=0x20;
		spi_com(W, 0x6A, &data, MPU);
		data=0x0D;
		spi_com(W, 0x24, &data, MPU);

		data=0x0C;
		spi_com(W, 0x25, &data, MPU);
		data=0x0B;						//CONFIG REGISTO CTRL2
		spi_com(W, 0x26, &data, MPU);
		data=0x01;
		spi_com(W, 0x63, &data, MPU);
		data=0x81;
		spi_com(W, 0x27, &data, MPU);
		_delay_us(100);
	
		data=0x0C;
		spi_com(W, 0x25, &data, MPU);
		data=0x0A;						//CONFIG REGISTO CTRL1
		spi_com(W, 0x26, &data, MPU);
		data=0x11;
		spi_com(W, 0x63, &data, MPU);
		data=0x81;
		spi_com(W, 0x27, &data, MPU);
		_delay_us(100);
	
		//leitura sensibilidade eixo x
		data=0x0C|0x80;
		spi_com(W, 0x25, &data, MPU);
		data=0x10;
		spi_com(W, 0x26, &data, MPU);
		data=0x81;
		spi_com(W, 0x27, &data, MPU);
		_delay_ms(10);
		spi_com(R, 0x49, &data, MPU);
		mag_s_x = data;
		
		//leitura sensibilidade eixo y
		data=0x0C|0x80;
		spi_com(W, 0x25, &data, MPU);
		data=0x11;
		spi_com(W, 0x26, &data, MPU);
		data=0x81;
		spi_com(W, 0x27, &data, MPU);
		_delay_ms(10);
		spi_com(R, 0x49, &data, MPU);
		mag_s_y = data;
		
		//leitura sensibilidade eixo z
		data=0x0C|0x80;
		spi_com(W, 0x25, &data, MPU);
		data=0x12;
		spi_com(W, 0x26, &data, MPU);
		data=0x81;
		spi_com(W, 0x27, &data, MPU);
		_delay_ms(10);
		spi_com(R, 0x49, &data, MPU);
		mag_s_z= data;
	
		//ciclo para ler quando o sensor está pronto a enviar os dados
		do 
		{
			data=0x0C|0x80;
			spi_com(W, 0x25, &data, MPU);
			data=0x02;
			spi_com(W, 0x26, &data, MPU);
			data=0x81;
			spi_com(W, 0x27, &data, MPU);
			_delay_us(300);
			spi_com(R, 0x49, &data, MPU);
		} while ((data&0x01)!=1);
		
		//leitura dos registos de dados
		
		//ler eixo x 
		data=0x0C|0x80;
		spi_com(W, 0x25, &data, MPU);
		data=0x04;
		spi_com(W, 0x26, &data, MPU);
		data=0x81;
		spi_com(W, 0x27, &data, MPU);
		_delay_us(300);
		spi_com(R, 0x49, &data, MPU);
		
		mag_x=data;

		data=0x0C|0x80;
		spi_com(W, 0x25, &data, MPU);
		data=0x03;
		spi_com(W, 0x26, &data, MPU);
		data=0x81;
		spi_com(W, 0x27, &data, MPU);
		_delay_us(300);
		spi_com(R, 0x49, &data, MPU);
		
		mag_x=(mag_x<<8)+data;
		mag_x = (uint16_t)(mag_x * ((((float)(mag_s_x-128)*0.5)/mag_s_x)+1));

		//ler eixo y
		
		data=0x0C|0x80;
		spi_com(W, 0x25, &data, MPU);
		data=0x06;
		spi_com(W, 0x26, &data, MPU);
		data=0x81;
		spi_com(W, 0x27, &data, MPU);
		_delay_us(300);
		spi_com(R, 0x49, &data, MPU);
		
		mag_y=data;
		
		data=0x0C|0x80;
		spi_com(W, 0x25, &data, MPU);
		data=0x05;
		spi_com(W, 0x26, &data, MPU);
		data=0x81;
		spi_com(W, 0x27, &data, MPU);
		_delay_us(300);
		spi_com(R, 0x49, &data, MPU);
		
		mag_y=(mag_y<<8)+data;
		mag_y = (uint16_t)(mag_y * ((((float)(mag_s_y-128)*0.5)/mag_s_y)+1));
		
		//leitura z
		
		data=0x0C|0x80;
		spi_com(W, 0x25, &data, MPU);
		data=0x08;
		spi_com(W, 0x26, &data, MPU);
		data=0x81;
		spi_com(W, 0x27, &data, MPU);
		_delay_us(300);
		spi_com(R, 0x49, &data, MPU);
		
		mag_z=data;

		data=0x0C|0x80;
		spi_com(W, 0x25, &data, MPU);
		data=0x07;
		spi_com(W, 0x26, &data, MPU);
		data=0x81;
		spi_com(W, 0x27, &data, MPU);
		_delay_us(300);
		spi_com(R, 0x49, &data, MPU);
		
		mag_z=(mag_z<<8)+data;
		mag_z = (uint16_t)(mag_z * ((((float)(mag_s_z-128)*0.5)/mag_s_z)+1));

		
		sprintf(buffer, "\n Dados sensor MPU-9250 \n ");
		mostrapc(buffer);

		sprintf(buffer, "Acelerometro (m/s^2): \n ");
		mostrapc(buffer);
		sprintf(buffer, "%d \t %d \t %d\n", acl_x_aux, acl_y_aux , acl_z_aux);
		mostrapc(buffer);

		sprintf(buffer, "Temperatura (ºC): \n ");
		mostrapc(buffer);
		sprintf(buffer, "%.2f \n", tempauxMPU2);
		mostrapc(buffer);

		sprintf(buffer, "Giroscopio (rad/s): \n ");
		mostrapc(buffer);	
		sprintf(buffer, "%d \t %d \t %d\n", gir_x, gir_y , gir_z);
		mostrapc(buffer);
		
		sprintf(buffer, "Magnetometro (uT): \n ");
		mostrapc(buffer);
		sprintf(buffer, "%d \t %d \t %d\n", mag_x, mag_y , mag_z);
		mostrapc(buffer);


}



int main(void)
{
    init_io(); // chamar funcao de inicializacoes
    while (1) 
    {
	//	dht22();
	//	bmp();
	//	mpu();
		buffer[0] = 0x32;
		buffer[1] = '\0';
		mostrapc(buffer);

    }
}


