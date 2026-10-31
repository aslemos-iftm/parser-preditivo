
struct arvgen {
    char info;
    struct arvgen *prim;
    struct arvgen *prox;
    struct arvgen *ant;
};

typedef struct arvgen ArvGen;

ArvGen* criaArv (char c);
void insereSubarvore(ArvGen* a,char *c);
int insereProd(ArvGen *r,ArvGen *novo,ArvGen *pai);
void imprimeArv (ArvGen* a,int l);
int buscaArv (ArvGen* a, char c);
void liberaArv (ArvGen* a);
void imprimeExp(ArvGen* a,int l,int m);
