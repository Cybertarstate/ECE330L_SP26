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
#include "seg7.h"

uint8_t convert_ascii_to_7seg(char c);
void update_display(uint8_t *ship_map, uint32_t *values);
void place_ship(uint32_t *values);

extern uint32_t ADC_Values[3];
uint8_t display_buffer[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
uint8_t dim_buffer[8] = {1, 1, 1, 1, 1, 1, 1, 1};
uint8_t blink_mask[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
uint8_t p1_ships[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
uint8_t p2_ships[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
GameState current_state = MSG_PLAYER1; // Make sure this matches your header enum
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

struct player_maps {
    // 1 = Boat, 0 = Empty
    uint8_t boat_vert[2][16];
    uint8_t boat_horiz[3][8];

    // 1 = Shot, 0 = No shot
    uint8_t shot_vert[2][16];
    uint8_t shot_horiz[3][8];
};

struct gamelogic {
    int cursor_x;
    int cursor_y;
    int is_vert_mode;
};

struct gamelogic game = {0, 0, 0};

char Message[] = {
    SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,
    CHAR_B,CHAR_A,CHAR_T,CHAR_T,CHAR_L,CHAR_E,CHAR_S,CHAR_H,CHAR_I,CHAR_P,CHAR_S,
    SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE,SPACE
};
extern char _7SEG[];

void Battleship_Init(void) {
    // 1. Configure ADC1 for 8-bit Scan Mode
    RCC->APB2ENR |= (1 << 8);
    ADC1->CR2 |= 1;
    ADC1->CR1 |= (1 << 25) | (1 << 8); // 8-bit + Scan Mode

    // 2. Clear Buffer
    for(int i = 0; i < 8; i++) display_buffer[i] = 0xFF;
}

void Battleship_StartScreen(void) {
    int total_scroll_steps = sizeof(Message) - 8;
    for (int shift = 0; shift <= total_scroll_steps; shift++) {
        for (int i = 0; i < 8; i++) {
            display_buffer[7 - i] = _7SEG[(uint8_t)Message[shift + i]];
        }
        HAL_Delay(150);
    }
    for(int i = 0; i < 8; i++) display_buffer[i] = 0xFF;
}

void scroll_message(char* str) {
    int len = 0;
    while(str[len] != '\0') len++; // Get length

    // Simple scroll logic using your _7SEG table
    for (int shift = 0; shift <= (len + 8); shift++) {
        for (int i = 0; i < 8; i++) {
            int char_idx = shift + i - 8;
            if (char_idx >= 0 && char_idx < len) {
                // You'll need a helper to convert ASCII to your _7SEG indices
                display_buffer[7 - i] = convert_ascii_to_7seg(str[char_idx]);
            } else {
                display_buffer[7 - i] = 0xFF;
            }
        }
        HAL_Delay(150);
    }
}

void cursor_map(uint32_t *values) {
    for(int i = 0; i < 8; i++) {
        display_buffer[i] = 0xFF;
    }

    if (values[2] < 128) {
        // --- HORIZONTAL MODE ---
        int total_steps = (values[0] * 23) / 255;
        int col = 7 - (total_steps % 8);
        int row = total_steps / 8;

        if (row == 0)      display_buffer[col] &= top;
        else if (row == 1) display_buffer[col] &= middle;
        else               display_buffer[col] &= bottom;
    }
    else {
        // --- VERTICAL MODE ---
        int v_steps = (values[1] * 31) / 255;
        int row_set = v_steps / 16;
        int col = 7 - ((v_steps % 16) / 2);
        int side = v_steps % 2;

        if (row_set == 0) {
            if (side == 0) display_buffer[col] &= top_left;
            else           display_buffer[col] &= top_right;
        } else {
            if (side == 0) display_buffer[col] &= bottom_left;
            else           display_buffer[col] &= bottom_right;
        }
    }
}

int ship_count = 0;
const int MAX_SHIPS = 5;

void place_ship(uint32_t *values) {
    // 1. Determine which player's map we are currently editing
    uint8_t *current_map = (current_state == PLACE_PLAYER1) ? p1_ships : p2_ships;

    int col;
    uint8_t pattern;

    // 2. Calculate the specific segment based on Potentiometer inputs
    if (values[2] < 128) {
        // --- HORIZONTAL MODE ---
        int total_steps = (values[0] * 23) / 255;
        col = 7 - (total_steps % 8);
        int row = total_steps / 8;

        // Define patterns for top, middle, and bottom segments
        if (row == 0)      pattern = ~(0x01); // Segment A
        else if (row == 1) pattern = ~(0x40); // Segment G
        else               pattern = ~(0x08); // Segment D
    }
    else {
        // --- VERTICAL MODE ---
        int v_steps = (values[1] * 31) / 255;
        col = 7 - ((v_steps % 16) / 2);
        int row_set = v_steps / 16;
        int side = v_steps % 2;

        // Define patterns for side segments
        if (row_set == 0) {
            pattern = (side == 0) ? ~(0x20) : ~(0x02); // Top-Left vs Top-Right
        } else {
            pattern = (side == 0) ? ~(0x10) : ~(0x04); // Bottom-Left vs Bottom-Right
        }
    }

    // 3. Commit the ship if the segment is currently empty
    // Since segments are active-low, we check if the bit is currently '1' (off)
    if (current_map[col] & ~pattern) {
        current_map[col] &= pattern; // Turn the segment ON (0) in the ship map
        ship_count++;                // Increment the global ship counter
    }

    // 4. State Machine Transition Logic
    if (ship_count >= MAX_SHIPS) {
        ship_count = 0; // Reset counter for the next player or next phase

        if (current_state == PLACE_PLAYER1) {
            current_state = MSG_PLAYER2; // Move to Player 2 message
        } else if (current_state == PLACE_PLAYER2) {
            current_state = BATTLE_PHASE; // Both players ready, start the game!
        }
    }
}

void update_display(uint8_t *ship_map, uint32_t *values) {
    // 1. Start by loading the saved boats into the buffer
    // Since 0 is "ON" (active low), if a boat is there, it stays there.
    for(int i = 0; i < 8; i++) {
        display_buffer[i] = ship_map[i];
    }

    // 2. Calculate the cursor position based on pots
    int col;
    uint8_t cursor_pattern;

    if (values[2] < 128) { // Horizontal Mode
        int total_steps = (values[0] * 23) / 255;
        col = 7 - (total_steps % 8);
        int row = total_steps / 8;
        if (row == 0)      cursor_pattern = top;
        else if (row == 1) cursor_pattern = middle;
        else               cursor_pattern = bottom;
    } else { // Vertical Mode
        int v_steps = (values[1] * 31) / 255;
        col = 7 - ((v_steps % 16) / 2);
        int row_set = v_steps / 16;
        int side = v_steps % 2;
        if (row_set == 0) cursor_pattern = (side == 0) ? top_left : top_right;
        else              cursor_pattern = (side == 0) ? bottom_left : bottom_right;
    }

    if (blink_mask[col] == 0xFF) {
        display_buffer[col] &= cursor_pattern;
    }
}

static void composite () {

}


extern void draw_board (void) {
//	/*******************************************************************************
//	Code to mask and bit shift 0-7 value of digit and 0-15 value of hex_char
//	to output correct bit pattern to GPIO_Output
//	*******************************************************************************/
//	//turn it all off //bottom FF go high, latch in by setting upper FF to high on second line
//	//0xFFFF turns them all off, so i dont know why in the init they are all on
//	GPIOE->ODR = 0xFF;
//	GPIOE->ODR |= 0xFF00;
//	//select specific
//	//we are shifting a 0 value essentially
//	GPIOE->ODR = (0xFF00 | ~(1<<7)) & ~(1<<(8));
//	GPIOE->ODR |= 0xFF00;
//	//select specific
//	//did multiple lines, to play with it
//	//we are deffinetly just overwriting
//	//could prolly or in with prev value
//	//the AND portion should be the display select
//	//i think it should maybe be ~(1<<(i+8))
//	GPIOE->ODR = (0xFF00 | ~(1<<5)) & ~(1<<(8));
//	GPIOE->ODR |= 0xFF00;
//	//select specific
//	GPIOE->ODR = (0xFF00 | ~(1<<2)) & ~(1<<(4));
//	GPIOE->ODR |= 0xFF00;
//
//		return;

}

uint8_t convert_ascii_to_7seg(char c) {
    switch (c) {
        case 'P': return 0x0C; // Segments a, b, f, g
        case 'L': return 0x38; // Segments d, e, f
        case 'A': return 0x08; // Segments a, b, c, e, f, g
        case 'Y': return 0x11; // Segments b, c, d, f, g
        case 'E': return 0x06; // Segments a, d, e, f, g
        case 'R': return 0x2F; // Segments e, g (lowercase r)
        case '1': return 0x79; // Segments b, c
        case '2': return 0x24; // Segments a, b, d, e, g
        case ' ': return 0xFF; // All segments off
        default:  return 0xFF;
    }
}

extern void poll_input(void) {
    // ADC_Values[0] = X Pot, [1] = Y Pot, [2] = Mode Pot

    // 1. Mode Switch (Zone based)
    game.is_vert_mode = (ADC_Values[2] > 2048) ? 1 : 0;

    // 2. Map Navigation
    if (game.is_vert_mode) {
        // Vertical Map: 16 wide (X), 2 high (Y)
        game.cursor_x = ADC_Values[0] >> 8;  // 4096 / 256 = 16 positions
        game.cursor_y = ADC_Values[1] >> 11; // 4096 / 2048 = 2 positions
    } else {
        // Horizontal Map: 8 wide (X), 3 high (Y)
        game.cursor_x = ADC_Values[0] >> 9;  // 4096 / 512 = 8 positions
        game.cursor_y = (ADC_Values[1] * 3) >> 12; // 0, 1, or 2
    }
}

extern void gametime (void) {
    // 1. Clear everything
    for(int i=0; i<8; i++) display_buffer[i] = 0xFF;

    // 2. ONLY light up the bottom bar of Digit 0 (far right)
    display_buffer[0] = bottom;
    dim_buffer[0] = 1;
}

extern void gameresult (void) {

}
