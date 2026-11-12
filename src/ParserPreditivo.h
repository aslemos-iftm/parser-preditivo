
#include "PilhaAutomato.h"
#include "ArvoreAutomato.h"

struct lookahead{
    char entrada;
    char topo;
    char regra[10];
};
typedef struct lookahead Tabela;

#define MSGSIZE 100 //Tamanho maximo da mensagem com o resultado da analise

#define MAXNT 10  //Maximo de nao terminais na gramatica
#define MAXALT 5  //Maximo de lados direitos (alternativas) por nao terminal

//Uma linha da gramatica: um nao terminal e todos os seus lados direitos.
//Por exemplo, R -> +AR | e fica com naoTerminal 'R' e as alternativas "+AR" e "".
//O lado direito "" representa a producao vazia (epsilon).
struct producao{
    char naoTerminal;
    char alternativas[MAXALT][10];
    int qtdAlternativas;
};
typedef struct producao Producao;

//A mensagem com o resultado da analise e escrita no vetor "mensagem",
//para que quem chama decida onde mostra-la (terminal ou janela)
void predictiveParse(Tabela *t, Pilha *p,char *expressao,ArvGen *a,char *mensagem);

int simboloTerminal(char s);

void preencheTabela(Tabela *t);
int buscaRegra(Tabela *t,char tpinfo,char expinfo);

//Reconstroi a gramatica a partir da tabela de lookahead, sem repeticoes,
//e a guarda no vetor "g". Devolve quantos nao terminais foram encontrados.
int extraiGramatica(Tabela *t, Producao *g);
