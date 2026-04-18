/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : battleship.c
  * @brief          : Main program body
  ******************************************************************************
  *
  *
  * Copyright (c) 2023 Boise State University
  * All rights reserved.
  *
  * This file provides a library for outputting characters to the 7 segment
  * displays on the STM32F4 Discovery Peripheral Board for ECE330L lab.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "battleship.h"


enum board {
	top = ~1,
	middle = ~(1<<6),
	bottom = ~(1<<3),
	top_right = ~(1<<1),
	top_left = ~(1<<5),
	bottom_right = ~(1<<2),
	bottom_left = ~(1<<4),
	dot = ~(1<<7)
};

enum gamestate {
	start,
	playerplace1,
	playerplace2,
	playerturn1,
	playerturn2,
	endresult
};

enum gameplay {
	hit,
	miss,
	still,
	sunk,
	cursor
};

struct map_s {
	unsigned char horiz[3][8];
	unsigned char vert [2][16];
};

struct player_map {

};

struct gamelogic {


};


static void cursor_map() {

}


static void composite () {

}


extern void draw_board (void) {
	/*******************************************************************************
	Code to mask and bit shift 0-7 value of digit and 0-15 value of hex_char
	to output correct bit pattern to GPIO_Output
	*******************************************************************************/
	//turn it all off //bottom FF go high, latch in by setting upper FF to high on second line
	//0xFFFF turns them all off, so i dont know why in the init they are all on
	GPIOE->ODR = 0xFF;
	GPIOE->ODR |= 0xFF00;
	//select specific
	//we are shifting a 0 value essentially
	GPIOE->ODR = (0xFF00 | ~(1<<7)) & ~(1<<(8));
	GPIOE->ODR |= 0xFF00;
	//select specific
	//did multiple lines, to play with it
	//we are deffinetly just overwriting
	//could prolly or in with prev value
	//the AND portion should be the display select
	//i think it should maybe be ~(1<<(i+8))
	GPIOE->ODR = (0xFF00 | ~(1<<5)) & ~(1<<(8));
	GPIOE->ODR |= 0xFF00;
	//select specific
	GPIOE->ODR = (0xFF00 | ~(1<<2)) & ~(1<<(4));
	GPIOE->ODR |= 0xFF00;

		return;

}

extern void poll_input (void) {

}

extern void gametime (void) {

}

extern void gameresult (void) {

}


