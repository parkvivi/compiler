# Compilador `C-`

Esta é uma continuação do projeto [compiler-v0](https://github.com/parkvivi/compiler-v0), que foi desenvolvido inicialmente em colaboração com [@souza-luiza](https://github.com/souza-luiza). O compilador para a linguagem `C-`, conforme especificada por Louden (2004), foi modificado para a geração de linguagem de máquina com base no projeto [ARM-32bit](https://github.com/parkvivi/ARM-32bit).

## Requisitos

- **Flex** (Gerador de Analisador Léxico)
- **Bison/Yacc** (Gerador de Analisador Sintático)
- **GCC** (GNU Compiler Collection)
- **Graphviz** (Visualização da AST)
- **Make** (Ferramenta de Automação)

## Uso

### 1. Compilar

```powershell
make
```

Esse comando gera o executável do compilador em `build/parser.exe` (Windows) ou `build/parser` (Linux/macOS).

### 2. Rodar os testes principais (`gcd` e `sort`)

```powershell
make run
```

O alvo `run` executa:

- `entrada/main/gcd.txt`
- `entrada/main/sort.txt`

E salva os artefatos separadamente em:

- `build/gcd`
- `build/sort`

Cada pasta contém:

- `arvore.dot`
- `arvore.png`
- `tabsimbolos.txt`
- `codigointermediario.txt`

### 3. Rodar um arquivo de teste qualquer

Você pode rodar manualmente qualquer entrada:

```powershell
.\build\parser.exe entrada\seu_teste.txt
```

Por padrão, os arquivos de saída vão para `build/`.

Se quiser isolar a saída em uma pasta específica, use a variável de ambiente `COMPILER_OUTPUT_DIR`:

```powershell
if (!(Test-Path build\meu_teste)) { New-Item -ItemType Directory -Path build\meu_teste | Out-Null }
$env:COMPILER_OUTPUT_DIR = "build\meu_teste"
.\build\parser.exe entrada\seu_teste.txt
dot -Tpng build\meu_teste\arvore.dot -o build\meu_teste\arvore.png
Remove-Item Env:COMPILER_OUTPUT_DIR
```

### 4. Limpar arquivos gerados

```powershell
make clean
```

Esse comando remove todo o diretório `build`.

## Alvos do Makefile

- `make` ou `make all`: compila o parser
- `make parser`: compila o parser
- `make run`: compila (se necessário) e executa `gcd` e `sort`
- `make run-gcd`: executa somente `entrada/main/gcd.txt`
- `make run-sort`: executa somente `entrada/main/sort.txt`
- `make scanner`: compila o scanner em modo standalone
- `make clean`: remove `build`

## Referências

> LOUDEN, Kenneth C. Compiladores: princípios e práticas. São Paulo: Cengage Learning, 2004.
