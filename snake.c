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

// Almacenamos la direcciones de Switches, D_Pads y Led's
volatile unsigned int *switch0 = (volatile unsigned int *)SWITCHES_0_BASE; 
// almacenamos la direccion del inicio de la matriz
volatile unsigned int *led_base = (volatile unsigned int *)LED_MATRIX_0_BASE; // Cast a entero
// almacenamos los D_Pads
volatile unsigned int *d_padUp = (volatile unsigned int *)D_PAD_0_UP;
volatile unsigned int *d_padDo = (volatile unsigned int *)D_PAD_0_DOWN;
volatile unsigned int *d_padLe = (volatile unsigned int *)D_PAD_0_LEFT;
volatile unsigned int *d_padRi = (volatile unsigned int *)D_PAD_0_RIGHT;

