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

void draw_board (void);
void poll_input (void);
void gametime (void);
void gameresult (void);



#endif /* __BATTLESHIP_H */
