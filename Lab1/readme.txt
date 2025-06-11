MYBASH

DESCRIPCIÓN GENERAL:
    MyBash es un shell personalizado que permite la ejecución de comandos internos como cd, exit, help, echo y pwd, así como de comandos externos.
    Este proyecto implementa una estructura básica de procesamiento de comandos, redirección de entrada y salida, y manejo de pipelines.

COMANDOS IMPLEMENTADOS:
    cd: Cambia el directorio de trabajo actual.
    Uso: cd [directorio]

    exit: Termina la ejecución del shell.
    Uso: exit

    help: Muestra información sobre los autores del shell y una breve descripción de los comandos internos implementados.
    Uso: help

    echo: Imprime un mensaje en la salida estándar.
    Uso: echo [mensaje]

    pwd: Muestra el directorio de trabajo actual.
    Uso: pwd

ESTRUCTURA DEL PROYECTO:

1. commands.h y command.c
    En estos archivos definimos la estructura de los comandos que se ejecutan en la shell.
    No tuvimos demasiada dificultad ya que teníamos conocimiento previo de Algoritmos II.

2. builtin.h y builtin.c
    En este módulo implementamos los comandos internos del shell, como cd, exit y help. 
    Decidimos agregar también echo y pwd porque nos parecieron dos comandos no difíciles de implementar, que podrían ser útiles al usarlos. 
    Además, definimos una función get_command_type para extraer el tipo de comando, y usamos un switch para procesar cada tipo.

3. parser.h
    Este archivo fue proporcionado por la cátedra. Está encargado de leer y construir los pipelines de comandos.

4. execute.h y execute.c
    Este módulo es el encargado de la ejecución de comandos. Aquí encontramos la mayor dificultad al realizarlo. 
    La lógica que utilizamos es la siguiente:

    Si el pipeline no está vacío:

        Obtengo el primer comando.
        Si es interno, lo ejecuto con builtin_run().
        Si es externo:
            Obtengo el número de pipes (|).
            Creo los pipes que van a comunicar a los distintos procesos (cada comando es un proceso).
            Uso arrays de la forma fd[i][j], donde i identifica el comando (si es el primero, el segundo, el último, etc.), y j es 0 o 1 (0 para lectura y 1 para escritura).

    Para ejecutar los comandos del pipeline hago un for:

        Hago un fork (cada comando es un proceso distinto).
        En el hijo:
            Redirecciono la entrada y salida estándar de ser necesario.
            Redirecciono en caso de que exista < y/o >.
            Cierro los pipes.
            Ejecuto con execvp.
        En el padre:
            Cierro el pipe de lectura anterior (si i != 0).
            Cierro el pipe de escritura actual (si el comando no es el último).
    Finalmente, verifico si hay que esperar a que termine el proceso o no.

5. mybash.c
    En este archivo definimos la función main, inicializamos el shell y pasamos al parser para su ejecución. 
    Aquí se controla el ciclo de vida del shell, ejecutando comandos hasta que el usuario decida salir. 
    Al final, tuvimos dificultades al probar comandos cuando presionábamos ENTER, pero lo solucionamos. 
    También está comentada la función ping_pong_loop(NULL) ya que relentizaba toda la shell.

MANEJO DE ERRORES:
    Tenemos un manejo de errores básico para garantizar que se puedan detectar y gestionar problemas comunes durante la ejecución de comandos. 
    Probamos la gestión de errores en la ejecución de comandos, redirección, creación de procesos, validación de procesos, argumentos y aserciones.

AUTORES:
    Facundo Gabriel Garione Chávez
    Malena Bustamante
    Santino Bianco Monasterio
    Luis Bautista Córdoba Buffa