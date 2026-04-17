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
	top_left = (1<<1),
	top = (1<<2),
	top_right = (1<<3),
	bottom_right = (1<<4),
	bottom_left = (1<<5),
	bottom = (1<<6),
	dot = (1<<7)
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


extern void draw_board (int digit,int digit2) {
	/*******************************************************************************
	Code to mask and bit shift 0-7 value of digit and 0-15 value of hex_char
	to output correct bit pattern to GPIO_Output
	*******************************************************************************/
		// Set selected digit to 0, all others high, and output 7 segment pattern
		//GPIOE->ODR = (0xFF00 | ~(digit)) & ~(1<<(7-digit2)+8);
		//GPIOE->ODR = (0xFF00 | 1) & ~(1<<(8));



		// Set all selects high to latch-in character

		GPIOE->ODR |= 0xFF00;
		//GPIOE->ODR = 1<<6;
		//bottomleft GPIOE->ODR = 1<<4;
		//topleft GPIOE->ODR = 1<<5;
		//middle GPIOE->ODR = 1<<6;
		//GPIOE->ODR = 1<<7;
		return;

}

extern void poll_input (void) {

}

extern void gametime (void) {

}

extern void gameresult (void) {

}


