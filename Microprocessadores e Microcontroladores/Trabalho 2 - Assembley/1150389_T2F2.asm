; Trabalho2-F1.asm
; Created: 30/11/2018 
; Author : anaisa;

//defini�ao de "variaveis" aos registos (endere�amento imediato)
	.def tmp = r16                 
	.def contador20ms = r17
	.def flag = r18
	.def disp = r19
	.def contador500ms = r20
	.def contador5s = r21
	.def vald1 = r22
	.def vald2 = r23
	.def nlancamento = r24

//defini�ao registos inferiores a r16 (endere�amento direto)
	.def valfrq1 = r14
	.def valfrq2 = r15


//registo das flags que irao ser utilizadas ao longo do programa
	.equ flag20ms = 0
	.equ flag500ms = 1
	.equ flag5s = 2
	.equ flag100ms = 3
	.equ piscadisp1 = 4
	.equ piscadisp2 = 5
//inicializa��o do microcontrolador
	.cseg 
	.org 0									//vetor de reset
		jmp main							
	.org 0x1E								//vetor de interrup�ao do tc0 por compara�ao
		jmp int_tc0							//salta para intc0
	.org 0x46

//inicializa��es de portos e pinos
init_io:

	ldi tmp, 0b11000000 					//configura�ao binaria para o switches a ser ativo
	out ddrd,tmp							//configura o porto D, como saidas para o bit 7 e 6, e os restantes como entradas
	out portd,tmp							//seleciona o display mais a direita (pino 7 e 6) desactiva os pull-ups internos nos restantes
	ser tmp									//coloca todos os bits de r16 a 1
	out ddrc,tmp							//configura o porto C como saida
	out portc,tmp							//coloca o display apagado
	out ddra, tmp
	ldi tmp, 0xff
	out porta, tmp

						/**************************************TCR0***********************************************/

	ldi tmp, 0b00001110						//configura�ao do tc0, em modo 2 com prescaler de 256
	out tccr0 , tmp
	ldi tmp , 124							//configura�ao do ocr0=124
	out ocr0 , tmp

	ldi tmp, 0b00000010						//configura�ao de mascara para uso em flags
	out timsk, tmp
						/*********************Valores Default para flags e contadores****************************/
	clr flag
	clr disp							
	clr contador20ms	
	clr contador500ms
	clr contador5s
	clr nlancamento
	clr vald1
	clr vald2
	ldi tmp, 10
	mov valfrq1,tmp
	ldi tmp, 25
	mov valfrq2,tmp
	clr tmp

	sei										//ativa�ao das interrupcoes a nivel global
	ret

//load do programa
main:
	ldi tmp,low(RAMEND)					//carrega a stack na ultima linha de memoria de dados
	out spl,tmp
	ldi tmp,high(RAMEND)
	out sph,tmp
	call init_io						//chama fun�ao para inicializa��o 

//ciclo infinito para inicio e stop do programa
ciclo:
	sbic pind,0			//testa sw1, e se foi premido
	rjmp ciclo			//enquanto nao for continua a testar continuamente se foi
	ldi vald1,0
	ldi vald2,0
		cpi nlancamento, 8				//compara se ja atingiu 8 lan�amentos 
		brne novolancamento
		ldi nlancamento,0				//reset numero jogada se ja se tiverem atingido as 8 jogadas
		ldi tmp, 0xff					//reset leds
		out porta,tmp
		novolancamento:
			inc nlancamento

				/*********************************************"fun�oes" de roleta *********************************************/
startD1:
	call mudaf			//chama "fun�ao" para testar a mudan�a de frequencia
	inc vald1			//incrementa valor a ser mostrado no display 1
	call esperar1		//faz o delay correspondente a frequencia alcan�ada
	cpi vald1 , 6		//compara�ao se a variavel � o m�ximo do dado
	brne startD1		//se dado=!6 volta ao inicio para incrementar e se dado=6
	ldi vald1,0			//limpa dado e volta ao inicio do ciclo de incrementa�ao 
	rjmp startD1		

