; trabalho 1 ex2.asm
; Created: 03/11/2018 17:44:58
; Author : anaisa

.cseg 
.org 0   // reset
jmp main 
.org 0x46 //escrita do programa

start: //inicialização portos

ldi r16, 0 
out DDRA, r16	//conf entrada porto A
out PORTA, r16 // desativa as R de pull-up internas 

ldi r16 , 0b11111111 //carregar r16 a 1
out DDRC, r16 //conf porto C como saida
out PORTC, r16 //configura as saidas(leds) como ficando apagados

ret //return

main:
		ldi r16,0xff	//vetor de stack (inicialização)
		out spl,r16
		ldi r16,0x10						
		out sph,r16
		call start   

ciclo:
		call testesw
		jmp ciclo  //garante q testa sempre ate q algo seja pressionado


testesw:
	switch_1:
		SBIC PINA,0        //"le" no pino A se o botao do sw1 foi premido //salta se bit=0 ou seja pressionado
		rJMP switch_2      //se não foi, testa o 2
		call AcendeEApaga1  //se foi premido acende leds v=500ms


	switch_2:
		SBIC PINA,1        //"le" no pino A se o botao do sw2 foi premido //salta se bit=0 ou seja pressionado
		ret     //se não foi, testa o sw6
		call AcendeEApaga2  //se foi premido acende leds v=1s


AcendeEApaga1:
		LDI r16,0b11111111 //garante todos apagados
		out PORTC,r16
		call Delay500ms
		call testesw
		SBIS PINA, 5 //salta se bit=1 ou seja nao pressionado
		jmp ciclo //ciclo para ficar preso
		LDI r16,0b01111111 //acende led1
		out PORTC,r16
		call Delay500ms
		call testesw
		SBIS PINA, 5 //salta se bit=1 ou seja nao pressionado
		jmp ciclo //ciclo para ficar preso
		LDI r16,0b00111111 //acende led2 
		out PORTC,r16
		call Delay500ms
		call testesw
		SBIS PINA, 5 //salta se bit=1 ou seja nao pressionado
		jmp ciclo //ciclo para ficar preso
		LDI r16,0b00011111 //acende led3
		out PORTC,r16
		call Delay500ms
		call testesw
		SBIS PINA, 5 //salta se bit=1 ou seja nao pressionado
		jmp ciclo //ciclo para ficar preso
		LDI r16,0b00001111 //acende led4
		out PORTC,r16
		call Delay500ms
		call testesw
		SBIS PINA, 5 //salta se bit=1 ou seja nao pressionado
		jmp ciclo //ciclo para ficar preso
		LDI r16,0b00000111 //acende led5
		out PORTC,r16
		call Delay500ms
		call testesw
		SBIS PINA, 5 //salta se bit=1 ou seja nao pressionado  (teste sw6)
		jmp ciclo //ciclo para ficar preso
		LDI r16,0b00000011 //acende led6
		out PORTC,r16
		call Delay500ms
		call testesw
		SBIS PINA, 5 //salta se bit=1 ou seja nao pressionado
		jmp ciclo //ciclo para ficar preso
		LDI r16,0b00000001 //acende led7
		out PORTC,r16
		call Delay500ms
		call testesw
		SBIS PINA, 5 //salta se bit=1 ou seja nao pressionado
		jmp ciclo //ciclo para ficar preso
		LDI r16,0b00000000 //acende led8
		out PORTC,r16
		call Delay500ms
		call testesw
		SBIS PINA, 5 //salta se bit=1 ou seja nao pressionado
		jmp ciclo //ciclo para ficar preso
		jmp AcendeEApaga1 //recomeçar a sequencia com todos desativos
	
