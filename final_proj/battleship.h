/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : battleship.h
  * @brief          : Header for battleship.c library file.
  *                   This file contains the common defines of the library.
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
#ifndef __BATTLESHIP_H
#define __BATTLESHIP_H

#include "stm32f4xx_hal.h"

/* --- Game State Logic --- */
typedef enum {
    MSG_PLAYER1,
    PLACE_PLAYER1,
    MSG_PLAYER2,
    PLACE_PLAYER2,
    BATTLE_PHASE,
    GAME_OVER
} GameState;

/* --- Main Loop Interface --- */
void poll_input(void);  // Reads ADCs and Fire Button
void gametime(void);    // Handles State Machine transitions
void draw_board(void);  // Renders maps + cursor to display_buffer
void gameresult(void);  // Handles win/loss screens

/* --- Setup & Helpers --- */
void Battleship_Init(void);
void Battleship_StartScreen(void);
void scroll_message(char* str);
uint8_t convert_ascii_to_7seg(char c);
void place_ship(uint32_t *values);

/* --- Shared Global Buffers --- */
// These allow main.c to see the buffers if needed,
// while they are managed by battleship.c
extern uint8_t display_buffer[8];
extern uint8_t p1_ships[8];
extern uint8_t p2_ships[8];
extern GameState current_state;

#endif /* __BATTLESHIP_H */
