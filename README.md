# Compilador `C-`

Esta é uma continuação do projeto [compiler-v0](https://github.com/parkvivi/compiler-v0). O compilador para a linguagem `C-`, conforme especificada por Louden (2004), foi modificado para a geração de linguagem de máquina com base no projeto [ARM-32bit](https://github.com/parkvivi/ARM-32bit).

## Requisitos

- **Flex** (Gerador de Analisador Léxico)
- **Bison/Yacc** (Gerador de Analisador Sintático)
- **GCC** (GNU Compiler Collection)
- **Graphviz** (Visualização da AST)
- **Make** (Ferramenta de Automação)

## Estrutura do Repositório
```
compiler/
├── documentacao/   # documentação do compilador
├── entrada/        # CÓDIGOS TESTE
|   ├── lexico/                 # teste do analisador léxico
|   ├── sintatico/              # teste do analisador sintático
|   ├── semantico/              # teste do analisador semântico
|   ├── auxiliar/               # auxiliares
|   └── main/                   # principais
└── src/            # CÓDIGO-FONTE
    ├── lexico/                 # analisador léxico
    ├── sintatico/              # analisador sintático
    ├── ast/                    # tratamento da AST
    ├── dot/                    # criação do png da AST
    ├── semantico/              # analisador semântico
    ├── gci/                    # gerador de código intermediário
    ├── assembly/               # gerador de código assembly
    ├── executavel/             # gerador de código binário
    └── main/                   # código principal
```

## Uso

### 1. Compilar

```powershell
make
```

Esse comando gera o executável do compilador em `build/parser.exe` (Windows) ou `build/parser` (Linux/macOS).

### 2. Rodar os testes principais (`fatorial`, `gcd` e `sort`)

```powershell
make run
```

O alvo `run` executa:

- `entrada/main/fatorial.txt`
- `entrada/main/gcd.txt`
- `entrada/main/sort.txt`

E salva os artefatos separadamente em:

- `build/fatorial`
- `build/gcd`
- `build/sort`

Cada pasta contém:

- `*.png`: AST correspondente ao código-fonte
- `tabsimbolos.txt`: lista de variáveis e funções declaradas no código de entrada, separadas por escopo
- `codigointermediario.txt`: código intermediário gerado
- `assembly.txt`: código assembly gerado
- `executavel.txt`: código binário gerado

### 3. Limpar arquivos gerados

```powershell
make clean
```

Esse comando remove todo o diretório `build`.

## Alvos do Makefile

- `make` ou `make all`: compila o parser
- `make parser`: compila o parser
- `make run`: compila (se necessário) e executa `fatorial`, `gcd` e `sort`
- `make run-fatorial`: executa somente `entrada/main/fatorial.txt`
- `make run-gcd`: executa somente `entrada/main/gcd.txt`
- `make run-sort`: executa somente `entrada/main/sort.txt`
- `make scanner`: compila o scanner em modo standalone
- `make clean`: remove `build`

## Referências

> LOUDEN, Kenneth C. Compiladores: princípios e práticas. São Paulo: Cengage Learning, 2004.
