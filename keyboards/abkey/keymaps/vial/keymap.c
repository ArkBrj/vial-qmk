// Copyright 2023 QMK
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H
#include "print.h"
#include "qp.h"
#include "ssd1306xled_font6x8.qff.h"
#include <stdio.h>

extern const char* labels[];

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT_ortho_5x4(
        KC_1,   KC_2,   _______,_______,
        MO(1),  MO(2),  _______,_______,
        _______,_______,_______,_______,
        _______,_______,_______,_______,
        _______,_______,_______,_______
    ),
    [1] = LAYOUT_ortho_5x4(
        KC_2,   KC_3,   _______,_______,
        KC_TRNS,MO(3),  _______,_______,
        _______,_______,_______,_______,
        _______,_______,_______,_______,
        _______,_______,_______,_______
    ),
    [2] = LAYOUT_ortho_5x4(
        KC_3,   KC_4,   _______,_______,
        MO(3),  KC_TRNS,_______,_______,
        _______,_______,_______,_______,
        _______,_______,_______,_______,
        _______,_______,_______,_______
    ),
    [3] = LAYOUT_ortho_5x4(
        KC_5,   KC_6,   _______,_______,
        KC_TRNS,KC_TRNS,_______,_______,
        _______,_______,_______,_______,
        _______,_______,_______,_______,
        _______,_______,_______,_______
    ),
};

// const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
//     [0] = LAYOUT_ortho_5x4(
//         KC_ESC,  KC_2,    _______, _______,
//         KC_1,    TO(1),   _______, _______,
//         _______, _______, _______, _______,
//         _______, _______, _______, _______,
//         _______, _______, _______, _______
//     ),
//     [1] = LAYOUT_ortho_5x4(
//         KC_ESC,  KC_4,    _______, _______,
//         KC_3,    TO(2),   _______, _______,
//         _______, _______, _______, _______,
//         _______, _______, _______, _______,
//         _______, _______, _______, _______
//     ),
//     [2] = LAYOUT_ortho_5x4(
//         KC_ESC,  KC_6,    _______, _______,
//         KC_5,    TO(0),   _______, _______,
//         _______, _______, _______, _______,
//         _______, _______, _______, _______,
//         _______, _______, _______, _______
//     ),
// };

const uint8_t LAYER_CNT = ARRAY_SIZE(keymaps);

typedef struct {
  uint8_t x;
  uint8_t y;
} coors_t;

coors_t keycoors[MATRIX_ROWS-1][MATRIX_COLS] = {0};

painter_device_t display;
painter_font_handle_t font;

const uint8_t DISPLAY_I2C_ADDR = 0x3c;
const uint8_t DISPLAY_WIDTH = 128;
const uint8_t DISPLAY_HEIGHT = 64;
const uint8_t DISPLAY_VIEWPORT_OFFS_X = 2;

const uint8_t KEY_DISPLAY_WIDTH = 31;
const uint8_t KEY_DISPLAY_HEIGHT = 11;

void keyboard_pre_init_user(void) {
    font = qp_load_font_mem(font_ssd1306xled_font6x8);

    display = qp_sh1106_make_i2c_device(DISPLAY_WIDTH + 2 * DISPLAY_VIEWPORT_OFFS_X, DISPLAY_HEIGHT, DISPLAY_I2C_ADDR);
    qp_init(display, QP_ROTATION_0);
    qp_power(display, true);
    qp_clear(display);
    
    uint8_t y = 0;
    for (int r = 0; r < MATRIX_ROWS-1; ++r) {
      uint8_t x = DISPLAY_VIEWPORT_OFFS_X;
      for (int c = 0; c < MATRIX_COLS; ++c) {
        keycoors[r][c].x = x;
        keycoors[r][c].y = y;
        x += KEY_DISPLAY_WIDTH;
      }
      y += KEY_DISPLAY_HEIGHT;
    }

    // for (int r = 0; r < MATRIX_ROWS-1; ++r) {
    //   for (int c = 0; c < MATRIX_COLS; ++c) {
    //     qp_rect(display,
    //             keycoors[r][c].x,
    //             keycoors[r][c].y,
    //             keycoors[r][c].x + KEY_DISPLAY_WIDTH,
    //             keycoors[r][c].y + KEY_DISPLAY_HEIGHT,
    //             255,
    //             255,
    //             255,
    //             false);
    //   }
    // }

    qp_flush(display);
    uprintln("keyboard_pre_init_user");
}

// void keyboard_post_init_user(void) {
//   // debug_enable=true;
//   // debug_matrix=true;
//   // debug_keyboard=false;
//   // debug_mouse=true;

//   uprintln("keyboard_post_init_user");
// }

void draw_label(layer_state_t state, uint8_t row, uint8_t col, bool inverted) {
  char buffer[8];
  const char *label = "";

  for (int layer = LAYER_CNT - 1; layer >= 0; --layer) {
    if (IS_LAYER_ON_STATE(state, layer)) {
      uint16_t kc = keymaps[layer][row][col];
      uint16_t lskc = kc & ~0x1f;
      if (lskc == QK_TO ||
            lskc == QK_MOMENTARY ||
            lskc == QK_DEF_LAYER ||
            lskc == QK_PERSISTENT_DEF_LAYER ||
            lskc == QK_TOGGLE_LAYER ||
            lskc == QK_ONE_SHOT_LAYER ||
            lskc == QK_LAYER_TAP_TOGGLE) {
        snprintf(buffer, sizeof(buffer), "<L%d>", kc & 0x1f);
        label = buffer;
        break;
      } else if (kc <= 255) {
        if (kc != KC_TRNS &&
            labels[(uint8_t)kc] != NULL) {
          label = labels[(uint16_t)kc];
          break;
        }
      }
    }
  }

  int16_t width = qp_textwidth(font, label);
  uint8_t offset_x = (KEY_DISPLAY_WIDTH - width) / 2 + 1;

  qp_rect(display,
          keycoors[row][col].x,
          keycoors[row][col].y,
          keycoors[row][col].x + KEY_DISPLAY_WIDTH,
          keycoors[row][col].y + KEY_DISPLAY_HEIGHT,
          0,
          0,
          0,
          true);

  qp_rect(display,
          keycoors[row][col].x,
          keycoors[row][col].y,
          keycoors[row][col].x + KEY_DISPLAY_WIDTH,
          keycoors[row][col].y + KEY_DISPLAY_HEIGHT,
          255,
          255,
          255,
          inverted);

  uint8_t fgval = inverted ? 0 : 255;

  qp_drawtext_recolor(display,
                      keycoors[row][col].x + offset_x,
                      keycoors[row][col].y + 2,
                      font,
                      label,
                      0, 0, fgval,
                      0, 0, ~fgval);
  
  qp_flush(display);
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  if (record->event.type == KEY_EVENT) {
    draw_label(layer_state, record->event.key.row, record->event.key.col, record->event.pressed);

    // if (record->event.pressed) {
    //   uprintf("%d", LAYER_CNT);
    // }
  }

  return true;
}

layer_state_t layer_state_set_user(layer_state_t state) {
  uprintf("Layer: %x\r\n", state);
  
  for (uint8_t r = 0; r < MATRIX_ROWS-1; ++r) {
    for (uint8_t c = 0; c < MATRIX_COLS; ++c) {
      draw_label(state, r, c, false);
    }
  }

  return state;
}

layer_state_t default_layer_state_set_user(layer_state_t state) {
  return layer_state_set_user(state);
}
