// main.cpp

#include "abcg.hpp"
#include "window.hpp"

int main(int argc, char **argv) {
  try {
    // Cria a aplicação ABCg
    abcg::Application app(argc, argv);

    // Instancia a janela principal do jogo
    Window window;

    // Configurações do OpenGL
    window.setOpenGLSettings({
      .samples = 4, // Anti-aliasing
      .preserveWebGLDrawingBuffer = false
    });

    // Configurações da janela
    window.setWindowSettings({
      .width = 1024,
      .height = 768,
      .title = "Bloxorz 3D"
    });

    // Executa a aplicação
    app.run(window);

    return 0;
  } catch (std::exception const &e) {
    // Imprime qualquer erro que tenha ocorrido
    std::cerr << "Erro: " << e.what() << std::endl;
    return -1;
  }
}
