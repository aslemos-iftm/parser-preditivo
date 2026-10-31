#include "ParserPreditivo.h"
#include <stdio.h>
#include <string.h>

void preencheTabela(Tabela *t, int n)
{
    int i;
    t[0].entrada='a';
    t[0].topo='S';
    strcpy(t[0].regra,"AR");
    t[1].entrada='b';
    t[1].topo='S';
    strcpy(t[1].regra,"AR");
    t[2].entrada='c';
    t[2].topo='S';
    strcpy(t[2].regra,"AR");
    t[3].entrada='(';
    t[3].topo='S';
    strcpy(t[3].regra,"AR");
    t[4].entrada='+';
    t[4].topo='R';
    strcpy(t[4].regra,"+AR");
    t[5].entrada=')';
    t[5].topo='R';
    strcpy(t[5].regra,"");
    t[6].entrada='$';
    t[6].topo='R';
    strcpy(t[6].regra,"");
    t[7].entrada='a';
    t[7].topo='A';
    strcpy(t[7].regra,"XB");
    t[8].entrada='b';
    t[8].topo='A';
    strcpy(t[8].regra,"XB");
    t[9].entrada='c';
    t[9].topo='A';
    strcpy(t[9].regra,"XB");
    t[10].entrada='(';
    t[10].topo='A';
    strcpy(t[10].regra,"XB");
    t[11].entrada='+';
    t[11].topo='B';
    strcpy(t[11].regra,"");
    t[12].entrada='*';
    t[12].topo='B';
    strcpy(t[12].regra,"*XB");
    t[13].entrada=')';
    t[13].topo='B';
    strcpy(t[13].regra,"");
    t[14].entrada='$';
    t[14].topo='B';
    strcpy(t[14].regra,"");
    t[15].entrada='a';
    t[15].topo='X';
    strcpy(t[15].regra,"a");
    t[16].entrada='b';
    t[16].topo='X';
    strcpy(t[16].regra,"b");
    t[17].entrada='c';
    t[17].topo='X';
    strcpy(t[17].regra,"c");
    t[18].entrada='(';
    t[18].topo='X';
    strcpy(t[18].regra,"(S)");
}

void predictiveParse(Tabela *t, Pilha *p,char *expressao,ArvGen *a) {
    size_t i=0;
    int j,k;

    while (vazia(p)==0 && i<strlen(expressao)) {

        if (simboloTerminal(p->topo->info)) {
            if (p->topo->info!= expressao[i]){
                printf("\nEsperava encontrar o simbolo %c e foi encontrado o simbolo %c\n",p->topo->info,expressao[i]);
                return;
            }
            else {
                pop(p);
                i++; //Lê o proximo simbolo
            }
        }
        else {
            k=buscaRegra(t,p->topo->info,expressao[i]);
            //printf("Regra %d\n",k);

            if (k==-1){
                printf("\nNao esperava encontrar o simbolo %c\n",expressao[i]);
                return;
            }
            else {
                //printf("\nVou desempilhar %c",p->topo->info);
                pop(p);
                for(j=strlen(t[k].regra)-1;j>-1;j--){
                    push(p,t[k].regra[j]);
                }
                insereSubarvore(a,t[k].regra); //Insere na árvore o lado direito da producao table[A][c]
            }
        }
    }

    if(vazia(p)==0) //Fim inesperado da entrada
        printf("\nFim inesperado da entrada\n");
    else if((i+1)<strlen(expressao))
            printf("\nFim inesperado da pilha\n");
         else
            printf("\nExpressao reconhecida pela linguagem!\n");
}

int simboloTerminal(char s) {
    if(s=='c'|| s=='a'|| s=='b' || s=='+' || s=='*' || s=='(' || s==')' || s=='$')
        return -1;
    else
        return 0;
}

int buscaRegra(Tabela *t,char tpinfo,char expinfo){
    int i;

    for(i=0;i<TABSIZE;i++)
        if(t[i].topo==tpinfo && t[i].entrada==expinfo)
            return i;

    return -1;
}
