#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define max 30

typedef enum {
    VAZIO,
    PREENCHIDO

} Estado;

typedef struct D{     //estrutura para a informacao sobre deputados
    char codigo[max];
    char nome[max];
    char genero;
    char circulo[max];
    Estado estado;

}tDeputado;

typedef struct P{    //estrutura para a informacao sobre cada partido
    char sigla[max];
    char nome[max];
    int quantidade;
    tDeputado deputados[max];
    Estado estado;

}tPartido;

typedef struct Par{   // estrutura para o Parlamento
    char pais[1];
    tPartido partido[20]; //20 partidos

}Parlamento;


Parlamento p; // variavel global do programa


void certificarQuePartidosEDeputadosEstaoVazios()
{
    int i, n;

    for(i=0;i<20;i++)   //Coloca todas as posicoes dos vetores partido e vetores deputados a zero.
    {
        p.partido[i].estado = VAZIO;

        for (n=0;n<max;n++)
        {
            p.partido[i].deputados[n].estado = VAZIO;
        }

    }
}

void meterDadosDeTeste()   //Escrita dos dados de teste
{
    strcpy(p.partido[0].sigla,"P1");
    strcpy(p.partido[0].nome,"Partido1");
    p.partido[0].quantidade = 5;
    p.partido[0].estado = PREENCHIDO;

    strcpy(p.partido[1].sigla,"P2");
    strcpy(p.partido[1].nome,"Partido2");
    p.partido[1].quantidade = 4;
    p.partido[1].estado = PREENCHIDO;

    strcpy(p.partido[2].sigla,"P3");
    strcpy(p.partido[2].nome,"Partido3");
    p.partido[2].quantidade = 3;
    p.partido[2].estado = PREENCHIDO;

    p.partido[0].deputados[0].estado = PREENCHIDO;
    strcpy(p.partido[0].deputados[0].codigo, "P1D1");
    strcpy(p.partido[0].deputados[0].circulo, "Porto");
    p.partido[0].deputados[0].genero = 'F';
    strcpy(p.partido[0].deputados[0].nome, "Maria");

    p.partido[0].deputados[1].estado = PREENCHIDO;
    strcpy(p.partido[0].deputados[1].codigo, "P1D2");
    strcpy(p.partido[0].deputados[1].circulo, "Porto");
    p.partido[0].deputados[1].genero =  'M';
    strcpy(p.partido[0].deputados[1].nome, "Joao");

    p.partido[0].deputados[2].estado = PREENCHIDO;
    strcpy(p.partido[0].deputados[2].codigo, "P1D3");
    strcpy(p.partido[0].deputados[2].circulo, "Coimbra");
    p.partido[0].deputados[2].genero = 'M';
    strcpy(p.partido[0].deputados[2].nome, "Paulo");

    p.partido[0].deputados[3].estado = PREENCHIDO;
    strcpy(p.partido[0].deputados[3].codigo, "P1D4");
    strcpy(p.partido[0].deputados[3].circulo, "Braga");
    p.partido[0].deputados[3].genero = 'M';
    strcpy(p.partido[0].deputados[3].nome, "Rui");

    p.partido[0].deputados[4].estado = PREENCHIDO;
    strcpy(p.partido[0].deputados[4].codigo, "P1D5");
    strcpy(p.partido[0].deputados[4].circulo, "Porto");
    p.partido[0].deputados[4].genero = 'F';
    strcpy(p.partido[0].deputados[4].nome, "Ines");

    p.partido[1].deputados[0].estado = PREENCHIDO;
    strcpy(p.partido[1].deputados[0].codigo, "P2D1");
    strcpy(p.partido[1].deputados[0].circulo, "Lisboa");
    p.partido[1].deputados[0].genero = 'F';
    strcpy(p.partido[1].deputados[0].nome, "Joana");

    p.partido[1].deputados[1].estado = PREENCHIDO;
    strcpy(p.partido[1].deputados[1].codigo, "P2D2");
    strcpy(p.partido[1].deputados[1].circulo, "Faro");
    p.partido[1].deputados[1].genero = 'F';
    strcpy(p.partido[1].deputados[1].nome, "Ana");

    p.partido[1].deputados[2].estado = PREENCHIDO;
    strcpy(p.partido[1].deputados[2].codigo, "P2D3");
    strcpy(p.partido[1].deputados[2].circulo, "Evora");
    p.partido[1].deputados[2].genero = 'M';
    strcpy(p.partido[1].deputados[2].nome, "Manuel");

    p.partido[1].deputados[3].estado = PREENCHIDO;
    strcpy(p.partido[1].deputados[3].codigo, "P2D4");
    strcpy(p.partido[1].deputados[3].circulo, "Lisboa");
    p.partido[1].deputados[3].genero = 'M';
    strcpy(p.partido[1].deputados[3].nome, "Pedro");

    p.partido[2].deputados[0].estado = PREENCHIDO;
    strcpy(p.partido[2].deputados[0].codigo, "P3D1");
    strcpy(p.partido[2].deputados[0].circulo, "Lisboa");
    p.partido[2].deputados[0].genero = 'M';
    strcpy(p.partido[2].deputados[0].nome, "Alberto");

    p.partido[2].deputados[1].estado = PREENCHIDO;
    strcpy(p.partido[2].deputados[1].codigo, "P3D2");
    strcpy(p.partido[2].deputados[1].circulo, "Porto");
    p.partido[2].deputados[1].genero = 'M';
    strcpy(p.partido[2].deputados[1].nome, "Carlos");

    p.partido[2].deputados[2].estado = PREENCHIDO;
    strcpy(p.partido[2].deputados[2].codigo, "P3D3");
    strcpy(p.partido[2].deputados[2].circulo, "Porto");
    p.partido[2].deputados[2].genero = 'F';
    strcpy(p.partido[2].deputados[2].nome, "Catarina");
}

