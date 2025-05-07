/*
##############################################
#                                            #
#     Alberto Alejandro Mercado Gómez        #
#      Isaac Alejandro Pérez Novoa           #
#                                            #
#   Materia: Organización y Arquitectura     #
#              de Computadoras               #
#                                            #
##############################################
*/

#include "ripes_system.h"
#include <stdio.h>
#include <stdlib.h> // Añadida para rand()

// Definimos las medidas exactas de nuestro pantalla
#define WIDTH 34 // WIDTH - 1 para el indice base 0
#define HEIGHT 24 // HEIGHT -1

#define matrix_wid 35 // Ancho exacto establecido en Ripes
#define matrix_hei 25 // Alto exacto establecido en Ripes

#define COLOR_SNAKE_HEAD 0x00FF00 // Color para la cabeza
#define COLOR_SNAKE_BODY 0x00C000 // Color para el cuerpo
#define COLOR_APPLE 0xFF0000 // Color a la manzana.

// Almacenamos la direcciones de Switches, D_Pads y Led's
volatile unsigned int *switch0 = (volatile unsigned int *)SWITCHES_0_BASE; 
// LEDS
volatile unsigned int *baseLed = (volatile unsigned int *)LED_MATRIX_0_BASE; // Cast a entero
// D_Pads
volatile unsigned int *dpadUp = (volatile unsigned int *)D_PAD_0_UP;
volatile unsigned int *dpadDo = (volatile unsigned int *)D_PAD_0_DOWN;
volatile unsigned int *dpadLe = (volatile unsigned int *)D_PAD_0_LEFT;
volatile unsigned int *dpadRi = (volatile unsigned int *)D_PAD_0_RIGHT;

// Registro para el estado de los LED's
unsigned int led_state[25][35];

// Array para Posiciones para el cuerpo
int snakex[100];
int snakey[100];
int snake_length = 1;

// Posicion de la manzana
int apple_x = 0;
int apple_y = 0;

// Puntero para cabeza de la serpiente y la manzana
volatile unsigned int *snakeHead = 0;
volatile unsigned int *apple = 0;

// Variables globales
unsigned int numRandom = 1;
int ingame = 0;
int direction = 1; // 1=arriba, 2=abajo, 3=izquierda, 4=derecha
int score = 1; 
int head_x = 0;    
int head_y = 0;
int debug_mode = 1;
int speed = 600; // Velocidad aumentada (valor más bajo = más rápido)

// Funciones de Snake:
void clearMatrix();
void snake();
void appleGen();
void dpads(int up, int down, int left, int right);
int abso(int value);
void comerApple();
int choque();
void updateMatrix();
int delApple();
void moverSnake();