startD2:
	call mudaf			//chama "fun�ao" para testar a mudan�a de frequencia
	inc vald2			//incrementa valor a ser mostrado no display 2
	call esperar2		//faz o delay de 20ms
	cpi vald2 , 6		//compara�ao se � variavel � o m�ximo do dado
	brne startD2		//se dado=!6 volta ao inicio para incrementar e se dado=6
	ldi vald2,0			//limpa dado e volta ao inicio do ciclo de incrementa�ao 
	rjmp startD2

				/************************************"fun�oes" para espera entre digitos *************************************/ 
 esperar1:
	sbis pind,3					//testa o sw4, se foi premido
	call stop1					//se premido, chama a fun�ao de stop
	sbrs flag, flag20ms			//testa se flag=1
	jmp esperar1				//se flag=0, continua o ciclo de espera
	cbr flag, (1<<flag20ms)		//se flag=1, mete novamente p bit do registo flag a 0 atrav�s da mascara 1>>flag20ms
	ret

 esperar2:
	sbis pind,3					//testa o sw4, se foi premido
	call stop2					//se premido, chama a fun�ao de stop
	sbrs flag, flag20ms			//testa se flag=1
	jmp esperar2				//se flag=0, continua o ciclo de espera
	cbr flag, (1<<flag20ms)	    //se flag=1, mete novamente p bit do registo flag a 0 atrav�s da mascara 1>>flag20ms
	ret

			/************************************"fun�oes" para stop entre displays *************************************/ 
stop1:
	clr flag                       //limpa registos
	clr contador20ms
	clr contador500ms
	clr contador5s
	pisca1:						   //ciclo para acender display
		sbrs flag, flag500ms       //se tiver a 1 salta
		rjmp pisca1
		cbr flag , (1<<flag500ms)  //limpa o bit para voltar a 0
		apaga1:                    //ciclo para apagar display
			sbr flag,(1<<piscadisp1)    
			sbrs flag, flag500ms
			rjmp apaga1
	cbr flag , (1<<flag500ms)     //limpa o bit para voltar a 0
	cbr flag,(1<<piscadisp1)      //indica a "funcao" de imprimir para o display que ja o pode acender
	sbrs flag, flag5s             //testa se passou 5s 
	rjmp pisca1
	cbr flag , (1<<flag5s)       //limpa o bit para voltar a 0
	rjmp startD2				 //chama o ciclo para fazer a roleta no 2 display mais � direita


stop2:
	clr flag
	clr contador20ms
	clr contador500ms
	clr contador5s
	pisca2:
		sbrs flag, flag500ms       //se tiver a 1 salta
		rjmp pisca2
		cbr flag , (1<<flag500ms)  //limpa o bit para voltar a 0
		apaga2:
			sbr flag,(1<<piscadisp2)
			sbrs flag, flag500ms
			rjmp apaga2
			cbr flag , (1<<flag500ms)  //limpa o bit para voltar a 0
			cbr flag,(1<<piscadisp2)
		sbrs flag, flag5s              //testa se passou 5s 
		rjmp pisca2
	cbr flag , (1<<flag5s)             //limpa o bit para voltar a 0
	call ligacao	                   // LIGAR LEDS, DISPLAY     
	rjmp ciclo			               //fica preso no fim

//sele�ao dos leds acesos e qual o lan�amento do jogo
ligacao:
    ldi tmp, 0b00000000   //sele�ao display 1(+ � esquerda)
	out portd, tmp

	cpi nlancamento,1    //compara��o entre o numero de lancamento e decimal
	breq led1			//se comparacao verdadeira vai para led1

	cpi nlancamento,2
	breq led2

	cpi nlancamento,3
	breq led3

	cpi nlancamento,4
	breq led4

	cpi nlancamento,5
	breq led5

	cpi nlancamento,6
	breq led6

	cpi nlancamento,7
	breq led7

	cpi nlancamento,8
	breq led8

//envio de dados para acender led's respetivos
led1:
	ldi tmp, 0b01111111 
	out porta, tmp       
	ret

led2:
	ldi tmp, 0b00111111
	out porta, tmp
	ret

led3:
	ldi tmp, 0b00011111
	out porta, tmp
	ret

led4:
	ldi tmp, 0b00001111
	out porta, tmp
	ret

led5:
	ldi tmp, 0b00000111
	out porta, tmp
	ret

led6:
	ldi tmp, 0b00000011
	out porta, tmp
	ret

led7:
	ldi tmp, 0b00000001
	out porta, tmp
	ret

led8:
	ldi tmp, 0b00000000
	out porta, tmp
	ret

			/******************"fun�oes" para mudan�a de frequencia em que a roleta altera os numeros*****************/
mudaf:
	sbis pind,1 			//testa o sw2, se foi premido
	call f1   				//se premido, chama a fun�ao f1
	sbis pind,2 			//testa o sw3, se foi premido
	call f2				    //se premido, chama a fun�ao f2
	ret

f1:
	ldi tmp, 50
	mov valfrq1,tmp         //define quantas repeti��es irao ser feitas � interrup��o de 2ms, para atingir 100ms
	ldi tmp, 5
	mov valfrq2,tmp			//define quantas repeti��es irao ser feita�s, para atingir 500ms
	ret

