#include <stdio.h>
#include <string.h>
//#include "PilhaAutomato.h"
//#include "ArvoreAutomato.h"
#include "ParserPreditivo.h"

int main ()
{
    char expressao[INPUTSIZE];
    char mensagem[MSGSIZE]; //Recebe o resultado do parser
    Pilha* pilhaExp;
    Tabela tab[TABSIZE];
    ArvGen* a = criaArv('S');
    
    preencheTabela(tab);

    printf("Entre com a expressao a ser reconhecida pela linguagem (maximo %d caracteres):", INPUTSIZE);
    scanf("%s",expressao);
    pilhaExp=cria();
    push(pilhaExp,'S');

    predictiveParse(tab, pilhaExp,expressao,a,mensagem);
    printf("\n%s\n",mensagem);

    imprimeArv(a,0);
    printf("\nExpressao com colchetes, obtida pela arvore:\n");
    imprimeExp(a,0,0);
    printf("\n");
    libera(pilhaExp);
}