void main()
{
    printf(" $$$$$$\\  $$\\   $$\\  $$$$$$\\  $$\\   $$\\ $$$$$$$$\\ \n");
    printf("$$  _$$\\ $$$\\  $$ |$$  _$$\\ $$ | $$  |$$  ___|\n");
    printf("$$ /  \\|$$$$\\ $$ |$$ /  $$ |$$ |$$  / $$ |      \n");
    printf("\\$$$$$$\\  $$ $$\\$$ |$$$$$$$$ |$$$$$  /  $$$$$\\    \n");
    printf(" \\_$$\\ $$ \\$$$$ |$$  _$$ |$$  $$<   $$  __|   \n");
    printf("$$\\   $$ |$$ |\\$$$ |$$ |  $$ |$$ |\\$$\\  $$ |      \n");
    printf("\\$$$$$$  |$$ | \\$$ |$$ |  $$ |$$ | \\$$\\ $$$$$$$$\\ \n");
    printf(" \\/ \\|  \\|\\|  \\|\\|  \\|\\|\n");
    
    printf("\n Instrucciones: \n");
    printf("1. Presiona Switch 0 para iniciar.\n");
    printf("2. SNAKE es de color verde, APPLE de color rojo.\n");
    printf("3. Para reiniciar el juego, apaga y prende el SWITCH 0.\n");
    
    // Inicializar el generador de números aleatorios
    srand(numRandom); // Usar nuestra variable global como semilla

    for (int y = 0; y < matrix_hei; y++) {
        for (int x = 0; x < matrix_wid; x++) {
            led_state[y][x] = 0x000000;
        }
    }

    clearMatrix();
    
    while (1) {
        // Espera a que se active el Switch 0 para iniciar
        printf("Esperando a que presiones Switch 0...\n");
        while (!(*switch0)) {
            // Solo espera
        }
        printf("¡Switch 0 activado! Iniciando juego...\n");
        
        // Reiniciar variables del juego
        score = 1;
        snake_length = 1;
        head_x = WIDTH / 2;
        head_y = HEIGHT - 2;
        direction = 1; // Comenzar moviéndose hacia arriba
        ingame = 1;
        
        // Inicializar juego
        clearMatrix();
        snake();
        appleGen();
        updateMatrix(); // Asegurar que se muestre la serpiente y la manzana inicial
        
        printf("¡Juego iniciado! Controla con los D-Pads\n");
        printf("Serpiente: VERDE | Manzana: ROJA\n");
        
        // Bucle principal del juego
        while (ingame) {
            // Verificar entradas de los D-Pads para cambiar dirección
            // Hacemos esto varias veces para una respuesta más rápida
            for (int i = 0; i < 5; i++) {
                dpads(*dpadUp, *dpadDo, *dpadLe, *dpadRi);
                
                // Pequeña pausa entre lecturas
                for (volatile int j = 0; j < speed / 15; j++) {
                    // Solo espera
                }
            }
            
            // Mover la serpiente en la dirección actual
            moverSnake();
            
            // Verificar si comió una manzana
            if (delApple()) {
                comerApple();
                appleGen(); // Generar nueva manzana inmediatamente
            }
            
            // Verificar colisiones
            if (choque()) {
                ingame = 0;
                continue;
            }
            
            // Actualizar la visualización
            updateMatrix();
            
            // Información del juego (reducida para menos spam en consola)
            if (debug_mode) {
                printf("Cabeza: (%d, %d) | Puntuación: %d | Longitud: %d\n", 
                      head_x, head_y, score, snake_length);
            }
            
            // Pausa más corta para controlar la velocidad
            // La lectura frecuente de D-Pads ya creó algo de retraso
            for (volatile int i = 0; i < speed / 2; i++) {
                // Solo espera
            }
        }
        
        printf(" $$$$$$\\   $$$$$$\\  $$\\      $$\\ $$$$$$$$\\        $$$$$$\\  $$\\    $$\\ $$$$$$$$\\ $$$$$$$\\  \n");
        printf("$$  _$$\\ $$  _$$\\ $$$\\    $$$ |$$  __|      $$  _$$\\ $$ |   $$ |$$  __|$$  _$$\\ \n");
        printf("$$ /  \\|$$ /  $$ |$$$$\\  $$$$ |$$ |            $$ /  $$ |$$ |   $$ |$$ |      $$ |  $$ |\n");
        printf("$$ |$$$$\\ $$$$$$$$ |$$\\$$\\$$ $$ |$$$$$\\          $$ |  $$ |\\$$\\  $$  |$$$$$\\    $$$$$$$  |\n");
        printf("$$ |\\$$ |$$  _$$ |$$ \\$$$  $$ |$$  _|         $$ |  $$ | \\$$\\$$  / $$  _|   $$  __$$< \n");
        printf("$$ |  $$ |$$ |  $$ |$$ |\\$  /$$ |$$ |            $$ |  $$ |  \\$$$  /  $$ |      $$ |  $$ |\n");
        printf("\\$$$$$$  |$$ |  $$ |$$ | \\_/ $$ |$$$$$$$$\\        $$$$$$  |   \\$  /   $$$$$$$$\\ $$ |  $$ |\n");
        printf(" \\/ \\|  \\|\\|     \\|\\|       \\/     \\/    \\|\\|  \\_|\n");

        printf("Puntuacion Final: %d\n", score);
        printf("Presiona Switch 0 para reiniciar\n\n");
        
        
        // Esperar a que se desactive el Switch 0
        while (*switch0) {
            // Solo espera
        }
    }
}