/* Adicao de um partido pelo utilizador */

void adicionarpartido()
{
    int n, i, x, y, z, siglaExiste, siglaExiste1, siglaExiste2, testegenero;
    char sigla[max], partido[max], codigo[max];
    char tgenero;
    n=0;

    for(x=0;x<20;x++) {
        if (p.partido[x].estado == VAZIO) { //encontrar 1 posicao vazia no array partidos
            do {                        ///// CONFIRMAÇAO SIGLA
                printf("Qual a sigla do partido?\n");
                scanf("%s", sigla);

                siglaExiste = 0;

                for (y = 0; y < 20; y++) {  //verificacao se existe no vetor partidos uma sigla igual
                    if (p.partido[y].estado == PREENCHIDO) { //procurar nos espacos preenchidos se existe o partido
                        if (strcmp(p.partido[y].sigla, sigla) == 0) //Comparaçao entre a informaçao do array e do utilizador
                        {
                            printf("\nEssa sigla ja existe\n");
                            printf("Insira outra sigla\n");
                            siglaExiste = 1;
                            break;
                        }
                    }
                }

            } while (siglaExiste);

            strcpy(p.partido[x].sigla, sigla);

                   //confirmação da existencia previa de um nome de partido igual
            do {
                printf("Qual o nome do partido?\n");
                scanf("%s", partido);

                siglaExiste1 = 0;

                for (y = 0; y < 20; y++) {
                    if (p.partido[y].estado == PREENCHIDO) {
                        if (strcmp(p.partido[y].nome, partido) == 0)
                        {
                            printf("Esse nome ja existe\n");
                            printf("Insira outro nome para o partido.\n");
                            siglaExiste1 = 1;
                            break;
                        }
                    }
                }

            } while (siglaExiste1);

            strcpy(p.partido[x].nome, partido);
            // FIM  verificaçao do nome do partido
            printf("Qual a quantidade de deputados?\n");
            scanf("%d", &(p.partido[x].quantidade));

            p.partido[x].estado = PREENCHIDO;
            n = p.partido[x].quantidade;
            // FIM preenchimento de dados para um novo partido, segue se a quantidade de deputados for maior que 0
            for (i = 0; i < n; i++) //Para o caso de existirem deputados a serem inseridos no partido criado

            {
                // //////////////////////////// INICIO VERIFICAR existencia de um deputado com igual codigo ao inserido

                do {
                    printf("Qual o codigo do deputado?\n");
                    scanf("%s", codigo);

                    siglaExiste2 = 0;
                    for(z=0; z<20;z++)
                    {
                        if(p.partido[z].estado==PREENCHIDO){

                            for (y = 0; y < 20; y++)
                            {
                                if (p.partido[z].deputados[y].estado == PREENCHIDO) {
                                    if (strcmp(p.partido[z].deputados[y].codigo, codigo) == 0)
                                    {
                                        printf("Esse codigo ja existe no parlamento\n");
                                        printf("Insira novo codigo para o deputado.\n");
                                        siglaExiste2 = 1;
                                        break;
                                    }
                                }
                            }
                        }
                    }

                } while (siglaExiste2);

                strcpy(p.partido[x].deputados[i].codigo, codigo);

                // FIM VERIFICAR CODIGO DEPUTADO

                printf("Qual o nome do deputado?\n");
                scanf("%s", p.partido[x].deputados[i].nome);
                do{    // verificação de caracteres válidos para o género do deputado
                    printf("Qual o genero? Introduza F ou f para feminino e M ou m para masculino.\n");
                    fflush(stdin);
                    scanf("%c", &tgenero);

                    switch (tgenero)
                    {
                        case 'M':
                        case 'm':
                            p.partido[x].deputados[i].genero = 'M';
                            testegenero = 1;
                            break;
                        case 'F':
                        case 'f':
                            p.partido[x].deputados[i].genero = 'F';
                            testegenero = 1;
                            break;
                        default:
                            testegenero = 0;
                            printf(" Sigla do genero errada! Introduza F ou f para feminino e M ou m para masculino!\n ");
                    }
                }while(testegenero==0); // fim verificaçao do genero
                printf("Qual o circulo eleitoral?\n");
                scanf("%s", p.partido[x].deputados[i].circulo);
                p.partido[x].deputados[i].estado = PREENCHIDO;
            }
            break;
        }
    }
}


