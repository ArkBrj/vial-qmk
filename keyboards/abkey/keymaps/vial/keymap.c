// Copyright 2023 QMK
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H
#include "qp.h"
#include "robotomono20.qff.h"
#include <stdio.h>

enum custom_keycodes {
    KC_P00 = SAFE_RANGE
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT_ortho_5x4(
        KC_ESC,  KC_2,    _______, _______,
        KC_1,    TO(1),   _______, _______,
        _______, _______, _______, _______,
        _______, _______, _______, _______,
        _______, _______, _______, _______
    ),
    [1] = LAYOUT_ortho_5x4(
        KC_ESC,  KC_4,    _______, _______,
        KC_3,    TO(2),   _______, _______,
        _______, _______, _______, _______,
        _______, _______, _______, _______,
        _______, _______, _______, _______
    ),
    [2] = LAYOUT_ortho_5x4(
        KC_ESC,  KC_6,    _______, _______,
        KC_5,    TO(0),   _______, _______,
        _______, _______, _______, _______,
        _______, _______, _______, _______,
        _______, _______, _______, _______
    ),
};

painter_device_t lcd;
painter_font_handle_t roboto_font;

layer_state_t layer_state_set_kb(layer_state_t state) {
  char buffer[64] = {0};
  switch (get_highest_layer(state)) {
    case 2:
      snprintf(buffer, sizeof(buffer), "LAYER 2");
      break;
    case 1:
      snprintf(buffer, sizeof(buffer), "LAYER 1");
      break;
    default:
      snprintf(buffer, sizeof(buffer), "LAYER 0");
      break;
    }

  int16_t width = qp_textwidth(roboto_font, buffer);
  qp_drawtext(lcd,
              (64 - (width / 2)),
              (32 - roboto_font->line_height),
              roboto_font,
              buffer);

  qp_flush(lcd);

  return state;
}

void keyboard_pre_init_user(void) {
    // char buffer[64] = "Test 00";
    lcd = qp_sh1106_make_i2c_device(128, 64, 0x3c);
    qp_init(lcd, QP_ROTATION_0);
    qp_power(lcd, true);
    qp_clear(lcd);
    // painter_image_handle_t img = qp_load_image_mem(gfx_reverb);
    // qp_drawimage(lcd, 0, 0, img);

    roboto_font = qp_load_font_mem(font_robotomono20);
    // int16_t width = qp_textwidth(roboto_font, buffer);
    // snprintf(buffer, sizeof(buffer), "Test %02d", width);
    // width = qp_textwidth(roboto_font, buffer);
    // qp_drawtext(lcd,
    //             (64 - (width / 2)),
    //             (32 - roboto_font->line_height),
    //             roboto_font,
    //             buffer);

    // qp_flush(lcd);
    // qp_close_image(img);
    // qp_rect(lcd, 2, 0, 129, 63, 255, 255, 255, false);
    // qp_flush(lcd);
}
