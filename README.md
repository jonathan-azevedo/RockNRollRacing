========================================================================
             ROCK N' ROLL RACING - REMAKE (INF01202 UFRGS)
========================================================================

DESCRIÇÃO
---------
Remake do clássico Rock N' Roll Racing (SNES) desenvolvido em C utilizando 
a biblioteca Raylib. Projeto final da disciplina de Algoritmos e 
Programação (INF01202) da UFRGS.

O jogo apresenta um sistema de corrida arcade com combate, incluindo:
- Física de colisão entre carros e cenário.
- Inteligência Artificial para pilotos inimigos.
- Sistema de itens (Mísseis, Bombas, Nitro, Escudo).
- Save/Load de progresso.
- Múltiplas pistas e veículos selecionáveis.

COMO EXECUTAR
-------------
Windows:
1. Certifique-se de que a pasta "Assets" está no mesmo diretório do executável.
2. Execute o arquivo "game.exe".

Compilação:
Requer o compilador GCC e a biblioteca Raylib instalada.
Executar no terminal o seguinte comando: 
gcc src/*.c -o {Caminho da pasta onde estão os arquivos}\game.exe -O2 -Wall -Wno-missing-braces -static -IC:/raylib/raylib/src -LC:/raylib/raylib/src -Isrc -Iinclude -lraylib -lopengl32 -lgdi32 -lwinmm 

INSTRUÇÕES DE JOGO
------------------
1. No Menu Principal, utilize o mouse para selecionar as opções (New Game, Continue, Exit).
2. Na tela de Setup, configure seu carro, mapa, número de voltas e inimigos.
3. Clique em "START RACE" para iniciar.

CONTROLES
---------
[ W ] ou [ Seta Cima ]    : Acelerar
[ S ] ou [ Seta Baixo ]   : Ré / Freio
[ A ] ou [ Seta Esquerda ]: Virar à Esquerda
[ D ] ou [ Seta Direita ] : Virar à Direita
[ ESPAÇO ]                : Usar Item Coletado (Tiro, Bomba, Nitro ou Escudo)
[ ESC ]                   : Pausar o Jogo / Acessar Menu de Save

DESENVOLVEDORES
---------------
1. Jonathan Azevedo
2. Felipe Burmann

Engenharia de Computação - UFRGS
-----
Jonathan [Seu Sobrenome] - Engenharia de Computação (UFRGS)
[Adicione nomes de colegas se houver]
