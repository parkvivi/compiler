# Compilador `C-`

Esta é uma continuação do projeto [Compiladores-Unifesp](https://github.com/parkvivi/Compiladores-Unifesp), que foi desenvolvido inicialmente em colaboração com [@souza-luiza](https://github.com/souza-luiza). O compilador para a linguagem `C-`, conforme especificada por Louden (2004), foi modificado para a geração de linguagem de máquina com base no projeto [ARM-32bit](https://github.com/parkvivi/ARM-32bit).

## Requisitos

- **Flex** (Gerador de Analisador Léxico)
- **Bison/Yacc** (Gerador de Analisador Sintático)
- **GCC** (GNU Compiler Collection)
- **Graphviz** (Visualização da AST)
- **Make** (Ferramenta de Automação)

## Instruções para Uso

> Observação: Aqui levando em conta somente o uso do Windows

* Compilar
```powershell
make
```

*  Executar
```powershell
.\build\parser entrada\codigo.txt
```
> Observação: `codigo.txt` pode ser substituido por qualquer código teste

* Gerar arquivo .png da AST
```powershell
dot -Tpng .\build\arvore.dot -o .\build\arvore.png
```

* **[ALTERNATIVA]** Compilar + Exectar + Gerar AST
```powershell
make run
```

*  Limpar arquivos gerados
```powershell
make clean
```

## Referências

> LOUDEN, Kenneth C. Compiladores: princípios e práticas. São Paulo: Cengage Learning, 2004.
