# Projeto Bloxorz - Equipe 5

### Projeto em Web Assembly

Para acessar, [clique aqui](./public/cube_trail.html).

### Integrantes

- Gabriel Garcia de Lima - 11201932522
- Fernando Hiroaki Suzuki - 11202130281
- Leonardo Fabiano de Sousa - 11201721317

### Descrição

Nosso projeto é uma implementação do clássico jogo de lógica e estratégia Bloxorz, onde o objetivo é guiar um bloco retangular através de um tabuleiro até encaixá-lo verticalmente em um buraco. A mecânica envolve movimentação estratégica, evitando cair para fora do tabuleiro e explorando diferentes orientações do bloco durante os movimentos. Esta implementação utiliza a biblioteca OpenGL para renderização gráfica e interação 3D.

## Como Funciona

1. O jogador controla um bloco 3D retangular que pode estar em três estados principais:

   - **Em pé (STANDING)**: O bloco ocupa um único quadrado do tabuleiro.
   - **Deitado no eixo X (LAYING_X)**: O bloco ocupa dois quadrados adjacentes horizontalmente.
   - **Deitado no eixo Z (LAYING_Z)**: O bloco ocupa dois quadrados adjacentes verticalmente.

2. O tabuleiro é uma grade quadriculada que contém um buraco. O bloco deve ser posicionado exatamente em pé sobre o buraco para vencer.

3. Movimentos são realizados em quatro direções (cima, baixo, esquerda, direita). Cada movimento altera a orientação ou posição do bloco.

4. Se o bloco cair para fora do tabuleiro ou não estiver corretamente alinhado, o jogo reinicia.

---

## Funcionalidades

- **Movimentação do Bloco**: O bloco pode se mover de forma fluida, com rotações e animações calculadas dinamicamente.
- **Tabuleiro Dinâmico**: Um buraco é gerado aleatoriamente no início de cada partida.
- **Detecção de Vitória e Derrota**:
  - Vitória: Quando o bloco está em pé exatamente sobre o buraco.
  - Derrota: Quando o bloco cai fora do tabuleiro ou não se ajusta ao buraco.
- **Animações Suaves**: O jogo utiliza transformações matriciais para animar os movimentos do bloco, garantindo uma experiência visual fluida.
- **Sistema de Reposicionamento**: Ao reiniciar, o bloco é posicionado em uma nova localização válida, enquanto o buraco é reposicionado de forma aleatória.

---

## Controles

- **Teclas de Direção**:
  - **W** ou **Seta para Cima**: Move o bloco para cima.
  - **S** ou **Seta para Baixo**: Move o bloco para baixo.
  - **A** ou **Seta para Esquerda**: Move o bloco para a esquerda.
  - **D** ou **Seta para Direita**: Move o bloco para a direita.

---

## Estrutura do Código

O projeto está dividido em três componentes principais, cada um encapsulado em um arquivo `.cpp`.

### Arquivos Principais

1. **`cube.cpp`**: Gerencia as funcionalidades relacionadas ao bloco, incluindo:
   - Movimentação.
   - Controle de estados e animações.
   - Detecção de queda e resete.
2. **`ground.cpp`**: Lida com a lógica do tabuleiro, como:
   - Verificação de limites.
   - Gerenciamento do buraco (posição e validade).
   - Renderização do tabuleiro.
3. **`window.cpp`**: Controla a interação entre o usuário e o jogo:
   - Processamento de eventos do teclado.
   - Integração entre o bloco e o tabuleiro.
   - Configuração da renderização e animação.

---

### Funções Importantes

#### `cube.cpp`

- **`Cube::move(float deltaTime)`**:
  Controla o movimento do bloco com base no estado atual e na direção, aplicando transformações matriciais para rotações e translações.

- **`Cube::generateRandomPosition()`**:
  Gera uma nova posição válida para o bloco, garantindo que ele não inicie sobre o buraco.

- **`Cube::resetGame()`**:
  Reposiciona o bloco e o buraco, reiniciando o jogo.

- **`Cube::paint()`**:
  Renderiza o bloco com as cores e posições apropriadas.

#### `ground.cpp`

- **`Ground::create()`**:
  Configura o tabuleiro, incluindo o tamanho, escala e posição inicial do buraco.

- **`Ground::isTile(int x, int z)`**:
  Verifica se uma posição é válida no tabuleiro (não é buraco ou fora dos limites).

- **`Ground::randomizeHole()`**:
  Define aleatoriamente a posição do buraco no tabuleiro.

#### `window.cpp`

- **`Window::onEvent(SDL_Event const &event)`**:
  Mapeia as entradas do teclado para os movimentos do bloco.

- **`Window::onPaint()`**:
  Controla a renderização do bloco e do tabuleiro.

- **`Window::onUpdate()`**:
  Atualiza o estado do bloco a cada quadro, aplicando animações e lógica de movimentação.

---

### Detalhes da Lógica

1. **Movimentação e Estados do Bloco**:

   - O estado do bloco determina como ele se move e ocupa o tabuleiro.
   - As rotações são calculadas com base em eixos de rotação e pontos de pivô.
   - Ao atingir o limite do tabuleiro ou o buraco incorretamente, o estado de queda é acionado.

2. **Gerenciamento do Tabuleiro**:

   - O tabuleiro é gerado como uma matriz 2D. Cada posição pode ser uma célula válida ou o buraco.
   - A função `isTile()` garante que o bloco não ultrapasse os limites ou caia em locais inválidos.

3. **Detecção de Vitória/Derrota**:

   - **Vitória**: O bloco está em pé (`STANDING`) e sua posição coincide com a do buraco.
   - **Derrota**: O bloco ultrapassa os limites ou não se posiciona corretamente sobre o buraco.

4. **Animação Suave**:

   - Rotações e translações do bloco são calculadas frame a frame usando transformações matriciais para garantir fluidez.

5. **Aleatoriedade no Jogo**:
   - O buraco é gerado em posições aleatórias, garantindo que cada partida seja única.

---

Para mais detalhes sobre a lógica ou a estrutura de implementação, consulte os arquivos de código anexados.