/* Adicao de um deputado pelo utilizador */
void adicionardeputado()
{
    int n, j, z, x, y,i, siglaExiste, testegenero, siglaExiste1;
    char tgenero;
    char codigo[max], sigla[max];
    int respostaErrada = 0;
    char resposta;

    /* Adicionar informacao sobre o deputado */

    printf("O deputado ja tem partido? Responda S para Sim ou N para nao.\n");
    fflush(stdin); // Limpeza de valores anteriores que possam existir no buffer

    do {
        scanf("%s", &resposta);
        if (resposta == 's' || resposta == 'S')
        {
            // INICIO VERIFICAÇAO se a sigla existe ou nao
                    do {
                        printf("Qual a sigla do partido?\n");
                        scanf("%s", sigla);
                        siglaExiste = 0;
                        for (y = 0; y < 20; y++)
                        {
                            if (p.partido[y].estado == PREENCHIDO)
                            {
                                if (strcmp(p.partido[y].sigla, sigla) == 0)
                                {
                                    siglaExiste = 1;

                                    n = y; //auxiliar para a posiçao dos partidos
                                    for (j = 0; j < max; j++)
                                    { // percorrer vetor dos deputados para preeenher
                                        if (p.partido[n].deputados[j].estado == VAZIO)
                                        {

                                            //Verificaçao existencia de codigo de deputado igual
                                            do {
                                                printf("Qual o codigo do deputado?\n");
                                                scanf("%s", codigo);
                                                siglaExiste1 = 0;
                                                for(z=0; z<20;z++)
                                                {
                                                    if(p.partido[z].estado==PREENCHIDO)
                                                    {
                                                        for (y = 0; y < 20; y++)
                                                        {
                                                            if (p.partido[z].deputados[y].estado == PREENCHIDO)
                                                            {
                                                                if (strcmp(p.partido[z].deputados[y].codigo, codigo) == 0) // caso exista codigo igual
                                                                {
                                                                    printf("Esse codigo ja existe no parlamento!!!!!!\n");
                                                                    printf("Insira novo codigo para o deputado.\1\n\n");
                                                                    siglaExiste1 = 1;
                                                                    break;
                                                                }
                                                            }
                                                        }
                                                    }

                                                }
                                            } while (siglaExiste1);
                                            strcpy(p.partido[n].deputados[j].codigo, codigo);
                                            //// FIM CODIGO VER SIGLA

                                            printf("Qual o nome do deputado?\n");
                                            scanf("%s", p.partido[n].deputados[j].nome);

                                            ///////// TESTE GENERO
                                            do{
                                                printf("Qual o genero? Introduza F ou f para feminino e M ou m para masculino.\n");
                                                fflush(stdin);
                                                scanf("%c", &tgenero);

                                                switch (tgenero)
                                                {
                                                    case 'M':
                                                    case 'm':
                                                        p.partido[n].deputados[j].genero = 'M';
                                                        testegenero = 1;
                                                        break;
                                                    case 'F':
                                                    case 'f':
                                                        p.partido[n].deputados[j].genero = 'F';
                                                        testegenero = 1;
                                                        break;
                                                    default:
                                                        testegenero = 0;
                                                        printf(" Sigla do genero errada! Introduza F ou f para feminino e M ou m para masculino!\n ");
                                                }

                                            }while(testegenero==0);
                                            printf("Qual o circulo eleitoral?\n");
                                            fflush(stdin);
                                            scanf("%s", p.partido[n].deputados[j].circulo);
                                            p.partido[n].quantidade=p.partido[n].quantidade +1;
                                            p.partido[n].deputados[j].estado = PREENCHIDO;
                                            break;
                                        }

                                    }
                                    break;

                                }
                            }
                        }

                        if(!siglaExiste)
                            {
                                printf("A sigla inserida nao existe!");
                                break;
                            }

                    } while (!siglaExiste);
            break;
        }// FECHA IF DE ENTRAR PARA UM PARTIDO EXISTENTE
        else if (resposta == 'n' || resposta == 'N')
        {
            printf("Vai ter de adicionar um partido antes:\n");
            adicionarpartido(); //Chama a função para adicionar um partido antes de adicionar um deputado sem partido
            respostaErrada = 0;
            break;
        } else
        {
            printf("A opcao escolhida nao e valida\n");
            respostaErrada = 1;
        }
        //break;
    }while(respostaErrada);
}

