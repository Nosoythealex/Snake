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

// Definimos las medidas exactas de nuestro pantalla
#define BOARD_WIDTH 34 // BOARD_WIDTH - 1 para el indice base 0
#define BOARD_HEIGHT 24 // BOARD_HEIGHT -1

#define led_matrix_wid 35 // Ancho exacto establecido en Ripes
#define led_matrix_hei 25 // Alto exacto establecido en Ripes

#define COLOR_SNAKE_HEAD 0x00FF00 // Color para la cabeza
#define COLOR_SNAKE_BODY 0x00C000 // Color para el cuerpo
#define COLOR_APPLE 0xFF0000 // Color a la manzana.

// Almacenamos la direcciones de Switches, D_Pads y Led's
volatile unsigned int *switch0 = (volatile unsigned int *)SWITCHES_0_BASE; 
// LEDS
volatile unsigned int *led_base = (volatile unsigned int *)LED_MATRIX_0_BASE; // Cast a entero
// D_Pads
volatile unsigned int *d_padUp = (volatile unsigned int *)D_PAD_0_UP;
volatile unsigned int *d_padDo = (volatile unsigned int *)D_PAD_0_DOWN;
volatile unsigned int *d_padLe = (volatile unsigned int *)D_PAD_0_LEFT;
volatile unsigned int *d_padRi = (volatile unsigned int *)D_PAD_0_RIGHT;

// Registro para elestado de los LED's
unsigned int led_state[25][35];


// Array para Posicion para el cuerpo y coordenadas
int snake_bx[100];
int snake_by[100];
int snake_length =1;

// Posicion de la manzana
int apple_x = 0;
int apple_y = 0;

// Puntero para cabeza de la serpiente y la manzana
volatile unsigned int *snakeHead = 0;
volatile unsigned int *apple =0;


// Variables globales
unsigned int next_random = 1;
int in_game = 0;
int direction = 1; // 1=arriba, 2=abajo, 3=izquierda, 4=derecha
int score = 1;
int head_x = 0;    
int head_y = 0;
int debug_mode = 1;
int game_speed = 600; // Velocidad aumentada (valor más bajo = más rápido)

void main()
{
    printf("   ▄████████ ███▄▄▄▄      ▄████████    ▄█   ▄█▄    ▄████████ \n");
    printf("  ███    ███ ███▀▀▀██▄   ███    ███   ███ ▄███▀   ███    ███ \n");
    printf("  ███    █▀  ███   ███   ███    ███   ███▐██▀     ███    █▀  \n");
    printf("  ███        ███   ███   ███    ███  ▄█████▀     ▄███▄▄▄     \n");
    printf("▀███████████ ███   ███ ▀███████████ ▀▀█████▄    ▀▀███▀▀▀     \n");
    printf("         ███ ███   ███   ███    ███   ███▐██▄     ███    █▄  \n");
    printf("   ▄█    ███ ███   ███   ███    ███   ███ ▀███▄   ███    ███ \n");
    printf(" ▄████████▀   ▀█   █▀    ███    █▀    ███   ▀█▀   ██████████ \n");
    printf("                                      ▀                      \n");
    printf("\n Instrucciones: \n")
    printf("1.Presiona Switch 0 para iniciar.")
    printf("2.SNAKE es de color verde, APPLE de color rojo.")
    printf("3.Para reiniciar el juego, apaga y prende el SWITCH 0.\n")
}