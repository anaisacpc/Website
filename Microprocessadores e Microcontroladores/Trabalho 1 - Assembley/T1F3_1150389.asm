; trabalho1 funcionamento3.asm
; Author : anaisa


.def tmp = r16
.def maxcap = r17
.def mincap = r18
.def cap = r19


.equ sw4cap = 0							;numero do pino do switch que demermina numero maximo de capsulas como 4
.equ sw6cap = 1							;numero do pino do switch que demermina numero maximo de capsulas como 6
.equ sw9cap = 2							;numero do pino do switch que demermina numero maximo de capsulas como 9
.equ swcol = 4							;numero do pino do switch para colocar capsula
.equ swret = 5							;numero do pino do switch para retirar capsula


.cseg 
.org 0									;vetor de reset
	jmp main							;assim que o micro arranca, salta para main
.org 0x46								;deixa espaco na memoria para os vetores de interrupcao

init_io:
ser tmp									;coloca todos os bits de r16 a 1
out ddra,tmp							;configura o porto A como saida
out porta,tmp							;inicia os leds todos desligados
out ddrc,tmp							;configura o porto C como saida
ldi tmp,0b11000000						;coloca o bit 7 e 6 a 1, os restantes a zero
out ddrd,tmp							;configura o porto D, como saidas para o bit 7 e 6, e os restantes como entradas
out portd,tmp							;seleciona o display mais a esquerda (pino 7 e 6) desactiva os pull-ups internos nos restantes

ser tmp									;coloca todos ospinos de r16 a 1
out portc,tmp							;apaga o display

ldi cap,0								//valores default
ldi maxcap,9
ldi mincap,3

ret


main:
	ldi tmp,low(RAMEND)					;carrega a stack na ultima linha de memoria de dados
	out spl,tmp
	ldi tmp,high(RAMEND)
	out sph,tmp
	call init_io

	ciclo:
	call esperar
	sbis pind,swcol
	rjmp funcadicionar					;se sw 5 foi pressionado salta para a funcao correpondente
	sbis pind,swret
	rjmp funcretirar					;se sw 6 foi pressionado salta para a funcao correpondente
	sbis pind,sw4cap
	call func4cap						;se sw 1 foi pressionado salta para a funcao correpondente
	sbis pind,sw6cap
	call func6cap						;se sw 2 foi pressionado salta para a funcao correpondente
	sbis pind,sw9cap
	call func9cap						;se sw 3 foi pressionado salta para a funcao correpondente
	rjmp ciclo

	esperar:
	ldi tmp,10							;delay de 15 ms garante que existe um intervalo entre verificaoes
	cicloesperar:
	dec tmp
	call delay
	cpi tmp,0 
	brne cicloesperar
	ret

	funcadicionar:
	sbis pina,0							;verifica se o led VE esta aceso
	rjmp ciclo							;se estiver regressa ao teste de sw
	cp cap,maxcap
	brsh ciclo							;se o numero de capsulas na maquina for igual ou maior que o maximo de capsulas
	inc cap								;incrementa o registo capsulas
	call display						;chama funcao para imprimir numero de capsula no display
	cp cap,maxcap
	brne ciclo							;se o numero de capsulas ainda nao tiver atingido o maximo permitido volta para teste sw
	cbi porta,0							;acende led VE
	rjmp ciclo

	funcretirar:
	cpi cap,0
	breq ciclo							;se a maquina estiver vazia salta para ciclo
	dec cap								;decrementa o numero de capsulas
	call display						;imprime no display o numero de capsulas
	cp cap,mincap						;compara o numero de capsulas na maquina com o minima para permitir adicao de mais
	brsh ciclo							;se o numero de capsulas ainda for igual ou superior ao minimo salta para ciclo
	sbi porta,0							;caso contrario desliga led VE
	rjmp ciclo

	func4cap:
	call delay
	sbic pind,sw4cap
	ret
	ldi maxcap,4
	ldi mincap,1
	cp cap,maxcap
	brne return						;se o numero de capsulas ainda nao tiver atingido o maximo permitido volta para teste sw
	cbi porta,0							;acende led VE
	ret

	func6cap:
	call delay
	sbic pind,sw6cap
	ret
	ldi maxcap,6
	ldi mincap,2
	cp cap,maxcap
	brne return							;se o numero de capsulas ainda nao tiver atingido o maximo permitido volta para teste sw
	cbi porta,0							;acende led VE
	ret

	func9cap:
	call delay
	sbic pind,sw9cap
	ret
	ldi maxcap,9
	ldi mincap,3
	cp cap,maxcap
	brne return							;se o numero de capsulas ainda nao tiver atingido o maximo permitido volta para teste sw
	cbi porta,0							;acende led VE
	return:
	ret



delay:
		push r16
		push r17
		push r18

		ldi r18,13
ciclo1: ldi r17,3
ciclo2: ldi r16,150
ciclo3:	dec r16
									
		brne ciclo3

		dec r17
		brne ciclo2

		dec r28
		brne ciclo1
											
		pop r18
		pop r17
		pop r16
ret


display:
		cpi cap,0
		breq disp0

		cpi cap,1
		breq disp1

		cpi cap,2
		breq disp2

		cpi cap,3
		breq disp3

		cpi cap,4
		breq disp4

		cpi cap,5
		breq disp5

		cpi cap,6
		breq disp6

		cpi cap,7
		breq disp7

		cpi cap,8
		breq disp8

		cpi cap,9
		breq disp9

disp0:
ldi tmp, 0xc0
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

disp9:
ldi tmp, 0x90
out portc, tmp
ret