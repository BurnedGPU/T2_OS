cmake_minimum_required(VERSION 3.28)
project(cmatch C)

set(CMAKE_C_STANDARD 17)

# Incluir la carpeta de cabeceras
include_directories(include)

# Agregar TODOS los archivos .c al ejecutable
add_executable(cmatch
        src/main.c
        src/config.c
        src/matchmaker.c
        src/player.c
        src/board.c
        src/elo.c
        src/monitor.c
        src/snapshot.c
)

# Enlazar la libreria pthread y la libreria matematica (m)
target_link_libraries(cmatch pthread m)
