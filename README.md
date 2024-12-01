# Projeto Pinball - Equipe 5

## Tempo de Reação com Pinball

Este projeto visa desenvolver um jogo 2D inspirado no Pinball, com o objetivo de avaliar o tempo de reação de indivíduos com dificuldades motoras em tratamento. Através deste jogo, será possível monitorar a evolução e a melhora dos participantes ao longo do tempo. O uso de jogos como ferramenta de reabilitação representa uma abordagem interessante para o acompanhamento do progresso terapêutico.

### Integrantes

- Gabriel Garcia de Lima - 11201932522
- Fernando Hiroaki Suzuki - 11202130281
- Leonardo Fabiano de Sousa - 11201721317

Este é um jogo (Pinball) interativo 2D inspirado em pinball, onde a bola pode colidir com paredes, obstáculos e flippers. O objetivo principal é manipular a bola através de controles para evitar que ela saia da tela, enquanto interage com obstáculos no caminho.

## Como Funciona

- O jogo consiste em uma área de jogo delimitada por paredes (superior, inferior, esquerda e direita), e o jogador controla dois flippers que podem ser movidos para desviar ou impulsionar a bola.
- A bola começa com uma velocidade inicial e pode colidir com paredes e obstáculos no jogo. Quando a bola colide com um obstáculo, ela pode mudar sua direção com base no ângulo da colisão.
- Além disso, existem obstáculos adicionais gerados aleatoriamente em posições dentro da área de jogo.

## Funcionalidades

- **Flippers**: O jogador pode controlar os flippers com as teclas de seta esquerda e direita.
- **Obstáculos**: Obstáculos circulares são gerados aleatoriamente na parte superior da tela.
- **Controle da Bola**: A bola se move e interage fisicamente com os flippers, paredes e obstáculos.

## Controles

- **Espaço**: Iniciar o jogo (lançar a bola).
- **Seta para a Esquerda**: Ativar o flipper esquerdo.
- **Seta para a Direita**: Ativar o flipper direito.

## Estrutura do Código

### Arquivos Principais

- **window.cpp**: Contém a maior parte da lógica do jogo, incluindo a renderização da bola, flippers, obstáculos e detecção de colisões.
- **Shaders**: Os shaders são usados para transformar e colorir os objetos (bola, flippers, obstáculos) na tela.

### Funções Importantes

- **onCreate()**: Inicializa a janela e configura os shaders, a posição dos obstáculos e a configuração inicial do jogo.
- **onUpdate()**: Atualiza a posição da bola com base no tempo (deltaTime) e verifica colisões.
- **onEvent()**: Processa eventos de teclado, como pressionar as teclas de controle do flipper e iniciar o jogo.
- **onPaint()**: Realiza o desenho dos objetos na tela, incluindo a bola, flippers e obstáculos.
- **checkCollisions()**: Verifica colisões entre a bola e os flippers, paredes e obstáculos.

### Detalhes da Lógica

- **Obstáculos**: São gerados aleatoriamente, mas sempre afastados das paredes para evitar que fiquem fora da área visível. Eles são desenhados como círculos brancos.
- **Flippers**: São representados por dois objetos, um à esquerda e outro à direita. Eles têm uma rotação controlada pelas teclas de seta.
- **Bola**: A bola se move de acordo com a velocidade e direção, sendo afetada por colisões com obstáculos e flippers.

## Como Compilar e Executar

1. **Instalar Dependências**:

   - OpenGL
   - GLSL
   - GLM
   - SDL2

2. **Compilação**:
   Compile o código utilizando um compilador C++ com suporte ao OpenGL, GLM e SDL2.

3. **Execução**:
   Após a compilação, execute o arquivo gerado. O jogo será iniciado e você poderá interagir com ele usando as teclas definidas.
