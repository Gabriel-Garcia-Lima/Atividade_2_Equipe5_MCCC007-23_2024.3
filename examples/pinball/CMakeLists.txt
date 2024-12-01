project(pinball)
add_executable(${PROJECT_NAME} main.cpp window.cpp render.cpp gamedata.cpp)
enable_abcg(${PROJECT_NAME})
