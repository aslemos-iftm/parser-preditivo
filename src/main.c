#include <stdio.h>
#include <string.h>
//#include "PilhaAutomato.h"
//#include "ArvoreAutomato.h"
#include "ParserPreditivo.h"

int main ()
{
    char c,expressao[INPUTSIZE];
    Pilha* pilhaExp;
    Tabela tab[TABSIZE];
    int i;
    ArvGen* a = criaArv('S');
    char v;
    preencheTabela(tab,TABSIZE);


    printf("Entre com a expressao a ser reconhecida pela linguagem (maximo %d caracteres):", INPUTSIZE);
    scanf("%s",expressao);
    pilhaExp=cria();
    push(pilhaExp,'S');

    predictiveParse(tab, pilhaExp,expressao,a);

    imprimeArv(a,0);
    printf("\nExpressao com colchetes, obtida pela arvore:\n");
    imprimeExp(a,0,0);
    printf("\n");
    libera(pilhaExp);
}

