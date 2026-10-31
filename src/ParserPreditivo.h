
#include "PilhaAutomato.h"
#include "ArvoreAutomato.h"

struct lookahead{
    char entrada;
    char topo;
    char regra[10];
};
typedef struct lookahead Tabela;

void predictiveParse(Tabela *t, Pilha *p,char *expressao,ArvGen *a);

int simboloTerminal(char s);

void preencheTabela(Tabela *t, int n);
int buscaRegra(Tabela *t,char tpinfo,char expinfo);