// Limpiar toda la matriz LED
void clearMatrix()
{
    volatile unsigned int *led_ptr;
    printf("Limpiando matriz LED...\n");
    
    for (int y = 0; y < matrix_hei; y++) {
        for (int x = 0; x < matrix_wid; x++) {
            led_ptr = baseLed + (matrix_wid * y) + x;
            *led_ptr = 0x000000; // Apagar LED
            
            // Actualizar también nuestro registro de estado
            led_state[y][x] = 0x000000;
        }
    }
    
    printf("Matriz LED limpiada\n");
}

// Inicializar la serpiente en la posición inicial
void snake()
{
    printf("Inicializando serpiente...\n");
    
    // Establecer posición inicial
    head_x = WIDTH / 2;
    head_y = HEIGHT - 2;
    
    // Almacenar la posición inicial en el array del cuerpo
    snakex[0] = head_x;
    snakey[0] = head_y;
    
    printf("Serpiente inicializada en (%d, %d)\n", head_x, head_y);
}


// Ahora usamos la función rand() de la biblioteca stdlib.h
/*
//Funcion en base XORShift para numeros aleatorios
int random() 
{
    numRandom ^= numRandom << 13; // recorremos 13 bits a la izquierda
    numRandom ^= numRandom >> 17; // ahora 17 a la derecha
    numRandom ^= numRandom << 5; // ultimo, 5 a la izquiera

    return numRandom % 32768;
}
*/

// Función auxiliar para calcular valor absoluto
int abso(int value) {
    return value >= 0 ? value : -value;
}

// Verificar la dirección basada en los D-Pads
void dpads(int up, int down, int left, int right)
{
    // Asegurarse de que los valores sean 0 o 1, en caso de que tengan otros valores
    up = up ? 1 : 0;
    down = down ? 1 : 0;
    left = left ? 1 : 0;
    right = right ? 1 : 0;
    
    // Solo cambiar si es una dirección válida (no puede ir en dirección opuesta)
    if (up && direction != 2) { // No puede ir hacia arriba si va hacia abajo
        direction = 1;
        if (debug_mode) printf("Direccion: UP\n");
    }
    else if (down && direction != 1) { // No puede ir hacia abajo si va hacia arriba
        direction = 2;
        if (debug_mode) printf("Direccion: DOWN\n");
    }
    else if (left && direction != 4) { // No puede ir hacia izquierda si va hacia derecha
        direction = 3;
        if (debug_mode) printf("Direccion: LEFT\n");
    }
    else if (right && direction != 3) { // No puede ir hacia derecha si va hacia izquierda
        direction = 4;
        if (debug_mode) printf("Direccion: RIGHT\n");
    }
}

// Mover la serpiente en la dirección actual
void moverSnake()
{
    // Guardar la posición anterior de todos los segmentos para transición suave
    int old_x[100], old_y[100];
    for (int i = 0; i < snake_length; i++) {
        old_x[i] = snakex[i];
        old_y[i] = snakey[i];
    }
    
    // Mover todos los segmentos (excepto la cabeza) a la posición del segmento anterior
    for (int i = snake_length - 1; i > 0; i--) {
        snakex[i] = old_x[i-1];
        snakey[i] = old_y[i-1];
    }
    
    // Mover la cabeza en la dirección actual
    switch (direction) {
        case 1: // Arriba
            head_y -= 2; // Movimiento de para los dos leds del cuerpo.
            break;
        case 2: // Abajo
            head_y += 2; 
            break;
        case 3: // Izquierda
            head_x -= 2; 
            break;
        case 4: // Derecha
            head_x += 2; 
            break;
    }
    
    // Actualizar la posición de la cabeza en el array
    snakex[0] = head_x;
    snakey[0] = head_y;
    
    // Actualizar el puntero de la cabeza
    snakeHead = baseLed + (matrix_wid * head_y) + head_x;
}

