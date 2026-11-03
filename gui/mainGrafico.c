#include <stdio.h>
#include "raylib.h"
#include "ParserPreditivo.h"

#define LARGURA 1000      //Largura da janela, em pixels
#define ALTURA 700        //Altura da janela, em pixels
#define MARGEM 30         //Distancia entre o desenho e a borda da janela
#define TOPO_ARVORE 170   //Altura onde comeca a area da arvore
#define ESPACO_MAXIMO 80  //Maior distancia entre dois nodos vizinhos
#define RAIO_MAXIMO 18    //Maior raio dos circulos dos nodos

//Medidas usadas para desenhar a arvore na janela
struct desenho {
    Font fonte;
    float inicioX;   //Posicao x da coluna 0
    float inicioY;   //Posicao y do nivel 0
    float espacoH;   //Distancia horizontal entre duas colunas
    float espacoV;   //Distancia vertical entre dois niveis
    float raio;      //Raio dos circulos
};
typedef struct desenho Desenho;

//Conta quantos niveis tem a arvore (so a raiz = 1 nivel)
int contaNiveis(ArvGen *a)
{
    ArvGen *p;
    int maior=0,n;
    for(p=a->prim;p!=NULL;p=p->prox){
        n=contaNiveis(p);
        if(n>maior)
            maior=n;
    }
    return maior+1;
}

//Converte a posicao do nodo (coluna e nivel) em pixels na janela
Vector2 centroNodo(ArvGen *a,Desenho d)
{
    Vector2 centro;
    centro.x=d.inicioX+a->coluna*d.espacoH;
    centro.y=d.inicioY+a->nivel*d.espacoV;
    return centro;
}

//Escreve um texto centralizado no ponto indicado
void escreveCentralizado(Font fonte,const char *texto,Vector2 centro,float tamanho)
{
    Vector2 medida=MeasureTextEx(fonte,texto,tamanho,0);
    Vector2 posicao={centro.x-medida.x/2,centro.y-medida.y/2};
    DrawTextEx(fonte,texto,posicao,tamanho,0,BLACK);
}

//Escolhe a cor do nodo:
//vazio (producao vazia), terminal, nao terminal expandido ou nao expandido
Color corNodo(ArvGen *a)
{
    if(a->info=='\0')
        return LIGHTGRAY;  //Nodo vazio
    if(simboloTerminal(a->info))
        return SKYBLUE;    //Terminal
    if(a->prim==NULL)
        return ORANGE;     //Nao terminal que ficou sem expandir
    return LIME;           //Nao terminal expandido
}

//Desenha um nodo: circulo colorido, contorno e simbolo
void desenhaNodo(ArvGen *a,Vector2 centro,Desenho d)
{
    char simbolo[2]={a->info,'\0'};

    DrawCircleV(centro,d.raio,corNodo(a));
    DrawCircleLines((int)centro.x,(int)centro.y,d.raio,BLACK);
    if(a->info=='\0')
        escreveCentralizado(d.fonte,"ε",centro,d.raio*1.3f);
    else
        escreveCentralizado(d.fonte,simbolo,centro,d.raio*1.3f);
}

//Desenha a arvore: primeiro as linhas ate cada filho e os filhos,
//depois o proprio nodo, que fica por cima das linhas
void desenhaArvore(ArvGen *a,Desenho d)
{
    ArvGen *p;
    Vector2 centro=centroNodo(a,d);

    for(p=a->prim;p!=NULL;p=p->prox){
        DrawLineEx(centro,centroNodo(p,d),2,DARKGRAY);
        desenhaArvore(p,d);
    }
    desenhaNodo(a,centro,d);
}

//Calcula as medidas do desenho para que a arvore caiba na janela
Desenho calculaDesenho(Font fonte,int folhas,int niveis)
{
    Desenho d;
    float larguraArea=LARGURA-2*MARGEM;
    float alturaArea=ALTURA-TOPO_ARVORE-MARGEM;

    d.fonte=fonte;
    d.espacoH=larguraArea/folhas;
    if(d.espacoH>ESPACO_MAXIMO)
        d.espacoH=ESPACO_MAXIMO;
    d.espacoV=alturaArea/niveis;
    if(d.espacoV>ESPACO_MAXIMO)
        d.espacoV=ESPACO_MAXIMO;

    d.raio=RAIO_MAXIMO;            //O circulo nao pode ser maior
    if(d.raio>d.espacoH*0.4f)      //que o espaco entre os nodos
        d.raio=d.espacoH*0.4f;
    if(d.raio>d.espacoV*0.4f)
        d.raio=d.espacoV*0.4f;

    d.inicioX=LARGURA/2.0f-(folhas-1)*d.espacoH/2; //Centraliza a arvore
    d.inicioY=TOPO_ARVORE+d.raio;
    return d;
}

