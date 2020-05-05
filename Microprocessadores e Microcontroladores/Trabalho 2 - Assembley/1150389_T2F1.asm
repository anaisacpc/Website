; Trabalho2-F1.asm
; Created: 16/11/2018 09:04:46
; Author : anaisa;

//definiçao de "variaveis" aos registos
	.def tmp = r16                 
	.def contador20ms = r17
	.def flag = r18
	.def dado = r19
	.def contador500ms = r20
	.def contador5s = r21

//registo das flags que irao ser utilizadas ao longo do programa
	.equ flag20ms = 0
	.equ flag500ms = 1
	.equ flag5s = 2

//inicialização do microcontrolador
	.cseg 
	.org 0									//vetor de reset
		jmp main							
	.org 0x1E								//vetor de interrupçao do tc0 por comparaçao
		jmp int_tc0							//salta para intc0
	.org 0x46

//inicializações de portos e pinos
init_io:

	ldi tmp, 0b11000000						//coloca os bits 7 e 6 a um e os bits 5 a 0 a zero no registo r16
	out ddrd,tmp							//configura o porto D, como saidas para o bit 7 e 6, e os restantes como entradas
	out portd,tmp							//seleciona o display mais a direita (pino 7 e 8) desactiva os pull-ups internos nos restantes
	ser tmp									//coloca todos os bits de r16 a 1
	out ddrc,tmp							//configura o porto C como saida
	out portc,tmp							//coloca o display apagado

						/**************************************TCR0***********************************************/

	ldi tmp, 0b00001110						//configuraçao do tc0, em modo 2 com prescaler de 256
	out tccr0 , tmp
	ldi tmp , 124							//configuraçao do ocr0=124
	out ocr0 , tmp

	ldi tmp, 0b00000010						
	out timsk, tmp							//ativar a interrupcao do TC0 por comparacao
						/*********************Valores Default para flags e contadores****************************/
	clr flag
	ldi dado,0								
	clr contador20ms	
	clr contador500ms
	clr contador5s
	sei										//ativaçao das interrupcoes a nivel global
	ret

//load do programa
main:
	ldi tmp,low(RAMEND)					//carrega a stack na ultima linha de memoria de dados
	out spl,tmp
	ldi tmp,high(RAMEND)
	out sph,tmp
	call init_io						//chama funçao para inicialização 

//ciclo infinito para inicio e stop do programa
ciclo:
	//switch_1:
		sbic pind,0			//testa sw1, e se foi premido, salta o ciclo e vai para a funçao start
		rjmp ciclo			//se nao premido retorna a testar o switch

//funçao de roleta
start:
	inc dado			//incrementa dado
	call display		//"imprime" numero no display
	call esperar		//faz o delay de 20ms
	cpi dado , 6		//comparaçao se á variavel é o máximo do dado
	brne start			//se dado!=6 volta ao inicio para incrementar e se dado=6
	ldi dado,0			//limpa dado e volta ao inicio do ciclo de incrementaçao 
	rjmp start

//funçao do delay de 20ms
 esperar:
	sbis pind,3				//testa o sw4, se foi premido
	call stop				//se premido, chama a funçao de stop
	sbrs flag, flag20ms		//testa se flag indicativa do cronometro de 20ms ter terminado
	jmp esperar				//se a flag estiver a zero continua o ciclo de espera
	cbr flag, (1<<flag20ms) //zera o bit do registo flag, que esta a ser apontado pela mascara 1>>flag20ms (e o bit, valor decimal a que corresponde a palavra flag20ms nos .def)
	ret

//funçao de stop
stop:
	clr flag				//reset flags e contadores
	clr contador20ms
	clr contador500ms
	clr contador5s
	pisca: 
		call display
		sbrs flag, flag500ms	   //se tiver a 1 salta
		rjmp pisca
		cbr flag , (1<<flag500ms)  //limpa o bit para voltar a 0
		apaga:
			ldi tmp, 0xFF          //carrega registo tempo com valores todos a 1
			out portc, tmp
			sbrs flag, flag500ms
			rjmp apaga
			cbr flag , (1<<flag500ms)  //limpa o bit para voltar a 0
		sbrs flag, flag5s              //testa se passou 5s 
		rjmp pisca
	cbr flag , (1<<flag5s)             //limpa o bit para voltar a 0
	call display					   //se passou os 5s, fica apenas com o numero aceso
	ldi dado, 0
	rjmp ciclo						   //ficar preso ao fim dos 5s sem piscar

//definiçao da interrupçao do tc0
int_tc0:
	//sbis pind,0
	//ldi dado,3
	inc contador20ms				   //incrementa contador sempre q vem a este ciclo
	cpi contador20ms, 10			   //compara se ja incrementou 10x o contador
	BRNE fimInt						   //se nao incrementou salta
		ldi contador20ms, 0			   //se incrementou , zera o contador
		sbr flag , (1<<flag20ms)	   // coloca a flag de 20 ms a 1
		inc contador500ms
		cpi contador500ms, 25          //compara se ja incrementou 25x para fazer os 500ms (pq ja passaram os 20ms de cada vez q incrementa este contador)
		BRNE fimInt
			ldi contador500ms, 0
			sbr flag, 1<<flag500ms
			inc contador5s
			cpi contador5s , 10
			BRNE fimInt
			sbr flag, 1<<flag5s
			ldi contador5s, 0

	fimInt:							
		reti						//retorna ao ponto onde foi interrompido e reativa as interrupçoes 

//configuração do display
display:
		cpi dado,1
		breq disp1

		cpi dado,2
		breq disp2

		cpi dado,3
		breq disp3

		cpi dado,4
		breq disp4

		cpi dado,5
		breq disp5

		cpi dado,6
		breq disp6

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
