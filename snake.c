lea/*
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
    
    for (int y = 0; y < led_matrix_height; y++) {
        for (int x = 0; x < led_matrix_width; x++) {
            led_ptr = baseLed + (led_matrix_width * y) + x;
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
    head_x = BOARD_WIDTH / 2;
    head_y = BOARD_HEIGHT - 2;

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




