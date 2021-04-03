#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>

unsigned char *p_portc; /*Ponteiro dos registradores port c */
unsigned char *p_ddrc; /*Ponteiro dos registradores port c */
unsigned char *p_pinc; /*Ponteiro dos registradores port c */
unsigned char *p_portd; /*Ponteiro dos registradores port d */
unsigned char *p_ddrd; /*Ponteiro dos registradores port d */
unsigned char *mcucr; /* Ponteiro do regristrador de controle */
unsigned char contador; /* Conta quantas vezes o botao foi pressionado */
unsigned char button; /* Indica o estado do botao */
unsigned char estado; /* Indica o estado da maquina de estados responsavel pela mudanca no valor exibido no display */
unsigned char display[16] = { /* Vetor com os valores a serem exibidos no display a cada atualizacao do contador. */
/* O display acende conforme cada segmento Ã© setado em 1, formando os numeros correspondentes em hexadecimal mais o ponto decimal. O bit 0 corresponde ao ponto e de 1 a 7 correspondem, respectivamente, aos segmentos de a atÃ© g, formando os nÃºmeros no display. Abaixo, tem-se o correspondente em hexadecimal da relaÃ§ao de segmentos acesos que formam um numero. */
/* g f e d c b a dp - x7 x6 x5 x4 x3 x2 x1 x0*/
0x7F, /* 0 - 01111111 */
0xD, /* 1 - 00001101 */
0xB7, /* 2 - 10110111 */
0x9F, /* 3 - 10011111 */
0xCD, /* 4 - 11001101 */
0xDB, /* 5 - 11011011*/
0xFB, /* 6 - 11111011*/
0xF, /* 7 - 00001111*/
0xFF, /* 8 - 11111111 */
0xDF, /* 9 - 11011111*/
0xEF, /* A - 11101111*/
0xF9, /* b - 11111001*/
0x73, /* C - 01110011 */
0xBD, /* d - 10111101 */
0xF3, /* E - 11110011 */
0xE3 /* F - 11100011 */
};

/* Funcao que atribui enderecos aos registradores. Alem disso, seta as portas como entradas ou saidas. Tambem faz o controle do resistor de pull up e define o estado inical do display. */
void setup () { 

  /* Atribui ao ponteiro o endereÃ§o do regristrador de controle. Isso permite o controle de pull up do arduino, permitindo o uso do botao. Quando tem 1 no bit 4,desabilita o pull up, do contrario, habilita */
  mcucr = (unsigned char *)0x55;
  *mcucr &= ~(1 << 4);
  
  /* Atribui aos ponteiros os enderecos do registrador Port C */
  p_portc = (unsigned char *) 0x28;
  p_ddrc = (unsigned char *) 0x27;
  p_pinc = (unsigned char *) 0x26;
  
  /* Define PC0 como um regisrador de entrada */
  *p_ddrc &= 0xFE;
  
  /* Ativa o resistor de pull up em PC0 */
  *p_portc |= 0x01;
  
  /* Atribui aos ponteiros os enderecos do registrador Port D */
  p_portd = (unsigned char *) 0x2B;
  p_ddrd = (unsigned char *) 0x2A;
    
  /* Define as portas PDx como saida */
  *p_ddrd |= 0xFF;
  
  /* Inicia o display aparecendo 0 */
  *p_portd |= 0x7E;
  
}

/* Funcao que altera o que aparece no display conforme o botao e pressionado */
void contador_display () { 
	/* Debounce */
  	if ((*p_pinc & 0x01) != button) /* Detecta uma mudanca no estado do botao */
    {
      _delay_ms(30); 
      if ((*p_pinc & 0x01) != button) /* Se mesmo com o delay a mudança permanece, atualizo o estado do botao */
      {
      	button = *p_pinc & 0x01; 
      }
    } 
  switch (estado) {
/* O problema e tratado como uma maquina de estados. No estado inical, o display exibe 0 e o botao nao esta pressionado. Quando pressiono o botao, mudo o valor no display conforme o contador de numero de vezes que o botao foi pressionado. No estado seguinte, tem-se um novo valor no display e o botao pressionado. Se ele continua pressionado, a situacao nao se altera. Se o botao e solto, passo para o estado inicia, caracterizado pelo botao nao pressionado. No entando, o valor no display se altera a depender do contador. */
  	case 0:
/* Botao inicalmente nao pressionado e valor X no display */
    	if (button == 0x0) { /* Botao pressionado */
          *p_portd &= 0x00; /* Apago o que esta no display para inserir um novo valor*/
          contador = (contador + 1)%16; /* Altero o contador para indicar que o botao foi pressionado. O numero de vezes que o botao foi pressionado e o que ira aparecer no display. No entanto, a contagem vai de 0 a 15 em hexadecimal. Diante disso, quando chego em 15, a contagem se inicia novamente, por isso o cÃ¡culo em divisao em mÃ³dulo. */
    	  *p_portd |= display[contador]; /* Acende os segmentos do display de modo a exibir o valor correspondente ao contador. */
    	  estado = estado + 1; /* Altero o estado da maquina de estados */  
        }
        break;
    case 1:
    	if (button == 0x01) { /* Botao anteriomente pressionado e solto. */
          *p_portd &= 0xFE; /* Embora o display continue exibindo o valor do contador, o ponto decimal Ã© apagado se o botao esta solto. */
          estado = 0; /* Retorno ao estado inicial de botao nao pressionado */
          
        }
    	break;

}
}



int main () {

  setup(); /* Chama a funcao que seta as portas do microcontrolador */
  contador = 0; /* Inicia o contador em zero */
  estado = 0; /* Inicia o estado da maquina de estados em zero */
  button = *p_pinc & 0x01; /* Compara o registrador que contem o estado do botao com 1 no bit zero. A operacao feita e & a fim de reconhecer se no bit zero do registrador tem-se um ou zero. Um indica que o botao esta solto e zero, pressionado. */
  
  while (1) {
      
	  _delay_ms (1); /* O delay para o programa antes da execucao das tarefas dentro do looping para que o programa nao fique com atraso caso fique lento */
	  contador_display(); /* Chama a funcao que altera os valores exibidos no display */ 
  
}

}