/* Mostrar deputados todos os partidos */

void mostrardeputados()
{
    int i,n;

    for (i=0; i<20; i++)
    {
        if(p.partido[i].estado == PREENCHIDO) {
            for (n = 0; n < max; n++) {
                if (p.partido[i].deputados[n].estado == PREENCHIDO) {
                    printf("Informacao sobre cada deputado:\n");
                    printf("Codigo: %s \n", p.partido[i].deputados[n].codigo);
                    printf("Nome: %s \n", p.partido[i].deputados[n].nome);
                    printf("Genero: %c \n", p.partido[i].deputados[n].genero);
                    printf("Circulo eleitoral: %s \n\n", p.partido[i].deputados[n].circulo);
                }
            }
        }
    }

}

void mostrarpartido()
{
    int i;

    for (i=0; i<20; i++)
    {
        if(p.partido[i].estado == PREENCHIDO) {
            printf("Informacao sobre cada partido:\n");
            printf("Sigla: %s \n", p.partido[i].sigla);
            printf("Nome: %s \n", p.partido[i].nome);
            printf("Quantidade de deputados: %d \n\n", p.partido[i].quantidade);
        }
    }

}

void mostrarparlamento()
{
    int i;

    printf("Informacao sobre o parlamento:\n");
    printf("Pais: %s \n", p.pais);

    for (i=0; i<20; i++)
    {
        if(p.partido[i].estado == PREENCHIDO) {

            printf("Nome: %s \n", p.partido[i].nome);

        }
    }

}

