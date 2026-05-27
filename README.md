# cmatch

Motor de emparejamiento concurrente (Matchmaking) desarrollado en C para torneos multihilo. Utiliza la librería **POSIX Threads (`pthread`)** para gestionar la sincronización de estados y recursos compartidos en tiempo real.

## 🏗️ Estructura del Proyecto

```text
├── CMakeLists.txt      # Configuración para desarrollo en CLion / CMake
├── Makefile            # Script de compilación oficial para la terminal (GCC)
├── .env                # Archivo de configuración global de la simulación (requerido)
├── include/            # Cabeceras (.h): Definición de estructuras, prototipos y Mutex globales
└── src/                # Código fuente (.c): Lógica de hilos, lectura de config y sincronización
```

## 🚀 Cómo arrancar el proyecto

### 1. Requisitos previos
- Compilador **GCC** (soporte para el estándar C17).
- Entorno Linux, WSL o MinGW (Windows).
- Librería nativa `pthread`.

### 2. Configuración del entorno
El sistema requiere un archivo `.env` en la raíz del proyecto para inicializar la memoria y los hilos. Ejemplo de formato esperado:

```env
N_PLAYERS=100
K_BOARDS=20
K_ELO=32
MAX_ELO_DIFF=50
TURN_DELAY_MS=100
REENTER_PROBABILITY=0.8
SNAPSHOT_PATH=dump.bin
```

### 3. Compilación y Ejecución
Utiliza el `Makefile` incluido para automatizar la construcción con los flags correctos (`-Wall -Wextra -std=c17 -lpthread`):

```bash
# 1. Compilar el proyecto
make

# 2. Ejecutar el motor
./cmatch

# 3. Limpiar los archivos binarios compilados (.o y .exe)
make clean
```
