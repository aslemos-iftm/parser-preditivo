# Parser preditivo — instruções para a IA

## O projeto

Parser preditivo LL(1) escrito em C, usado como material didático em Teoria da Computação (Licenciatura em Computação) e em Introdução ao Desenvolvimento de Sistemas (Técnico em Desenvolvimento de Sistemas, IFTM). Ele lê uma expressão, consulta uma tabela de lookahead e constrói a árvore de derivação usando uma pilha e uma árvore genérica.

Gramática atual (`$` marca o fim da entrada):

```
S → A R
R → + A R | ε
A → X B
B → * X B | ε
X → a | b | c | ( S )
```

Arquivos em `src/`:

- `PilhaAutomato.c/.h`: a pilha (lista encadeada de `char`).
- `ArvoreAutomato.c/.h`: a árvore genérica (primeiro filho / próximo irmão / pai).
- `ParserPreditivo.c/.h`: a tabela de lookahead e o laço do parser.
- `main.c`: entrada e saída.

## Quem trabalha aqui e como

O professor quer **entender cada mudança**, mesmo sem escrever o código. Por isso:

- Responda em português do Brasil.
- Trabalhe em passos pequenos. Antes de mudar mais de um arquivo, ou de mudar o comportamento do programa, apresente um plano curto e espere a aprovação.
- Ao propor uma mudança, explique o que ela faz e por quê, em linguagem acessível. Não presuma familiaridade avançada com C.
- Quando houver mais de um caminho razoável, mostre as opções com os prós e contras, em vez de escolher em silêncio.
- Não reescreva o que não foi pedido. Preserve o estilo e os nomes em português do código original.
- Este código também é lido por estudantes iniciantes. Prefira clareza a esperteza.

## Premissas didáticas do código

Algumas escolhas que parecem defeitos são deliberadas: o código foi escrito para ser lido por iniciantes, e a legibilidade tem prioridade. Antes de mudar qualquer uma delas, explique o problema e proponha a mudança; não a faça por conta própria.

- **Os nós vazios.** Nas produções vazias (ε), `insereSubarvore` recebe a cadeia `""` e cria um nó com o caractere `'\0'`. Isso deixa explícita, na árvore e na sua impressão, a aplicação da produção vazia. O nó também é estrutural: `insereProd` procura o primeiro não terminal **sem filhos**, e o nó vazio é o que marca que `R` ou `B` já foram expandidos. Removê-lo quebra a construção da árvore.
- **A entrada simples.** A expressão é lida com `scanf("%s")` num vetor de `INPUTSIZE` (20) caracteres. É a forma mais simples de leitura para quem está começando; o risco de uma entrada longa demais é conhecido.
- **O `$` no fim da entrada.** Marca o fim da cadeia, como na teoria dos parsers preditivos. Tem valor didático e deve ser mantido.
- **A tabela escrita à mão** em `preencheTabela`, entrada por entrada, para que se veja a tabela de lookahead no código.

O programa deve compilar sem avisos com `gcc -Wall -Wextra -std=c11 src/*.c -o parser`.

## Compilar e testar

```
gcc -Wall -Wextra -std=c11 src/*.c -o parser
```

Entradas de teste (a entrada termina com `$`):

| Entrada | Esperado |
|---|---|
| `a+b*c$` | reconhecida |
| `(a+b)*c$` | reconhecida |
| `a+$` | erro: não esperava `$` |
| `ab$` | erro: não esperava `b` |
| `a+b*c` | erro: fim inesperado da entrada |

Depois de qualquer mudança, compile e rode estas entradas antes de dizer que terminou.