f2:
	ldi tmp, 10
	mov valfrq1,tmp        //define quantas repeti��es irao ser feitas � interrup��o de 2ms, para atingir 20ms
	ldi tmp, 25
	mov valfrq2,tmp		   //define quantas repeti��es irao ser feita�s, para atingir 500ms
	ret 


			/**********************************defini�ao da interrup�ao do tc0*******************************/
int_tc0:
	inc disp
	call refreshdisp			//fun�ao de atualizar os displays com valor a ser impresso
	cpi disp,3
	brne interrupcao			
	ldi disp,0

	interrupcao:
	inc contador20ms                //incrementa contador sempre q vem a este ciclo
	cp contador20ms, valfrq1		//compara se ja incrementou o contador de acordo com o definido para a frequencia
	BRNE fimInt						//se nao incrementou salta
		ldi contador20ms, 0				//se incrementou , zera o contador
		sbr flag , (1<<flag20ms)		// coloca a flag de 20 ms a 1	
		inc contador500ms
		cp contador500ms, valfrq2       //compara se ja prefez os 500ms
		BRNE fimInt
			ldi contador500ms, 0
			sbr flag, (1<<flag500ms)
			inc contador5s
			cpi contador5s , 10         //prefazer os 5s
			BRNE fimInt
			sbr flag, (1<<flag5s)
			ldi contador5s, 0
	fimInt:							
		reti						    //retorna ao ponto onde foi interrompido e reativa as interrup�oes 

						/******************************configura��o dos displays**********************************/
//ciclo para os displays estarem "sempre" acesos
refreshdisp:						//configura qual o display a acender naquele instante
	cpi disp,1
		breq refreshdisp1
	cpi disp,2
		breq refreshdisp2			//chamada do ciclo correspondente a acender este display
	cpi disp,3
		breq refreshdisp3
	ret

//ciclo para o display + � direita piscar
refreshdisp1:
	sbrs flag,piscadisp1			//verifica se este display esta na altura de piscar e deve ficar apagado
	rjmp acendedisp1				//caso nao seja verdade salta para imprimir o valor
			ldi tmp, 0b11000000		//caso seja for�a a que o display continue apagado
			out portd,tmp
			ldi tmp, 0xFF			//carrega registo tempo com valores todos a 1
			out portc, tmp
	ret

	acendedisp1:
	ldi tmp, 0b11000000				//seleciona o display - mais a direita
	out portd, tmp
	mov tmp, vald1					//prepara o valor no registo tmp para imprimir
	call display					//imprime
	ret

//ciclo para o  2 display + � direita piscar
refreshdisp2:
	sbrs flag,piscadisp2
	rjmp acendedisp2
			ldi tmp, 0b10000000
			out portd,tmp
			ldi tmp, 0xFF        //carrega registo tempo com valores todos a 1
			out portc, tmp
	ret

	acendedisp2:
	ldi tmp, 0b10000000
	out portd, tmp
	mov tmp, vald2
	call display
	ret

//ciclo para acender display + � esquerda sem que este pisque 
refreshdisp3:
	ldi tmp, 0
	out portd, tmp				   //display apagado no inicio do jogo
	mov tmp, nlancamento	       //coloca n da jogada a ser impresso pelo display
	call display				   //impressao
	ret

//configura�ao de como acender cada numero no display de 7 segmentos 
display:
		cpi tmp,0						//corresponde a ficar apagado e n�o a imprimir 0
		breq disp0

		cpi tmp,1
		breq disp1

		cpi tmp,2
		breq disp2

		cpi tmp,3
		breq disp3

		cpi tmp,4
		breq disp4

		cpi tmp,5
		breq disp5

		cpi tmp,6
		breq disp6

		cpi tmp,7
		breq disp7

		cpi tmp,8
		breq disp8
disp0:
ldi tmp, 0xff
out portc, tmp
ret

disp1:
ldi tmp, 0xf9
out portc, tmp
ret

disp2:
ldi tmp, 0xa4
out portc, tmp
ret

disp3:
ldi tmp, 0xb0
out portc, tmp
ret

disp4:
ldi tmp, 0x99
out portc, tmp
ret

disp5:
ldi tmp, 0x92
out portc, tmp
ret

disp6:
ldi tmp, 0x82
out portc, tmp
ret

disp7:
ldi tmp, 0xf8
out portc, tmp
ret

disp8:
ldi tmp, 0x80
out portc, tmp
ret