void eliminarpartido()
{
    int i, j, n, siglaExiste;
    char eliminar[max];

	do {
    printf("Qual o partido a eliminar? Insira a sigla.\n");
    scanf("%s", eliminar);
    siglaExiste = 0;

    for (i = 0; i < 20 ; i++)
    {
    	 if (p.partido[i].estado == PREENCHIDO)
		 {
		 	if (strcmp(p.partido[i].sigla, eliminar) == 0)   //A comparacao retorna 0 se inf array = sigla utilizador
        	{
        		siglaExiste = 1;
         		n=i;
            	for (j = 0; j < max; j++)
				{
            		p.partido[n].deputados[j].estado = VAZIO;
            	}
            p.partido[n].estado = VAZIO;
            break;
           }

		 }
     }

                        if(!siglaExiste)
                            {
                                printf("A sigla do partido inserida nao existe!");
                                break;
                            }

    } while (!siglaExiste);
}

void eliminardeputado()
{
    int i, n, siglaExiste;
    char eliminar[max];


do {
    printf("Qual o deputado a eliminar? Insira o codigo.\n");
    scanf("%s", eliminar);
    siglaExiste = 0;


    for (i = 0; i < 20; i++)
    {

        for (n = 0; n < max; n++)
        {
            if (strcmp(p.partido[i].deputados[n].codigo, eliminar) == 0)
            {
                p.partido[i].deputados[n].estado = VAZIO;
                siglaExiste = 1;
                break;
            }
        }

    }

     if(!siglaExiste)
	 {
         printf("O codigo do deputado inserido nao existe!");
        break;
    }

     } while (!siglaExiste);
}

/* mostrar deputados por uma sigla */

void mostrar ()
{   int i, n;
    char sigla[max];
    printf("Qual a sigla do partido que pretende que seja mostrado os dados \n");
    scanf("%s", sigla);
    for(i=0;i<20;i++) //max sao 20 partidos
    {
        if(p.partido[i].estado == PREENCHIDO)
        {
            if (strcmp(p.partido[i].sigla, sigla) == 0) { // comparacao se a sigla dada pelo utilizador existe
                for (n = 0; n < max; n++) {
                    if (p.partido[i].deputados[n].estado == PREENCHIDO) {
                        printf("Informacao sobre cada deputado:\n");
                        printf("Nome: %s \n", p.partido[i].deputados[n].nome);
                        printf("Codigo: %s \n", p.partido[i].deputados[n].codigo);
                        printf("Genero: %c \n", p.partido[i].deputados[n].genero);
                        printf("Circulo eleitoral: %s \n\n\n", p.partido[i].deputados[n].circulo);
                    }
                }

                return;
            }
        }
    }

    printf("Nao existe o Partido introduzido!\n");
}

void totalpartidos()
{
    int soma, i; // n de posicoes preenchidas com informacao, vai ser o n de partidos existente

    soma=0;

    for(i=0;i<20;i++)
    {
        if (p.partido[i].estado == PREENCHIDO)
        {
            soma=soma+1;
        }
    }

    printf("O numero total de partidos e: %d \n\n ", soma);


}

void totaldeputados(float *soma) // funcao que conta todos os deputados existentes na assembleia
{
    int  i, n;

    *soma=0;
    for(i=0;i<20;i++)
    {
        if(p.partido[i].estado ==PREENCHIDO)
        {
            for(n=0;n<max;n++)
            {
                if(	p.partido[i].deputados[n].estado == PREENCHIDO)
                    *soma=*soma+1;

            }
        }

    }


}

void totaldeputadosfeminino( float * somaf) // conta o numero de deputadas
{
    int  i, n;
    *somaf=0;

    for(i=0;i<20;i++)
    {
        if(p.partido[i].estado==PREENCHIDO)
        {
            for(n=0;n<max;n++)
            {
                if(p.partido[i].deputados[n].genero == 'F' && p.partido[i].deputados[n].estado == PREENCHIDO)
                    *somaf=*somaf+1;
            }

        }
    }

}

void mediafeminino(float a, float b) //calculo da media de deputadas
{
    float media;
    media=(b/a)*100;
    printf("Media de deputados do sexo feminino: %.2f%%\n", media);
    printf("Numero total de deputados do sexo feminino: %.f\n\n ",b);
}


