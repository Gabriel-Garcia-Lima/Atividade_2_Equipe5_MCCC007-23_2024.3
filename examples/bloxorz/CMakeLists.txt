project(bloxorz)
# Arquivos fonte e cabeçalhos
set(SOURCES
    main.cpp
    window.cpp
    camera.cpp
    level.cpp
    playerblock.cpp
)

set(HEADERS
    window.hpp
    camera.hpp
    level.hpp
    playerblock.hpp
    direction.hpp
)

# Adiciona o executável
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
enable_abcg(${PROJECT_NAME})
