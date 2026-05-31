# cmatch: Motor de Matchmaking Concurrente

Motor de emparejamiento concurrente (Matchmaking) desarrollado en C para torneos 
multihilo. Utiliza la librería **POSIX Threads (`pthread`)** para gestionar la 
sincronización de estados y recursos compartidos en tiempo real. 

Este proyecto simula un entorno de emparejamiento de jugadores basándose en un 
sistema de ELO estricto, prioridades por tiempo de espera y un manejo seguro 
de la concurrencia.

---

## ⚠️ INFORMACIÓN CRUCIAL (LEER ANTES DE EJECUTAR)

Si acabas de clonar este repositorio, ten en cuenta los siguientes detalles 
vitales para el correcto funcionamiento del motor:

1. **El formato del archivo `.env` (Windows vs Linux):** Si el archivo `.env` 
   fue editado en Windows, contendrá saltos de línea ocultos (`\r\n`). Al 
   ejecutarlo en Linux, esto romperá el *parser* de configuración y las 
   variables se inicializarán en 0. 
   👉 **Solución:** En Linux, ejecuta siempre `dos2unix .env` antes de 
   compilar por primera vez.

2. **Graceful Shutdown y la Terminal de CLion:** El sistema está diseñado para 
   atrapar la señal `Ctrl+C` (`SIGINT`), permitir que los hilos terminen sus 
   partidas en curso y volcar el progreso a un archivo (*Snapshot*) antes de 
   cerrarse de forma segura. 
   👉 **Advertencia para Windows:** La terminal integrada del IDE CLion **no 
   envía** la señal `SIGINT`, sino que realiza un *Hard Kill*, lo que corrompe 
   el guardado. Para probar el apagado seguro en Windows, ejecuta el programa 
   desde una ventana nativa de **PowerShell** o **CMD**. En Linux nativo, el 
   `Ctrl+C` funcionará perfectamente en cualquier terminal.
   
3. Si en las pruebas de uso del programa se nota que en la terminal no se despliega
   informacion alguna sobre lo que esta ocurriendo en la simulacion, esto es debido
   a un conflicto entre poder ver como se desarrolla el programa v/s poder
   interactuar con el mismo mediante el panel de monitrizacion. Esto se debe a que
   no era posible consultar la informacion de la simulacion al mismo tiempo que esta
   estaba corriendo en paralelo. Es por ello que solo se dispone de un tiempo acotado
   para utilizar la interfaz de monitoreo antes de que el programa finalize
   su ciclo de vida.
   `Para poder ver la informacion completa, se debe editar manualmente los archivos
   ´main.c, player.c y board.c y desmarcar los comentarios de todos los printf`
---

## ✨ Características del Motor

* **Matchmaking Concurrente:** Hilos independientes para Jugadores y Tableros 
  sincronizados mediante *Mutex* y Variables de Condición, garantizando cero 
  *busy-waiting*.
* **Matemática ELO Segura:** Cálculo de puntaje esperado y ajuste estadístico 
  encapsulado en una sección crítica (*elo_mutex*) libre de *Race Conditions*.
* **Snapshot (Guardado de Estado):** Vuelca las estadísticas de los jugadores 
  a un binario (`dump.bin`). Si el torneo se interrumpe, el sistema restaura 
  el ELO exacto en la siguiente ejecución.
* **Detector de Estancamiento (Stall Detector):** Un hilo supervisor monitorea 
  la simulación. Si el torneo se vacía o si los jugadores restantes son 
  incompatibles por diferencia de ELO, el sistema activa un auto-apagado limpio.

---

## 🏗️ Estructura del Proyecto

```text
├── CMakeLists.txt  s    # Configuración para IDEs (CLion / CMake)
├── .gitignore          # Archivo de exclusión de binarios y basura
├── .env                # Archivo de configuración global de la simulación
├── include/            # Cabeceras (.h): Estructuras, prototipos y Mutex
└── src/                # Código fuente (.c): Lógica, ELO, monitor, snapshot
```

