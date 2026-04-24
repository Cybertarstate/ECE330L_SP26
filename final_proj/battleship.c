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
#include "main.h"
#include "seg7.h"

extern uint32_t ADC_Values[3];
extern char _7SEG[];
extern unsigned char _7SEG_ASCII[];

uint8_t display_buffer[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
uint8_t p1_ships[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
uint8_t p2_ships[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
uint8_t blink_mask[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
uint8_t get_7seg_bits(int constant_val);

GameState current_state = MSG_PLAYER1;
int ship_count = 0;
const int MAX_SHIPS = 5;

enum board {
    top = ~0x01,
    middle = ~0x40,
    bottom = ~0x08,
    top_right = ~0x02,
    top_left = ~0x20,
    bottom_right = ~0x04,
    bottom_left = ~0x10,
    dot = ~0x80
};

/* --- Helper: Shared Cursor Math --- */
static void get_cursor(int *col, uint8_t *pattern) {
    if (ADC_Values[2] < 128) { // Horizontal Mode
        int steps = (ADC_Values[0] * 23) / 255;
        *col = 7 - (steps % 8);
        int row = steps / 8;
        *pattern = (row == 0) ? top : (row == 1) ? middle : bottom;
    } else { // Vertical Mode
        int v_steps = (ADC_Values[1] * 31) / 255;
        *col = 7 - ((v_steps % 16) / 2);
        int side = v_steps % 2;
        int row_set = v_steps / 16;
        if (row_set == 0) *pattern = (side == 0) ? top_left : top_right;
        else              *pattern = (side == 0) ? bottom_left : bottom_right;
    }
}

void set_board(void) {
    GPIOC->MODER &= ~(3 << (11 * 2));
    GPIOD->MODER = 0x55555555;
    GPIOE->MODER = 0x55555555;
    GPIOE->ODR   = 0xFFFF;

    GPIOA->MODER |= 0x000000FF;

    RCC->APB2ENR |= (1 << 8);          // Enable ADC1 Clock
    ADC1->CR2 |= 1;                    // ADC ON
    ADC1->CR1 |= (1 << 25);
    ADC1->CR1 |= (1 << 8);

    for(int i = 0; i < 8; i++) {
        p1_ships[i] = 0xFF;
        p2_ships[i] = 0xFF;
        display_buffer[i] = 0xFF;
        blink_mask[i] = 0xFF;
    }
}

void Battleship_StartScreen(void) {
    char Message[] = {
        SPACE, SPACE, SPACE, SPACE, SPACE, SPACE, SPACE, SPACE,
        CHAR_B, CHAR_A, CHAR_T, CHAR_T, CHAR_L, CHAR_E, CHAR_S, CHAR_H, CHAR_I, CHAR_P, CHAR_S,
        SPACE, SPACE, SPACE, SPACE, SPACE, SPACE, SPACE, SPACE
    };
    int total_len = sizeof(Message) / sizeof(Message[0]);
    for (int shift = 0; shift <= (total_len - 8); shift++) {
        for (int i = 0; i < 8; i++) {
            display_buffer[7 - i] = get_7seg_bits(Message[shift + i]);
            blink_mask[i] = 0xFF;
        }
        HAL_Delay(150);
    }
}

void poll_input(void) {
    for (int i = 0; i < 3; i++) {
        ADC1->SQR3 = (i + 1);
        ADC1->CR2 |= (1 << 30);
        while(!(ADC1->SR & (1 << 1)));
        ADC_Values[i] = ADC1->DR;
    }
}

uint8_t get_7seg_bits(int constant_val) {
    if (constant_val == SPACE) return 0xFF;
    return (uint8_t)_7SEG[constant_val];
}

void game_time(void) {
    static uint8_t last_btn = 1;
    uint8_t current_btn = (GPIOC->IDR & (1 << 11)) ? 1 : 0;
    uint8_t button_clicked = (last_btn == 1 && current_btn == 0);
    last_btn = current_btn;

    switch (current_state) {
        case MSG_PLAYER1: {
            char P1_Message[] = {
                SPACE, SPACE, SPACE, SPACE, SPACE, SPACE, SPACE, SPACE,
                CHAR_P, CHAR_L, CHAR_A, CHAR_Y, CHAR_E, CHAR_R, SPACE, CHAR_1,
                SPACE, SPACE, SPACE, SPACE, SPACE, SPACE, SPACE, SPACE
            };
            for (int shift = 0; shift <= (sizeof(P1_Message) - 8); shift++) {
                for (int i = 0; i < 8; i++) {
                    display_buffer[7 - i] = get_7seg_bits(P1_Message[shift + i]);
                    blink_mask[i] = 0xFF;
                }
                HAL_Delay(150);
            }
            ship_count = 0;
            current_state = PLACE_PLAYER1;
            break;
        }

        case PLACE_PLAYER1:
            if (button_clicked) place_ship(ADC_Values);
            break;

        case MSG_PLAYER2: {
            char P2_Message[] = {
                SPACE, SPACE, SPACE, SPACE, SPACE, SPACE, SPACE, SPACE,
                CHAR_P, CHAR_L, CHAR_A, CHAR_Y, CHAR_E, CHAR_R, SPACE, CHAR_2,
                SPACE, SPACE, SPACE, SPACE, SPACE, SPACE, SPACE, SPACE
            };
            for (int shift = 0; shift <= (sizeof(P2_Message) - 8); shift++) {
                for (int i = 0; i < 8; i++) {
                    display_buffer[7 - i] = get_7seg_bits(P2_Message[shift + i]);
                    blink_mask[i] = 0xFF;
                }
                HAL_Delay(150);
            }
            ship_count = 0;
            current_state = PLACE_PLAYER2;
            break;
        }

        case PLACE_PLAYER2:
            if (button_clicked) place_ship(ADC_Values);
            break;

        default: break;
    }
}

void place_ship(uint32_t *values) {
    uint8_t *current_map = (current_state == PLACE_PLAYER1) ? p1_ships : p2_ships;
    int col;
    uint8_t pattern;
    get_cursor(&col, &pattern);

    if (current_map[col] & ~pattern) {
        current_map[col] &= pattern;
        ship_count++;
    }

    if (ship_count >= MAX_SHIPS) {
        current_state = (current_state == PLACE_PLAYER1) ? MSG_PLAYER2 : BATTLE_PHASE;
    }
}

void draw_board(void) {
    uint8_t *active_map = (current_state == PLACE_PLAYER2) ? p2_ships : p1_ships;

    for (int i = 0; i < 8; i++) {
        display_buffer[i] = active_map[i];
        blink_mask[i] = 0xFF;
    }

    static uint32_t last_blink = 0;
    static uint8_t blink_on = 1;
    if (HAL_GetTick() - last_blink > 200) {
        blink_on = !blink_on;
        last_blink = HAL_GetTick();
    }

    if (blink_on) {
        int c_col;
        uint8_t c_pat;
        get_cursor(&c_col, &c_pat);
        display_buffer[c_col] &= c_pat;
    }

}

uint8_t convert_ascii_to_7seg(char c) {
    uint8_t pattern = _7SEG_ASCII[(unsigned char)c];
    return (pattern == 0 && c != ' ') ? 0xFF : pattern;
}

void scroll_message(char* str) {
    int len = 0;
    while(str[len] != '\0') len++;
    for (int shift = 0; shift <= (len + 8); shift++) {
        for (int i = 0; i < 8; i++) {
            int idx = shift + i - 8;
            display_buffer[7 - i] = (idx >= 0 && idx < len) ? convert_ascii_to_7seg(str[idx]) : 0xFF;
            blink_mask[i] = 0xFF;
        }
        HAL_Delay(150);
    }
}

void game_result(void) {}
