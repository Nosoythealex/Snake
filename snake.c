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
#include <stdlib.h> //rand()

// Definimos las medidas exactas de nuestro pantalla
#define WIDTH 34 // BOARD_WIDTH - 1 para el indice base 0
#define HEIGHT 24 // BOARD_HEIGHT -1

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

// Array para Posicion para el cuerpo y coordenadas
int snakex[100];
int snakey[100];
int snake_length =1;

// Posicion de la manzana
int apple_x = 0;
int apple_y = 0;

// Puntero para cabeza de la serpiente y la manzana
volatile unsigned int *snakeHead = 0;
volatile unsigned int *apple =0;


// Variables globales
int ingame = 0;
int direction = 1; // 1=UP, 2=DOWN(malito), 3=LEFT 4=RIGHT
int score = 1;
int head_x = 0;    
int head_y = 0;
int debug_mode = 1;
int speed = 600; // Velocidad: (valor más bajo = más rápido)


// Funciones de Snake:
void clearMatrix();
void snake();
void appleGen();
void dpads(int up, int down, int left, int right);
void abso(int value);
void comerApple();
int choque();
void updateMatrix();
int delApple();
void moverSnake();


void main()
{
    printf(" $$$$$$\\  $$\\   $$\\  $$$$$$\\  $$\\   $$\\ $$$$$$$$\\ \n");
    printf("$$  __$$\\ $$$\\  $$ |$$  __$$\\ $$ | $$  |$$  _____|\n");
    printf("$$ /  \\__|$$$$\\ $$ |$$ /  $$ |$$ |$$  / $$ |      \n");
    printf("\\$$$$$$\\  $$ $$\\$$ |$$$$$$$$ |$$$$$  /  $$$$$\\    \n");
    printf(" \\____$$\\ $$ \\$$$$ |$$  __$$ |$$  $$<   $$  __|   \n");
    printf("$$\\   $$ |$$ |\\$$$ |$$ |  $$ |$$ |\\$$\\  $$ |      \n");
    printf("\\$$$$$$  |$$ | \\$$ |$$ |  $$ |$$ | \\$$\\ $$$$$$$$\\ \n");
    printf(" \\______/ \\__|  \\__|\\__|  \\__|\\__|  \\__|\\________|\n");

    printf("\n Instrucciones: \n");
    printf("1. Presiona Switch 0 para iniciar.\n");
    printf("2. SNAKE es de color verde, APPLE de color rojo.\n");
    printf("3. Para reiniciar el juego, apaga y prende el SWITCH 0.\n");
    printf("\n Instrucciones: \n")
    printf("1.Presiona Switch 0 para iniciar.")
    printf("2.SNAKE es de color verde, APPLE de color rojo.")
    printf("3.Para reiniciar el juego, apaga y prende el SWITCH 0.\n")

    
    // Inicializar la matriz para que todos los LED's esten apagados
    for (int y = 0; y < led_matrix_height; y++) {
        for (int x = 0; x < led_matrix_width; x++) {
            led_state[y][x] = 0x000000;
        }
    }

    clearMatrix();


    
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

void snake()
{
    printf("Creando la serpiente...");

    //La ponemos en la posicion inicial
    head_x = WIDTH / 2;
    head_y = HEIGHT - 2;

    //Almacenamos los valores en nuestro array
    snake_bx[0] = head_x;
    snake_by[0] = head_y;

    //Solo para ver donde esta el error:
    printf("Serpiente creada en (%d, %d)\n", head_x, head_y);

}

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

// Para sacar el valor absoluto y no tener negativos
int abso(int value){
    return value >= 0 ? value : -value;
}

//Checar la direccion en base a lo D-PADS
void dpads(int up, int down, int left, int right)
{
    // Flag: Verificar que los valores sean 0 o 1
    up = up ? 1 : 0;
    down = down ? 1 : 0;
    left = left ? 1 : 0;
    right = right ? 1 : 0;

    // Cambio de direccion solo si es valido
    if (up && direction != 2) { // Si va hacia arriba no puede ir hacia abajo
        direction 1;
        if (debug_mode) printf("Direccion: UP\n");
    }
    else if(down && direction != 1) { // no puede ir hacia arriba
        direction 2;
        if (debug_mode) printf("Direccion: DOWN");
    }else if (left && direction != 4) {
        direction 3;
        if (debug_mode) printf("Direccion: LEFT");
    }else if (right && direction!= 3) {
        direction 4;
        if (debug_mode) printf("Direccion: RIGHT";)
    }
}

void appleGen()
{
    printf("Generando manzana...\n");
    
    int x, y;
    int attempts = 0;
    int valid_position = 0;
    
    while (!valid_position && attempts < 100) {
        attempts++;
        
        // generar numeros aleatorios
        x = (rand() % ((WIDTH - 4) / 2)) * 2;
        y = (rand() % ((HEIGHT - 4) / 2)) * 2;
        
        // ajustamos para que quede dentro de los bordes
        if (x < 2) x = 2;
        if (y < 2) y = 2;
        if (x > WIDTH - 4) x = WIDTH - 4;
        if (y > HEIGHT - 4) y = HEIGHT - 4;
        
        // Verificar que no esté en el cuerpo de la serpiente
        valid_position = 1;
        for (int i = 0; i < snake_length; i++) {
            // la distancia entre ambos
            int dist_x = abso(x - snakex[i]);
            int dist_y = abso(y - snakey[i]);
            
            // Si la distancia es menor a 2,chocan
            if (dist_x < 2 && dist_y < 2) {
                valid_position = 0;
                break;
            }
        }
    }
    
    // Si no es valida, ponerla en una posicion fija.
    if (!valid_position) {
        // La posicion contraria a la de la serpiente 
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
    printf("Manzana: (%d, %d)\n", x, y);
}

// Funcion para que la serpiente pueda comer(eliminar la manzana)
void comerApple()
{
    printf("¡Manzana comida! Puntuación: %d → %d\n", score, score + 1);
    
    // Aumentar la puntuación y la longitud
    score++;
    
    // Añadir un nuevo segmento al final de la serpiente
    // El nuevo segmento heredará la posición del último segmento
    int last_idx = snake_length - 1;
    int new_idx = snake_length;
    
    snakex[new_idx] = snakex[last_idx];
    snakey[new_idx] = snakey[last_idx];
    
    // Crecer el largo de la serpiente
    snake_length++;
    
    printf("Snake Lenght: %d\n", snake_length);
}

// Checar que no choque la serpiente con su cuerpo
int choque()
{
    // Verificar con la matrix de leds
    if (head_x < 0 || head_x >= WIDTH || 
        head_y < 0 || head_y >= HEIGHT) {
        printf("¡Chocaste contra el borde! Game Over\n");
        return 1;
    }
    
    // Verificar que cuando se choque con la misma (a partir del tercer 2x2)
    for (int i = 3; i < snake_length; i++) {
        if (head_x == snakex[i] && head_y == snakey[i]) {
            printf("¡Chocaste contigo mismo! Game Over\n");
            return 1;
        }
    }
    
    return 0;
}

// Mover la serpiente con ayuda de los DPADS
void moverSnake(){
    // guardamos la posicion anterior para el movimiento suave
    int old_x[100], old_y[100];
    for (int i = 0; i < snake_length; i++) {
        old_x[i] = snakex[i];
        old_y[i] = snakey[i];
    }
    
    // Movimiento del cuerpo a la posicion
    for (int i = snake_length - 1; i > 0; i--) {
        snakex[i] = old_x[i-1];
        snakey[i] = old_y[i-1];
    }
    
    // Mover la cabeza en la dirección actual
    // Movimiento en bloques de 2x2
    switch (direction) {
        case 1: // Arriba
            head_y -= 2; 
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
    
    // Actualizar la posición de la cabeza en el array original
    snakex[0] = head_x;
    snakey[0] = head_y;
    
    // Actualizar el puntero de la cabeza
    snakeHead = baseLed + (matrix_wid * head_y) + head_x;

}

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
    // Si las distancias en X y Y son ambas menores o iguales a 1, se come la manzana
    if (distancia_x <= 1 && distancia_y <= 1) {
        printf("Manzana comida: (%d,%d)!\n", apple_x, apple_y); //SOLO PARA CHECAR QUE SE LA COMA
        return 1;
    }
    // Para cuando se acerce a menos de un led
    if (abso(head_x - apple_x) <= 1 && abso(head_y - apple_y) <= 1) {
        printf("Manzana comida cerca de: (%d,%d)!\n", apple_x, apple_y);
        return 1;
    }
    
    return 0;
}

// Actualizar la Matriz y que se vea el movimiento
void updateMatrix()
{
    // Creamos una matrix temporal donde guardaremos los cambios
    unsigned int new_led_state[matrix_hei][matrix_wid];
    
    // Inicializar la matriz temporal con todos los LEDs apagados
    for (int y = 0; y < matrix_hei; y++) {
        for (int x = 0; x < matrix_wid; x++) {
            new_led_state[y][x] = 0x000000; // Color negro (apagado)
        }
    }
    
    // Dibujar la serpiente en la matriz temporal
    for (int i = 0; i < snake_length; i++) {
        // Para cada segmento, dibujamos un bloque 2x2
        int x = snakex[i];
        int y = snakey[i];
        
        // Deben de ser numeros pares
        x = (x / 2) * 2;
        y = (y / 2) * 2;
        
        // Verificar que las coordenadas estén dentro de los límites
        if (x >= 0 && x < matrix_wid - 1 && y >= 0 && y < matrix_hei - 1) {
            if (i == 0) {
                // La cabeza es de color verde mas suave
                new_led_state[y][x] = COLOR_SNAKE_HEAD;
                new_led_state[y][x+1] = COLOR_SNAKE_HEAD;
                new_led_state[y+1][x] = COLOR_SNAKE_HEAD;
                new_led_state[y+1][x+1] = COLOR_SNAKE_HEAD;
            } else {
                // El cuerpo es de color verde mas oscuro
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
        
        // Checar que sean numero pares
        int x = (apple_x / 2) * 2;
        int y = (apple_y / 2) * 2;
        
        if (x >= 0 && x < matrix_wid - 1 && y >= 0 && y < matrix_hei - 1) {
            new_led_state[y][x] = COLOR_APPLE; // Rojo 
            new_led_state[y][x+1] = COLOR_APPLE;
            new_led_state[y+1][x] = COLOR_APPLE;
            new_led_state[y+1][x+1] = COLOR_APPLE;
        }
        
        // Actualizar el puntero de la manzana
        apple = baseLed + (matrix_wid * y) + x;
    }
    
    // Actualizar SOLO los LEDs que han cambiado en la matriz real
    volatile unsigned int *led_ptr;
    for (int y = 0; y < matrix_hei; y++) {
        for (int x = 0; x < matrix_wid; x++) {
            // Solo actualizar si el estado ha cambiado
            if (led_state[y][x] != new_led_state[y][x]) {
                led_ptr = baseLed + (matrix_wid * y) + x;
                
                // Verificar que el puntero sea valido para actualizar.
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