AcendeEApaga2:
		LDI r16,0b11111111 //garante todos apagados
		out PORTC,r16
		call Delay500ms
		call testesw
		SBIS PINA, 5 //salta se bit=1 ou seja nao pressionado
		jmp ciclo //ciclo para ficar preso
		call Delay500ms
		LDI r16,0b01111111 //acende led1
		out PORTC,r16
		call Delay500ms
		call testesw
		SBIS PINA, 5 //salta se bit=1 ou seja nao pressionado
		jmp ciclo //ciclo para ficar preso
		call Delay500ms
		call testesw
		SBIS PINA, 5 //salta se bit=1 ou seja nao pressionado
		jmp ciclo //ciclo para ficar preso
		LDI r16,0b00111111 //acende led2 
		out PORTC,r16
		call Delay500ms
		call testesw
		SBIS PINA, 5 //salta se bit=1 ou seja nao pressionado
		jmp ciclo //ciclo para ficar preso
		call Delay500ms
		call testesw
		SBIS PINA, 5 //salta se bit=1 ou seja nao pressionado
		jmp ciclo //ciclo para ficar preso
		LDI r16,0b00011111 //acende led3
		out PORTC,r16
		call Delay500ms
		call testesw
		SBIS PINA, 5 //salta se bit=1 ou seja nao pressionado
		jmp ciclo //ciclo para ficar preso
		call Delay500ms
		call testesw
		SBIS PINA, 5 //salta se bit=1 ou seja nao pressionado
		jmp ciclo //ciclo para ficar preso
		LDI r16,0b00001111 //acende led4
		out PORTC,r16
		call Delay500ms
		call testesw
		SBIS PINA, 5 //salta se bit=1 ou seja nao pressionado
		jmp ciclo //ciclo para ficar preso
		call Delay500ms
		call testesw
		SBIS PINA, 5 //salta se bit=1 ou seja nao pressionado
		jmp ciclo //ciclo para ficar preso
		LDI r16,0b00000111 //acende led5
		out PORTC,r16
		call Delay500ms
		call testesw
		SBIS PINA, 5 //salta se bit=1 ou seja nao pressionado
		jmp ciclo //ciclo para ficar preso
		call Delay500ms
		call testesw
		SBIS PINA, 5 //salta se bit=1 ou seja nao pressionado
		jmp ciclo //ciclo para ficar preso
		LDI r16,0b00000011 //acende led6
		out PORTC,r16
		call Delay500ms
		call testesw
		SBIS PINA, 5 //salta se bit=1 ou seja nao pressionado
		jmp ciclo //ciclo para ficar preso
		call Delay500ms
		call testesw
		SBIS PINA, 5 //salta se bit=1 ou seja nao pressionado
		jmp ciclo //ciclo para ficar preso
		LDI r16,0b00000001 //acende led7
		out PORTC,r16
		call Delay500ms
		call testesw
		SBIS PINA, 5 //salta se bit=1 ou seja nao pressionado
		jmp ciclo //ciclo para ficar preso
		call Delay500ms
		call testesw
		SBIS PINA, 5 //salta se bit=1 ou seja nao pressionado
		jmp ciclo //ciclo para ficar preso
		LDI r16,0b00000000 //acende led8
		out PORTC,r16
		call Delay500ms
		call testesw
		SBIS PINA, 5 //salta se bit=1 ou seja nao pressionado
		jmp ciclo //ciclo para ficar preso
		call Delay500ms	
		call testesw
		SBIS PINA, 5 //salta se bit=1 ou seja nao pressionado
		jmp ciclo //ciclo para ficar preso
		jmp AcendeEApaga2 //recomeçar a sequencia com todos desativos

Delay500ms: 
push r18					
push r19					
push r20					
ldi r20,80					// atribui o valor decimal 80 ao registo R20                
CicloDelay0:
	ldi r19,170				// atribui o valor decimal 170 ao registo R19
CicloDelay1:
	ldi r18,196				// atribui o valor decimal 196 ao registo R18
CicloDelay2:
	dec r18
	brne CicloDelay2
	dec r19
	brne CicloDelay1
	dec r20
	brne CicloDelay0
	pop r20					    // recupera da memoria valor de registo R20
	pop r19						// recupera da memoria valor de registo R19
	pop r18						// recupera da memoria valor de registo R18
	ret
//função delay com valores Z=80 Y=170 X=196 FreqOsc=16 000 000 
//Para que delay=(20+3z(1+y+xy))/FreqOsc seja igual a aprox 500ms