// Actualizar la visualización del juego en la matriz LED
void updateMatrix()
{
    // En lugar de limpiar toda la matriz, crear una matriz temporal en memoria
    unsigned int new_led_state[matrix_hei][matrix_wid];
    
    // Inicializar la matriz temporal con todos los LEDs apagados
    for (int y = 0; y < matrix_hei; y++) {
        for (int x = 0; x < matrix_wid; x++) {
            new_led_state[y][x] = 0x000000; // Color negro (apagado)
        }
    }
    
    // Dibujar la serpiente en la matriz temporal
    for (int i = 0; i < snake_length; i++) {
        int x = snakex[i];
        int y = snakey[i];
        
        // Asegurarnos de que las coordenadas sean numeros validos para el cuerpo(2x2)
        x = (x / 2) * 2;
        y = (y / 2) * 2;
        
        // Verificar que las coordenadas estén dentro de los límites
        if (x >= 0 && x < matrix_wid - 1 && y >= 0 && y < matrix_hei - 1) {
            if (i == 0) {
                // La cabeza es de color verde brillante
                new_led_state[y][x] = COLOR_SNAKE_HEAD;
                new_led_state[y][x+1] = COLOR_SNAKE_HEAD;
                new_led_state[y+1][x] = COLOR_SNAKE_HEAD;
                new_led_state[y+1][x+1] = COLOR_SNAKE_HEAD;
            } else {
                // El cuerpo es de color verde menos brillante
                new_led_state[y][x] = COLOR_SNAKE_BODY;
                new_led_state[y][x+1] = COLOR_SNAKE_BODY;
                new_led_state[y+1][x] = COLOR_SNAKE_BODY;
                new_led_state[y+1][x+1] = COLOR_SNAKE_BODY;
            }
        }
    }
    
    // Dibujar la manzana en la matriz temporal
    if (apple_x >= 0 && apple_x < matrix_wid - 1 && 
        apple_y >= 0 && apple_y < matrix_hei - 1) {
        
        // Asegurarnos de que las coordenadas sean números pares para alineación 2x2
        int x = (apple_x / 2) * 2;
        int y = (apple_y / 2) * 2;
        
        if (x >= 0 && x < matrix_wid - 1 && y >= 0 && y < matrix_hei - 1) {
            new_led_state[y][x] = COLOR_APPLE; // Rojo brillante
            new_led_state[y][x+1] = COLOR_APPLE;
            new_led_state[y+1][x] = COLOR_APPLE;
            new_led_state[y+1][x+1] = COLOR_APPLE;
        }
        
        // Actualizar el puntero de la manzana
        apple = baseLed + (matrix_wid * y) + x;
    }
    
    // Se actualizan (NO CAMBIAR ._.) SOLO los LEDs que han cambiado en la matriz real
    volatile unsigned int *led_ptr;
    for (int y = 0; y < matrix_hei; y++) {
        for (int x = 0; x < matrix_wid; x++) {
            // Solo actualizar si el estado ha cambiado
            if (led_state[y][x] != new_led_state[y][x]) {
                led_ptr = baseLed + (matrix_wid * y) + x;
                
                // Verificar que el puntero sea válido
                if (led_ptr >= baseLed && 
                    led_ptr < baseLed + (matrix_wid * matrix_hei)) {
                    *led_ptr = new_led_state[y][x];
                    
                    // Actualizar el estado actual
                    led_state[y][x] = new_led_state[y][x];
                }
            }
        }
    }
}

