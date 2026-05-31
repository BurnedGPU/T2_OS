
```markdown
# cmatch: Motor de Matchmaking Concurrente

Motor de emparejamiento concurrente (Matchmaking) desarrollado en C para torneos multihilo. Utiliza la librería **POSIX Threads (`pthread`)** para gestionar la sincronización de estados y recursos compartidos en tiempo real. 

Este proyecto simula un entorno de emparejamiento de jugadores basándose en un sistema de ELO estricto, prioridades por tiempo de espera y un manejo seguro de la concurrencia.

---

## ⚠️ INFORMACIÓN CRUCIAL (LEER ANTES DE EJECUTAR)

Si acabas de clonar este repositorio, ten en cuenta los siguientes detalles vitales para el correcto funcionamiento del motor:

1. **El formato del archivo `.env` (Windows vs Linux):** Si el archivo `.env` fue editado en Windows, contendrá saltos de línea ocultos (`\r\n`). Al ejecutarlo en Linux, esto romperá el *parser* de configuración y las variables se inicializarán en 0. 
   👉 **Solución:** En Linux, ejecuta siempre `dos2unix .env` antes de compilar por primera vez.
2. **Graceful Shutdown y la Terminal de CLion:** El sistema está diseñado para atrapar la señal `Ctrl+C` (`SIGINT`), permitir que los hilos terminen sus partidas en curso y volcar el progreso a un archivo (*Snapshot*) antes de cerrarse de forma segura. 
   👉 **Advertencia para Windows:** La terminal integrada del IDE CLion **no envía** la señal `SIGINT`, sino que realiza un *Hard Kill*, lo que corrompe el guardado. Para probar el apagado seguro en Windows, ejecuta el programa desde una ventana nativa de **PowerShell** o **CMD**. En Linux nativo, el `Ctrl+C` funcionará perfectamente en cualquier terminal.

---

## ✨ Características del Motor

* **Matchmaking Concurrente:** Hilos independientes para Jugadores y Tableros sincronizados mediante *Mutex* y Variables de Condición, garantizando cero *busy-waiting*.
* **Matemática ELO Segura:** Cálculo de puntaje esperado y ajuste estadístico encapsulado en una sección crítica (*elo_mutex*) libre de *Race Conditions*.
* **Snapshot (Guardado de Estado):** Vuelca las estadísticas de los jugadores a un binario (`dump.bin`). Si el torneo se interrumpe, el sistema restaura el ELO exacto en la siguiente ejecución.
* **Detector de Estancamiento (Stall Detector):** Un hilo supervisor monitorea la simulación. Si el torneo se vacía o si los jugadores restantes son incompatibles por diferencia de ELO, el sistema activa un auto-apagado limpio.

---

## 🏗️ Estructura del Proyecto

```text
├── CMakeLists.txt      # Configuración para desarrollo en IDEs (CLion / CMake)
├── .gitignore          # Archivo de exclusión de binarios y basura de SO
├── .env                # Archivo de configuración global de la simulación
├── include/            # Cabeceras (.h): Estructuras, prototipos y Mutex
└── src/                # Código fuente (.c): Lógica de hilos, ELO, monitor y snapshot

```

---

## ⚙️ Configuración del Entorno

El sistema requiere el archivo `.env` en la raíz del proyecto. Variables editables:

```env
N_PLAYERS=100             # Cantidad total de hilos de jugadores
K_BOARDS=20               # Cantidad de hilos de tableros disponibles
K_ELO=32                  # Factor K para la fórmula de ELO
MAX_ELO_DIFF=50           # Diferencia máxima de ELO para permitir un emparejamiento
TURN_DELAY_MS=100         # Milisegundos que toma cada turno (usleep)
REENTER_PROBABILITY=0.8   # Probabilidad (0.0 a 1.0) de que un jugador busque otra partida tras terminar
SNAPSHOT_PATH=dump.bin    # Nombre del archivo de guardado de estado

```

---

## 🚀 Compilación y Ejecución

### 🐧 Entorno Linux (Recomendado / Evaluación Estricta)

Asegúrate de tener instalados los paquetes de compilación esenciales. La compilación se realiza usando el estándar estricto de C17 y enlazando la librería matemática (`-lm`) y de hilos (`-lpthread`).

```bash
# 1. Instalar requerimientos (Ubuntu/Debian/Mint)
sudo apt update && sudo apt install build-essential dos2unix

# 2. Curar el archivo de configuración (Vital)
dos2unix .env

# 3. Compilación estricta GCC
gcc -Wall -Wextra -std=c17 -lpthread src/*.c -I./include -o cmatch -lm

# 4. Ejecutar el simulador
./cmatch

```

### 🪟 Entorno Windows (PowerShell)

Si utilizaste CLion para generar los archivos de construcción (CMake), el ejecutable se encontrará en tu carpeta `cmake-build-debug`.

```powershell
# 1. Navegar al directorio de salida de CLion
cd cmake-build-debug

# 2. Ejecutar el simulador
.\cmatch.exe

# 3. Probar el Graceful Shutdown
# Presiona Ctrl+C en esta ventana. Verás el mensaje de apagado seguro y la creación del dump.bin.

```

```

```
