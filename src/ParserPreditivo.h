
#include "PilhaAutomato.h"
#include "ArvoreAutomato.h"

struct lookahead{
    char entrada;
    char topo;
    char regra[10];
};
typedef struct lookahead Tabela;

#define MSGSIZE 100 //Tamanho maximo da mensagem de resultado

int predictiveParse(Tabela *t, Pilha *p,char *expressao,ArvGen *a,char *mensagem);

int simboloTerminal(char s);

void preencheTabela(Tabela *t);
int buscaRegra(Tabela *t,char tpinfo,char expinfo);
