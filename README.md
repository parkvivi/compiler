# Compilador `C-`

Este repositório contém um compilador para a linguagem `C-`, conforme especificada por Louden (2004), modificado para a geração de linguagem de máquina com base no projeto [ARM-32bit](https://github.com/parkvivi/ARM-32bit).

## Requisitos

- **Flex** (Gerador de Analisador Léxico)
- **Bison/Yacc** (Gerador de Analisador Sintático)
- **GCC** (GNU Compiler Collection)
- **Graphviz** (Visualização da AST)
- **Make** (Ferramenta de Automação)

## Estrutura do Repositório
```
compiler/
├── src/            # CÓDIGO-FONTE
|   ├── lexico/                 # analisador léxico
|   ├── sintatico/              # analisador sintático
|   ├── ast/                    # tratamento da AST
|   ├── dot/                    # criação do png da AST
|   ├── semantico/              # analisador semântico
|   ├── gci/                    # gerador de código intermediário
|   ├── assembly/               # gerador de código assembly
|   ├── executavel/             # gerador de código binário
|   └── main/                   # código principal
├── user_programs/  # ENTRADAS (PROGRAMAS DE USUÁRIO)
├── .gitignore
├── Makefile        # automação da compilação
└── README.md       # documentação do repositório
```

## Uso

### 1. Compilar

```powershell
make
```

Esse comando gera o executável do compilador em `build/parser.exe` (Windows) ou `build/parser` (Linux/macOS).

### 2. Compilar os programas de usuário

```powershell
make run
```

O alvo `run` compila o parser e processa todos os arquivos `.c` encontrados
em `user_programs/<nome-do-programa>/`. Os artefatos de cada programa são
salvos na própria pasta do programa.

Para processar apenas um programa:

```powershell
make run-fibonacci
```

O nome usado no alvo corresponde ao nome da pasta dentro de `user_programs`.

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
- `make run`: compila o parser e todos os programas em `user_programs`
- `make run-<nome>`: compila somente o programa da pasta indicada
- `make clean`: remove `build`

## Referências

> LOUDEN, Kenneth C. Compiladores: princípios e práticas. São Paulo: Cengage Learning, 2004.
