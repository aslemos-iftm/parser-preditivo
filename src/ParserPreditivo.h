
#include "PilhaAutomato.h"
#include "ArvoreAutomato.h"

struct lookahead{
    char entrada;
    char topo;
    char regra[10];
};
typedef struct lookahead Tabela;

#define MSGSIZE 100 //Tamanho maximo da mensagem com o resultado da analise

//A mensagem com o resultado da analise e escrita no vetor "mensagem",
//para que quem chama decida onde mostra-la (terminal ou janela)
void predictiveParse(Tabela *t, Pilha *p,char *expressao,ArvGen *a,char *mensagem);

int simboloTerminal(char s);

void preencheTabela(Tabela *t);
int buscaRegra(Tabela *t,char tpinfo,char expinfo);
