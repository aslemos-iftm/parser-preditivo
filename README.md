# Parser preditivo

Parser preditivo LL(1) escrito em C, para estudo de gramáticas livres de contexto e análise sintática. O programa lê uma expressão, decide qual produção aplicar consultando uma **tabela de lookahead** e, ao mesmo tempo, constrói a **árvore de derivação** da expressão, usando uma pilha e uma árvore genérica.

Há duas versões do programa:

- **console** (`parser`): lê a expressão pelo teclado e imprime o resultado e a árvore em texto;
- **interface gráfica** (`parser-gui`): tem uma caixa para digitar a expressão e desenha a árvore na janela, com a raiz no alto e linhas ligando cada nó aos seus filhos.

## A gramática

```
S → A R
R → + A R | ε
A → X B
B → * X B | ε
X → a | b | c | ( S )
```

`S` gera somas de termos (`A`), e cada termo é um produto de fatores (`X`). Um fator é uma das letras `a`, `b` e `c` ou uma expressão entre parênteses. Os não terminais `R` e `B` existem para eliminar a recursão à esquerda, o que é necessário para que a gramática seja LL(1). `ε` é a cadeia vazia.

A entrada deve terminar com `$`, que marca o fim da cadeia, como na teoria dos parsers preditivos. Exemplo: `a+b*c$`.

## A tabela de lookahead

A linha é o não terminal no topo da pilha, e a coluna é o próximo símbolo da entrada. Cada célula indica a produção a aplicar. Uma célula vazia indica erro.

|       | a  | b  | c  | +   | *   | (   | ) | $ |
|-------|----|----|----|-----|-----|-----|---|---|
| **S** | AR | AR | AR |     |     | AR  |   |   |
| **R** |    |    |    | +AR |     |     | ε | ε |
| **A** | XB | XB | XB |     |     | XB  |   |   |
| **B** |    |    |    | ε   | *XB |     | ε | ε |
| **X** | a  | b  | c  |     |     | (S) |   |   |

No código, essa tabela é preenchida entrada por entrada na função `preencheTabela`, em `src/ParserPreditivo.c`.

## Organização dos arquivos

| Arquivo | Conteúdo |
|---|---|
| `src/PilhaAutomato.c` / `.h` | A pilha do autômato (lista encadeada de `char`). |
| `src/ArvoreAutomato.c` / `.h` | A árvore genérica: cada nó aponta para o primeiro filho (`prim`), o próximo irmão (`prox`) e o pai (`ant`). Inclui também o cálculo das posições dos nós para o desenho (`calculaPosicoes`). |
| `src/ParserPreditivo.c` / `.h` | A tabela de lookahead e o laço do parser (`predictiveParse`). |
| `src/main.c` | O programa principal da versão de console. |
| `gui/mainGrafico.c` | O programa principal da interface gráfica, feita com a biblioteca [raylib](https://www.raylib.com). |

As duas versões usam os mesmos arquivos de `src/`: só o programa principal muda.

## Como compilar

Os comandos abaixo usam o compilador `gcc` e devem ser executados na pasta principal do projeto.

### Versão de console

```
gcc -Wall -Wextra -std=c11 src/*.c -o parser
```

### Interface gráfica

A interface precisa da biblioteca **raylib**. No Windows, com o [MSYS2](https://www.msys2.org), ela é instalada pelo terminal do MSYS2 (UCRT64) com:

```
pacman -S mingw-w64-ucrt-x86_64-raylib
```

Depois, para compilar:

```
gcc -Wall -Wextra -std=c11 -Isrc gui/mainGrafico.c src/ArvoreAutomato.c src/ParserPreditivo.c src/PilhaAutomato.c -o parser-gui -lraylib -lopengl32 -lgdi32 -lwinmm
```

A opção `-Isrc` diz ao compilador onde procurar os arquivos `.h`. As opções `-l...` ligam o programa à raylib e às bibliotecas do Windows de que ela depende.

> **Observação sobre a fonte.** A fonte padrão da raylib não tem o símbolo `ε` nem letras acentuadas. Por isso, a interface carrega a fonte Arial do próprio Windows, em `C:/Windows/Fonts/arial.ttf`. Assim, **a interface gráfica, como está, só funciona no Windows**. Em outro sistema (Linux, macOS), é preciso trocar, em `gui/mainGrafico.c`, o caminho da fonte na chamada a `LoadFontEx` por o de uma fonte instalada que tenha esses caracteres, como a DejaVu Sans. Se a fonte não for encontrada, a raylib escreve com a fonte padrão, e o `ε` e os acentos não aparecem corretamente. O comando de compilação também muda fora do Windows, porque as bibliotecas `-lopengl32 -lgdi32 -lwinmm` são do Windows.

## Como usar

**Console:** execute `parser`, digite a expressão terminada em `$` e pressione Enter. O programa mostra o resultado, a árvore (nível, símbolo e pai de cada nó) e a expressão com colchetes obtida a partir da árvore.

**Interface gráfica:** execute `parser-gui`, digite a expressão na caixa e pressione Enter ou clique em **Analisar**. A mensagem aparece em verde se a expressão for reconhecida e em vermelho se houver erro. As cores dos nós indicam:

- **azul:** terminal;
- **verde:** não terminal;
- **laranja:** não terminal que ficou sem expandir, porque a análise parou antes, num erro;
- **cinza:** produção vazia (`ε`).

Quando a expressão é rejeitada, a árvore é mostrada até o ponto em que a análise parou. Os nós em laranja indicam onde o parser não conseguiu continuar.

## Entradas para teste

| Entrada | Resultado esperado |
|---|---|
| `a+b*c$` | reconhecida |
| `(a+b)*c$` | reconhecida |
| `a+$` | erro: não esperava `$` |
| `ab$` | erro: não esperava `b` |
| `a+b*c` | erro: fim inesperado da entrada |

## Escolhas feitas pensando em quem está aprendendo

Algumas partes do código podem parecer estranhas, mas foram escritas assim de propósito, para facilitar a leitura:

- **Os nós vazios.** Quando uma produção vazia (`ε`) é aplicada, a árvore ganha um nó com o caractere `'\0'`. Isso deixa visível que a produção foi usada. Esse nó também marca que `R` ou `B` já foram expandidos, e a construção da árvore depende disso.
- **A leitura simples da entrada.** No console, a expressão é lida com `scanf("%s")` num vetor de 20 caracteres. Uma entrada mais longa que isso causa problemas. Na interface gráfica, a caixa de texto não aceita mais de 19 caracteres.
- **O `$` no fim da entrada**, como na teoria.
- **A tabela escrita à mão**, entrada por entrada, para que ela possa ser lida diretamente no código.

## Problema comum no Windows

Algumas configurações do Windows (como o *Controle Inteligente de Aplicativos*) podem bloquear um executável recém-compilado, com uma mensagem de "acesso negado" ou de "política de Controle de Aplicativo". Nesse caso, é preciso liberar o programa nas configurações de segurança do Windows ou pedir ajuda a quem administra o computador.
