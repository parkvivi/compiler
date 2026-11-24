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
├── documentacao/   # documentação do compilador
├── entrada/        # CÓDIGOS TESTE
|   ├── lexico/                 # teste do analisador léxico
|   ├── sintatico/              # teste do analisador sintático
|   ├── semantico/              # teste do analisador semântico
|   ├── auxiliar/               # auxiliares
|   └── main/                   # principais
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

### 2. Compilar o sistema operacional e o BIOS

```powershell
make run
```

O alvo `run` executa os dois códigos-fonte:

- `os/os.c`
- `bios/bios.c`

Os artefatos gerados são separados em:

- `os/`
- `bios/`

Cada pasta contém:

- `*.png`: AST correspondente ao código-fonte
- `tabsimbolos.txt`: lista de variáveis e funções declaradas no código de entrada, separadas por escopo
- `codigointermediario.txt`: código intermediário gerado
- `assembly.txt`: código assembly gerado
- `executavel.txt`: código binário gerado

Para executar apenas um dos códigos:

```powershell
make run-os
make run-bios
```

### 3. Limpar arquivos gerados

```powershell
make clean
```

Esse comando remove o diretório `build` e os artefatos gerados em `os/` e `bios/`.

## Alvos do Makefile

- `make` ou `make all`: compila o parser
- `make parser`: compila o parser
- `make run`: compila (se necessário) e executa o OS e o BIOS
- `make run-os`: executa somente `os/os.c`
- `make run-bios`: executa somente `bios/bios.c`
- `make clean`: remove `build` e os artefatos gerados em `os/` e `bios/`

## Referências

> LOUDEN, Kenneth C. Compiladores: princípios e práticas. São Paulo: Cengage Learning, 2004.