## ⚙️ Configuración del Entorno
## El sistema requiere el archivo .env en la raíz del proyecto. Variables:
```text
Fragmento de código
N_PLAYERS=100             # Cantidad total de hilos de jugadores
K_BOARDS=20               # Cantidad de hilos de tableros disponibles
K_ELO=32                  # Factor K para la fórmula de ELO
MAX_ELO_DIFF=50           # Diferencia máxima de ELO permitida
TURN_DELAY_MS=100         # Milisegundos que toma cada turno (usleep)
REENTER_PROBABILITY=0.8   # Probabilidad (0.0 a 1.0) de seguir jugando
SNAPSHOT_PATH=dump.bin    # Nombre del archivo de guardado de estado
```

## 🚀 Compilación y Ejecución
## 🐧 Entorno Linux
## La compilación se realiza usando el estándar estricto de C17 y enlazando la
## librería matemática (-lm) y de hilos (-lpthread).
```text
Bash
# 1. Dirigirse a la terminal de linux y clonar el
repositorio con 'git clone https://github.com/BurnedGPU/T2_OS'

# 2. ingresar a la carpeta del repositorio con 'cd T2_OS/'

# 3. Instalar requerimientos (Ubuntu/Debian/Mint)
sudo apt update && sudo apt install build-essential dos2unix

# 4. Curar el archivo de configuración (Vital)
dos2unix .env

# 5. Compilación estricta GCC
gcc -Wall -Wextra -std=c17 -lpthread src/*.c -I./include -o cmatch -lm

# 6. Ejecutar el simulador
./cmatch

```
## 🪟 Entorno Windows (PowerShell)
```text
PowerShell
# 1. Navegar al directorio de salida del IDE
cd cmake-build-debug

# 2. Ejecutar el simulador
.\cmatch.exe

# 3. Probar el Graceful Shutdown
# Presiona Ctrl+C en esta ventana. Verás el mensaje de apagado seguro.
```
## PRUEBAS DE USO DEL PROGRAMA
Si el programa fue compilado en linux exitosamente y se puede ver en la terminal el ciclo de vida del mismo de inicio a fin
sin ningun problema, mostrando el mensaje clave 'simulacion finalizada correctamente', entonces lo primordial esta hecho.

En base a esto, lo siguente es realizar pruebas segun lo indicado en el instructivo de la tarea.

### GRACEFUL SHUTDOWN
Para probar si la simulacion se detiene adecuadamente sin que todo se frene de golpe,
solo es necesario detener la simulacion con `ctrl+c` y observar como aparecen los mensajes
de apagado respectivos.

### FUNCIONAMIENTO DEL SNAPSHOT
El snapshot se encarga de que podamos detener una simulacion, guardar los datos de los jugadores
e iniciar una nueva simulacion cargaando los datos guardados de la simulacion anterior.
Para probar su funcionamiento, se escoge a un jugador al azar (ej. p17), se toma nota de sus
datos mediante el panel de monitorizacion y se detiene la simulacion presionando la tecla 's'.
Posterior a eso se inicia una nueva simulacion con `./cmatch` y rapidamente se vuelven a consultar
los datos de ese mismo jugador. Si la cantidad de partidas ganadas, perdidas o empatadas 
se mantienen iguales o son mayores, entonces los datos fueron restaurados exitosamente.

### MONITORIZACION DE LA PARTIDA
Tal como se indica en las instrucciones, debe ser posible monitorear el estado de la simulacion 
mediante una interfaz, ingresando los respectivos "id's" para llamar a las funciones indicadas.
Para lograr esto, se dispone de un CLI que se despliega al inicio de cada simulacion. 
cada una de las opciones representa las funciones de monitoreo indicadas teniendo asi que 
* la m proporciona informacion de las partidas en curso
* la p imprime los datos de un jugador segun su id (ej. p4)
* la t imprime los datos de un tablero especifico segun su id (ej. t2)
* la s detiene la simulacion por completo

Para probar cada una de las funciones se debe escribir en la terminal dichos
comandos mientras la partida esta en curso, por lo que toda la informacion desplegada
dependera de que tan rapido se consulten los datos antes de que la simulacion llegue a su fin.