//Desenha a legenda das cores
void desenhaLegenda(Font fonte)
{
    const char *nomes[4]={"terminal","não terminal","não terminal não expandido","produção vazia (ε)"};
    Color cores[4]={SKYBLUE,LIME,ORANGE,LIGHTGRAY};
    float x=MARGEM;
    int i;

    for(i=0;i<4;i++){
        DrawCircle((int)x+8,TOPO_ARVORE-25,8,cores[i]);
        DrawCircleLines((int)x+8,TOPO_ARVORE-25,8,BLACK);
        DrawTextEx(fonte,nomes[i],(Vector2){x+22,TOPO_ARVORE-35},20,0,DARKGRAY);
        x=x+22+MeasureTextEx(fonte,nomes[i],20,0).x+30;
    }
}

int main(void)
{
    char expressao[INPUTSIZE]="";
    int tamanho=0;                 //Quantidade de caracteres digitados
    char mensagem[MSGSIZE]="Digite a expressão e pressione Enter ou clique em Analisar.";
    int reconhecida=-1;            //-1: nada analisado ainda; 1: reconhecida; 0: erro
    Tabela tab[TABSIZE];
    Pilha *pilhaExp;
    ArvGen *arvore=NULL;
    Desenho d;
    Font fonte;
    int codigos[192];              //Caracteres que a fonte deve conter
    int i,n=0,tecla,analisar;
    Rectangle caixa={MARGEM,50,400,40};
    Rectangle botao={MARGEM+420,50,130,40};
    Color corMensagem;

    preencheTabela(tab);

    InitWindow(LARGURA,ALTURA,"Parser preditivo");
    SetTargetFPS(60);

    //Carrega a fonte Arial do Windows com os caracteres comuns,
    //os acentos e o epsilon, que a fonte padrao da raylib nao tem
    for(i=32;i<=126;i++)
        codigos[n++]=i;
    for(i=160;i<=255;i++)
        codigos[n++]=i;
    codigos[n++]=0x03B5; //epsilon
    fonte=LoadFontEx("C:/Windows/Fonts/arial.ttf",64,codigos,n);
    SetTextureFilter(fonte.texture,TEXTURE_FILTER_BILINEAR);

    while(!WindowShouldClose()){

        //1) Le o teclado e o mouse
        tecla=GetCharPressed();
        while(tecla>0){ //Pode haver mais de uma tecla por quadro
            if(tecla>=32 && tecla<=126 && tamanho<INPUTSIZE-1){
                expressao[tamanho]=(char)tecla;
                tamanho++;
                expressao[tamanho]='\0';
            }
            tecla=GetCharPressed();
        }
        if((IsKeyPressed(KEY_BACKSPACE) || IsKeyPressedRepeat(KEY_BACKSPACE)) && tamanho>0){
            tamanho--;
            expressao[tamanho]='\0';
        }
        analisar=IsKeyPressed(KEY_ENTER) ||
                 (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(),botao));

        //2) Analisa a expressao, se foi pedido
        if(analisar){
            if(arvore!=NULL)
                liberaArv(arvore); //Descarta a arvore da analise anterior
            arvore=criaArv('S');
            pilhaExp=cria();
            push(pilhaExp,'S');
            reconhecida=predictiveParse(tab,pilhaExp,expressao,arvore,mensagem);
            libera(pilhaExp);
            d=calculaDesenho(fonte,calculaPosicoes(arvore,0,0),contaNiveis(arvore));
        }

        //3) Desenha a janela inteira
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawTextEx(fonte,TextFormat("Expressão (termine com $, até %d caracteres):",INPUTSIZE-1),
                   (Vector2){MARGEM,20},22,0,DARKGRAY);

        DrawRectangleRec(caixa,WHITE);
        DrawRectangleLinesEx(caixa,2,DARKGRAY);
        if(((int)(GetTime()*2))%2==0) //Cursor piscando
            DrawTextEx(fonte,TextFormat("%s_",expressao),(Vector2){caixa.x+10,caixa.y+8},26,0,BLACK);
        else
            DrawTextEx(fonte,expressao,(Vector2){caixa.x+10,caixa.y+8},26,0,BLACK);

        if(CheckCollisionPointRec(GetMousePosition(),botao))
            DrawRectangleRec(botao,LIGHTGRAY); //Mouse sobre o botao
        else
            DrawRectangleRec(botao,GRAY);
        DrawRectangleLinesEx(botao,2,DARKGRAY);
        escreveCentralizado(fonte,"Analisar",(Vector2){botao.x+botao.width/2,botao.y+botao.height/2},24);

        if(reconhecida==1)
            corMensagem=DARKGREEN;
        else if(reconhecida==0)
            corMensagem=MAROON;
        else
            corMensagem=DARKGRAY;
        DrawTextEx(fonte,mensagem,(Vector2){MARGEM,100},24,0,corMensagem);

        if(arvore!=NULL){
            desenhaLegenda(fonte);
            desenhaArvore(arvore,d);
        }

        EndDrawing();
    }

    if(arvore!=NULL)
        liberaArv(arvore);
    UnloadFont(fonte);
    CloseWindow();
    return 0;
}