void mediapartido(float a) //calculo da percentagem que cada partido possui
{
    int i, n, partidoExiste;
    char siglap[max];
    float mediap,somap;
    somap=0;

    do {
       	 	printf("Qual a sigla do partido?\n");
   		 	scanf("%s", siglap);
   		 	partidoExiste = 0;
   		 	for (i = 0; i < 20; i++)
   		 	{
   		 		 if (p.partido[i].estado == PREENCHIDO)
   		 		 {
   		 		 	if(strcmp(p.partido[i].sigla,siglap)==0) ////existe o partido
   		 		 	{
   		 		 		partidoExiste = 1;
   		 		 	    for(n=0;n<max;n++)
            			{
                			if(	p.partido[i].deputados[n].estado==PREENCHIDO)
                			{
                					somap=somap+1;
							}

						}

					}
				}

			}
			 if(!partidoExiste)
            {
                   printf("A sigla do partido inserida nao existe!");
                    break;
            }
    mediap=(somap/a)*100;
   	printf("A percentagem do partido %s e: %.2f%% \n", siglap ,mediap);
   	printf("Numero total de deputados existentes no parlamento: %.f\n\n",a);

     } while (!partidoExiste);
}


void votacao(){  //funcao que determina qual o partido mais votado e menos votado
    int i, indicemaior, indicemenor;

    for(i=0;i<max;i++)
    {
        if(p.partido[i].estado == PREENCHIDO)
        {
            indicemenor = i;
            indicemaior = i;
            break;
        }
    }

    for(i=i+1; i<max; i++)
    {
        if(p.partido[i].estado == PREENCHIDO)
        {
            if(p.partido[i].quantidade > p.partido[indicemaior].quantidade) //para o partido mais votado
                indicemaior=i;

            if(p.partido[i].quantidade < p.partido[indicemenor].quantidade) //para o partido menos votado
                indicemenor = i;
        }
    }

    printf("Partido mais votado :%s\n",p.partido[indicemaior].nome);
    printf("Partido menos votado :%s\n\n",p.partido[indicemenor].nome);
}

int main()
{
    float totalfeminino, nrtotaldeputados;
    meterDadosDeTeste(); //para preencher os arrays no inicio do programa, carregamento de dados teste

    printf("Qual o Pais onde se encontra o parlamento?\n");
    scanf("%s", p.pais);

    int opcao;

    do{
        printf(" \n");
        printf("1-Inserir deputado \n");
        printf("2-Inserir partido  \n");
        printf("3-Mostrar parlamento\n");
        printf("4-Mostrar  todos os partidos existentes \n");
        printf("5-Mostar todos os deputados do parlamento\n");
        printf("6-Mostrar nomes de todas os deputados de um partido\n");
        printf("7-Mostrar  numero total de partidos\n");
        printf("8-Mostrar numero e media de deputados do sexo feminino do parlamento\n");
        printf("9-Mostar a percentagem de deputados do partido\n");
        printf("10-Mostar partido mais votado e menos votado\n");
        printf("11-eliminar deputado\n");
        printf("12-eliminar partido\n");
        printf("0-Fechar programa\n");

        fflush(stdin);
        scanf("%d", &opcao);


        switch (opcao)
        {
            case 1: printf("\n");adicionardeputado();system("pause"); break;
            case 2: printf("\n");adicionarpartido(); system("pause"); break;
            case 3: printf("\n");mostrarparlamento() ;system("pause"); break;
            case 4: printf("\n");mostrarpartido();system("pause"); break;
            case 5: printf("\n");mostrardeputados();system("pause"); break;
            case 6: printf("\n");mostrar();system("pause"); break;
            case 7: printf("\n");totalpartidos();system("pause"); break;
            case 8: printf("\n");totaldeputadosfeminino(&totalfeminino);totaldeputados(&nrtotaldeputados); mediafeminino(nrtotaldeputados,totalfeminino);system("pause"); break;
            case 9: printf("\n");totaldeputados(&nrtotaldeputados); mediapartido(nrtotaldeputados);system("pause"); break;
            case 10: printf("\n");votacao();system("pause"); break;
            case 11: printf("\n");eliminardeputado();system("pause"); break;
            case 12: printf("\n");eliminarpartido();system("pause"); break;
            case 0: printf("\nTerminando...."); exit(0);break;
            default: printf("\nERRO - Opcao invalida! \n");system("pause");
        }
        system("cls");

    } while (opcao != '0');

    return 0;
}
