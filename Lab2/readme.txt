Laboratorio 2: Semáforos en XV6


DESCRIPCION GENERAL:
    En el entorno de RISC-V de XV6 tuvimos que implementar un sistema de Semáforos dentro del kernel de XV6
    y un programa de usuario llamado pingpong, que utiliza dichos semáforos para sincronizar la ejecución de 
    dos procesos. 

ESTRUCTURA DEL PROYECTO QUE IDEAMOS:
    semaforo.c:
    
    Para la implementación de los semáforos, decidimos crear un archivo separado (semaphore.c) en lugar de modificar
    directamente archivos ya existentes en el kernel
    por varias razones:

    Modularidad: Al crear un archivo independiente, mantenemos el código relacionado con los semáforos separado
        del resto del sistema. Esto hace que el código sea más fácil de entender, mantener y extender.

    Simplicidad: La implementación de los semáforos requería una estructura de datos específica y operaciones
        que gestionan el estado y el bloqueo de los procesos. Separar esta lógica en su propio archivo evita que
        otros componentes del kernel se vean afectados por esta funcionalidad adicional.

    Escalabilidad: Si en el futuro necesitamos implementar otro tipo de semáforos o ampliar la funcionalidad,
        tener un archivo dedicado facilita la extensión sin afectar otros subsistemas del kernel.

// Resumen de la estructura de semáforos
        struct semaphore {
            struct spinlock lock;
            int count;  // Valor del semáforo
            int open;   // Bandera para saber si está abierto o cerrado
        };

        // Funciones implementadas:
        // sem_open() - Inicializa un semáforo
        // sem_down() - Decrementa el semáforo (bloquea si es 0)
        // sem_up() - Incrementa el semáforo y desbloquea procesos
        // sem_close() - Cierra el semáforo


    pingpong.c
        El programa pingpong fue desarrollado en el espacio de usuario y tiene como objetivo mostrar la correcta sincronización
        entre dos procesos utilizando semáforos. La idea es que un proceso imprima "ping" y el otro "pong", alternadamente.

    Uso de Semáforos: 
        Para lograr la alternancia, usamos dos semáforos: uno inicializado en 0 (sem_ping) y otro en 1 (sem_pong). 
        Esto garantiza que el proceso padre (imprime "ping") comience primero. El proceso hijo (imprime "pong") solo puede 
        ejecutarse cuando el semáforo sem_ping le da permiso.

    Ciclo de ejecución: 
    Cada proceso realiza un sem_down en su semáforo antes de imprimir su mensaje y un sem_up en el semáforo del otro proceso, 
    permitiendo así la correcta sincronización.

// Resumen del programa pingpong
        void pingpong(int rally) {
            // Inicializamos dos semáforos: uno para "ping" y otro para "pong"
            sem_open(0, 0);  // Inicializa semáforo de "pong" en 0
            sem_open(1, 1);  // Inicializa semáforo de "ping" en 1

            int pid = fork();  // Crea el proceso hijo

            for (int i = 0; i < rally; i++) {
                if (pid == 0) {
                    // Proceso hijo imprime "pong"
                    sem_down(0);
                    printf("pong\n");
                    sem_up(1);
                } else {
                    // Proceso padre imprime "ping"
                    sem_down(1);
                    printf("ping\n");
                    sem_up(0);
                }
            }

            // Cerramos los semáforos
            sem_close(0);
            sem_close(1);
        }

MANEJO DE ERRORES:
    Durante el desarrollo, prestamos especial atención a la gestión de errores para garantizar que tanto los semáforos como el programa pingpong
    funcionen de manera robusta y predecible. 
    El manejo de errores incluye:

    Verificación de parámetros y estados en las syscalls de semáforos: 
    Validamos los identificadores de semáforos, aseguramos que los semáforos
    estén abiertos antes de realizar operaciones y controlamos que los procesos no intenten cerrar semáforos en uso.

    Manejo adecuado de errores en el programa pingpong: 
    Nos aseguramos de que el programa pingpong maneje correctamente los errores de creación de procesos 
    (fork()), apertura de semáforos y validación de argumentos, imprimiendo mensajes descriptivos y cerrando recursos abiertos antes de salir.

AUTORES:
    Facundo Gabriel Garione Chávez
    Malena Bustamante
    Santino Bianco Monasterio
    Luis Bautista Córdoba Buffa