// Verificar si la serpiente comió una manzana
int delApple()
{
    // Ajustar coordenadas para bloques 2x2
    int snake_head_x = (head_x / 2) * 2;
    int snake_head_y = (head_y / 2) * 2;
    int apple_block_x = (apple_x / 2) * 2;
    int apple_block_y = (apple_y / 2) * 2;
    
    // Calculamos la distancia entre los bloques
    int distancia_x = abso(snake_head_x - apple_block_x);
    int distancia_y = abso(snake_head_y - apple_block_y);
    
    // Si las distancias en X y Y son ambas menores o iguales a 1, hay superposición
    if (distancia_x <= 1 && distancia_y <= 1) {
        printf("Manzana comida en: (%d,%d)!\n", apple_x, apple_y);
        return 1;
    }
    
    // Método alternativo: verificar si la cabeza está cerca de la manzana
    if (abso(head_x - apple_x) <= 1 && abso(head_y - apple_y) <= 1) {
        printf("Manzana comida cerca de: (%d,%d)!\n", apple_x, apple_y);
        return 1;
    }
    
    return 0;
}

// Procesar cuando la serpiente come una manzana
void comerApple()
{
    printf("¡Manzana comida! Puntuacion %d → %d\n", score, score + 1);
    
    // Aumentar la puntuación y la longitud
    score++;
    
    // Añadir un nuevo segmento al final de la serpiente
    // El nuevo segmento heredará la posición del último segmento
    int last_idx = snake_length - 1;
    int new_idx = snake_length;
    
    snakex[new_idx] = snakex[last_idx];
    snakey[new_idx] = snakey[last_idx];
    
    // Incrementar la longitud después de añadir el segmento
    snake_length++;
    
    printf("Ahora mides: %d\n", snake_length);
}

// Generar una nueva manzana en una posición aleatoria
void appleGen()
{
    printf("Generando manzana...\n");
    
    int x, y;
    int attempts = 0;
    int valid_position = 0;
    
    while (!valid_position && attempts < 100) {
        attempts++;
        
        // Generar posición aleatoria (asegurando que sea par para alineación 2x2)
        x = (rand() % ((WIDTH - 4) / 2)) * 2;
        y = (rand() % ((HEIGHT - 4) / 2)) * 2;
        
        // Ajustar para estar dentro de los límites
        if (x < 2) x = 2;
        if (y < 2) y = 2;
        if (x > WIDTH - 4) x = WIDTH - 4;
        if (y > HEIGHT - 4) y = HEIGHT - 4;
        
        // Verificar que no esté en el cuerpo de la serpiente
        valid_position = 1;
        for (int i = 0; i < snake_length; i++) {
            int snake_x = (snakex[i] / 2) * 2; // Coordenada ajustada a par
            int snake_y = (snakey[i] / 2) * 2;
            
            // Comprobar si hay superposición con el bloque 2x2
            if ((x == snake_x && y == snake_y) || 
                (x == snake_x && y == snake_y + 1) ||
                (x == snake_x + 1 && y == snake_y) ||
                (x == snake_x + 1 && y == snake_y + 1)) {
                valid_position = 0;
                break;
            }
        }
    }
    
    // Si no se encuentra posición válida, usar una posición fija lejos de la serpiente
    if (!valid_position) {
        // Intentar colocar en el lado opuesto de la serpiente
        if (head_x < WIDTH / 2) {
            x = WIDTH - 4;
        } else {
            x = 4;
        }
        
        if (head_y < HEIGHT / 2) {
            y = HEIGHT - 4;
        } else {
            y = 4;
        }
    }
    
    // Guardar la posición de la manzana
    apple_x = x;
    apple_y = y;
    
    // Establecer la nueva manzana
    apple = baseLed + (matrix_wid * y) + x;
    printf("Manzana generada en: (%d, %d)\n", x, y);
}

// Verificar colisiones con los bordes o con el propio cuerpo
int choque()
{
    // Verificar colisión con los bordes
    if (head_x < 0 || head_x >= WIDTH || 
        head_y < 0 || head_y >= HEIGHT) {
        printf("Chocaste!!!!\n");
        return 1;
    }
    
    // Verificar colisión con el propio cuerpo (a partir del tercer segmento)
    // Se permite cierta superposición inicial para evitar colisiones falsas
    for (int i = 3; i < snake_length; i++) {
        if (head_x == snakex[i] && head_y == snakey[i]) {
            printf("Chocaste contigo mismo!!!!\n");
            return 1;
        }
    }
    
    return 0;
}