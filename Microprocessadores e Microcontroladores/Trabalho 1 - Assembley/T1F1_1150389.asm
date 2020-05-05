;
; AssemblerApplication2.asm
;
; Author : anaisa
;
.cseg 
.org 0   //linha memoria 0 - vetor reset
jmp main //1 coisa ser ir para a main
.org 0x46 //espaço de memoria de escrita do programa

start:
//inicialização 
ldi r16, 0 //pq e tudo entradas basta meter 0
out DDRA, r16	//conf como sendo entrada no porto A
out PORTA, r16 // desativa as R de pull-up internas do atmega (SO EXISTEM EM ENTRADAS)

ldi r16 , 0b11111111 //carregar r16 a 1
out DDRC, r16 //conf porto C como saida
out PORTC, r16 //configura as saidas(leds) como ficando apagados

ret //volta ao ponto anterior a chamada do call

//main
main:
		ldi r16,0xff	//espaço para ser usado no vetor de stack (inicialização)
		out spl,r16
		ldi r16,0x10						
		out sph,r16
		call start   //iniciar processador


	switch_1:
		SBIC PINA,0     //"le" no pino A se o botao do sw1 foi premido
		rJMP switch_2    //se não foi, testa o sw2
		LDI r16,0b00000000  //se foi acende todos os leds
		out PORTC,r16

    switch_2:
		SBIC PINA,1
		rJMP switch_3
		LDI r16,0b10000001
		out PORTC,r16


	switch_3:
		SBIC PINA,2
		rJMP switch_4
		LDI r16,0b11000011
		out PORTC,r16

	switch_4:
		SBIC PINA,3
		rJMP switch_6
		LDI r16,0b11100111
		out PORTC,r16

    switch_6:
		SBIC PINA,5
		rJMP switch_1
		LDI r16,0b11111111
		out PORTC,r16
		
		jmp switch_1
