#ifndef CPU_H
#define CPU_H

#include "memory.h"
#include "opcodes.h"
#include <SDL2/SDL_events.h>
#include <cstdint>
#include <iostream>
#include <stack>

#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32

const static uint8_t KEYMAP[16] = {
    SDLK_x, // 0
    SDLK_1, // 1
    SDLK_2, // 2
    SDLK_3, // 3
    SDLK_q, // 4
    SDLK_w, // 5
    SDLK_e, // 6
    SDLK_a, // 7
    SDLK_s, // 8
    SDLK_d, // 9
    SDLK_z, // A
    SDLK_c, // B
    SDLK_4, // C
    SDLK_r, // D
    SDLK_f, // E
    SDLK_v  // F
};

const static uint8_t FONTSET[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

class Chip8 {
public:
  Memory memory;
  uint16_t pc;        // Program Counter
  uint8_t V[16];      // Registers V0-VF
  uint16_t I;         // I Index Register
  uint16_t stack[16]; // Stack
  uint8_t sp;         // Stack pointer

  bool render_flag;

  uint8_t key_pressed;

  uint8_t delay_timer;
  uint8_t sound_timer;

  uint8_t screen[SCREEN_HEIGHT][SCREEN_WIDTH];

  uint8_t keyboard[16];

  Chip8() {
    pc = 0x200; // Programs in chip-8 are loaded from 0x200
    I = 0;
    sp = 0;
    delay_timer = 0;
    sound_timer = 0;
    render_flag = false;

    for (int i = 0; i < 16; i++)
      keyboard[i] = 0;

    for (int i = 0; i < 16; i++)
      V[i] = 0;

    for (int i = 0; i < 16; i++)
      stack[i] = 0;

    for (int i = 0; i < SCREEN_HEIGHT; i++)
      for (int j = 0; j < SCREEN_WIDTH; j++)
        screen[i][j] = 0;

    for (int i = 0; i < 80; i++)
      memory.write_memory(i, FONTSET[i]);

    key_pressed = 0;
  }

  // Complete documentation of instrcutions
  // http://devernay.free.fr/hacks/chip8/C8TECH10.HTM

  void Cycle() {

    if (delay_timer > 0)
      delay_timer--;
    if (sound_timer > 0)
      sound_timer--;

    uint16_t opcode = memory.read_memory(pc) << 8 | memory.read_memory(pc + 1);

    switch (opcode & 0xF000) {
    case 0x0000:
      switch (opcode & 0x00FF) {
      case CLS:
        for (int i = 0; i < SCREEN_HEIGHT; i++)
          for (int j = 0; j < SCREEN_WIDTH; j++)
            screen[i][j] = 0;
        break;
      case RET:
        sp--;
        pc = stack[sp];
        break;
      }
      break;
    case JP:
      pc = opcode & 0x0FFF;
      return;
    case CALL:
      stack[sp] = pc;
      ++sp;
      pc = opcode & 0x0FFF;
      return;
    case SE_Vx_byte: {
      uint8_t target_x = (opcode & 0x0F00) >> 8;
      uint8_t kk = opcode & 0x00FF;
      if (V[target_x] == kk)
        pc += 4;
      break;
    }
    case SNE_Vx_byte: {
      uint8_t target_x = (opcode & 0x0F00) >> 8;
      uint8_t kk = opcode & 0x00FF;

      if (V[target_x] != kk)
        pc += 4;
      break;
    }
    case SE_Vx_Vy: {
      uint8_t target_x = (opcode & 0x0F00) >> 8;
      uint8_t target_y = (opcode & 0x00F0) >> 4;

      if (V[target_x] == V[target_y])
        pc += 4;
      break;
    }

    case LD_Vx_byte: { // vx = kk
      uint8_t target_x = (opcode & 0x0F00) >> 8;
      uint8_t kk = opcode & 0x00FF;
      V[target_x] = kk;
      break;
    }

    case ADD_Vx: { // vx += kk
      uint8_t target_x = (opcode & 0x0F00) >> 8;
      uint8_t kk = opcode & 0x00FF;
      V[target_x] += kk;
      break;
    }

    case 0x8000: {
      switch (opcode & 0x000F) {
      case LD_Vx_Vy: {
        uint8_t target_x = (opcode & 0x0F00) >> 8;
        uint8_t target_y = (opcode & 0x00F0) >> 4;
        V[target_x] = V[target_y];
        break;
      }
      case OR_Vx_Vy: {
        uint8_t target_x = (opcode & 0x0F00) >> 8;
        uint8_t target_y = (opcode & 0x00F0) >> 4;

        V[target_x] = (V[target_x] | V[target_y]);
        break;
      }
      case AND_Vx_Vy: {
        uint8_t target_x = (opcode & 0x0F00) >> 8;
        uint8_t target_y = (opcode & 0x00F0) >> 4;

        V[target_x] = (V[target_x] & V[target_y]);
        break;
      }
      case XOR_Vx_Vy: {
        uint8_t target_x = (opcode & 0x0F00) >> 8;
        uint8_t target_y = (opcode & 0x00F0) >> 4;

        V[target_x] = (V[target_x] ^ V[target_y]);
        break;
      }
      case ADD_Vx_Vy: {
        uint8_t target_x = (opcode & 0x0F00) >> 8;
        uint8_t target_y = (opcode & 0x00F0) >> 4;
        uint16_t sum = (V[target_x] + V[target_y]);

        if (sum > 255) {
          V[0xF] = 1;
        } else {
          V[0xF] = 0;
        }

        V[target_x] = (sum & 0xFF);
        break;
      }
      case SUB_Vx_Vy: {
        uint8_t target_x = (opcode & 0x0F00) >> 8;
        uint8_t target_y = (opcode & 0x00F0) >> 4;

        if (V[target_x] > V[target_y]) {
          V[0xF] = 1;
        } else {
          V[0xF] = 0;
        }

        V[target_x] -= V[target_y];
        break;
      }
      case SHR_Vx_Vy: {
        uint8_t target_x = (opcode & 0x0F00) >> 8;

        if (V[target_x] % 2 == 1) {
          V[0xF] = 1;
        } else {
          V[0xF] = 0;
        }

        V[target_x] = V[target_x] >> 1;
        break;
      }
      case SUBN_Vx_Vy: {
        uint8_t target_x = (opcode & 0x0F00) >> 8;
        uint8_t target_y = (opcode & 0x00F0) >> 4;
        if (V[target_y] > V[target_x]) {
          V[0xF] = 1;
        } else {
          V[0xF] = 0;
        }

        V[target_x] = V[target_y] - V[target_x];
        break;
      }
      case SHL_Vx_Vy: {
        uint8_t target_x = (opcode & 0x0F00) >> 8;
        uint8_t target_y = (opcode & 0x00F0) >> 4;

        V[0xF] = (V[target_y] & 0x80) >> 7;
        V[target_x] = V[target_y] << 1;
        break;
      }
      }
      break;
    }

    case SNE_Vx_Vy: {
      uint8_t target_x = (opcode & 0x0F00) >> 8;
      uint8_t target_y = (opcode & 0x00F0) >> 4;

      if (V[target_x] != V[target_y])
        pc += 4;

      break;
    }

    case LD_I: { // i = nnn
      uint16_t nnn = opcode & 0x0FFF;
      I = nnn;
      break;
    }

    case JP_V0_addr: {
      uint16_t nnn = opcode & 0x0FFF;
      pc = (nnn + V[0]);
      return;
    }

    case RND_Vx_byte: {
      uint8_t target_x = (opcode & 0x0F00) >> 8;
      uint8_t kk = opcode & 0x00FF;
      uint8_t rand_num = rand() % 256;

      V[target_x] = rand_num & kk;
      break;
    }

    case DRW: {
      uint8_t target_v_reg_x = (opcode & 0x0F00) >> 8;
      uint8_t target_v_reg_y = (opcode & 0x00F0) >> 4;
      uint8_t sprite_height = opcode & 0x000F;
      uint8_t x_location = V[target_v_reg_x];
      uint8_t y_location = V[target_v_reg_y];
      uint8_t pixel;

      V[0xF] = 0;
      for (int y_coordinate = 0; y_coordinate < sprite_height; y_coordinate++) {
        pixel = memory.read_memory(I + y_coordinate);
        for (int x_coordinate = 0; x_coordinate < 8; x_coordinate++) {
          if ((pixel & (0x80 >> x_coordinate)) != 0) {
            if (screen[y_location + y_coordinate][x_location + x_coordinate] ==
                1) {
              V[0xF] = 1;
            }
            screen[y_location + y_coordinate][x_location + x_coordinate] ^= 1;
          }
        }
      }

      render_flag = true;
      break;
    }

    case 0xE000: {
      switch (opcode & 0x00FF) {
      case SKP_Vx: {
        uint8_t target_x = (opcode & 0x0F00) >> 8;
        if (keyboard[V[target_x]] != 0)
          pc += 4;
        break;
      }

      case SKNP_Vx: {
        uint8_t target_x = (opcode & 0x0F00) >> 8;
        if (keyboard[V[target_x]] == 0)
          pc += 4;
        break;
      } break;
      }
      break;
    }

    case 0xF000: {
      switch (opcode & 0x00FF) {
      case LD_Vx_DT: {
        uint8_t target_x = (opcode & 0x0F00) >> 8;
        V[target_x] = delay_timer;
        break;
      }
      case LD_Vx_K: {
        uint8_t target_x = (opcode & 0x0F00) >> 8;

        key_pressed = 0;

        for (int i = 0; i < 16; i++) {
          if (keyboard[i] != 0) {
            V[target_x] = i;
            key_pressed = 1;
          }
        }

        if (!key_pressed)
          return;

        break;
      }
      case LD_DT_Vx: {
        uint8_t target_x = (opcode & 0x0F00) >> 8;
        delay_timer = V[target_x];
        break;
      }
      case LD_ST_Vx: {
        uint8_t target_x = (opcode & 0x0F00) >> 8;
        sound_timer = V[target_x];
        break;
      }
      case ADD_I_Vx: {
        uint8_t target_x = (opcode & 0x0F00) >> 8;
        I += V[target_x];
        break;
      }
      case LD_F_Vx: {
        uint8_t target_x = (opcode & 0x0F00) >> 8;
        I = (V[target_x] * 0x5);
        break;
      }
      case LD_B_Vx: {
        uint8_t target_x = (opcode & 0x0F00) >> 8;
        memory.write_memory(I, V[target_x] / 100);
        memory.write_memory(I + 1, (V[target_x] / 10) % 10);
        memory.write_memory(I + 2, (V[target_x] % 100) % 10);
        break;
      }
      case LD_I_Vx: {
        uint8_t target_x = (opcode & 0x0F00) >> 8;

        for (int i = 0; i <= target_x; ++i) {
          memory.write_memory(I + 1, V[i]);
        }
        break;
      }
      case LD_Vx_I: {
        uint8_t target_x = (opcode & 0x0F00) >> 8;
        for (int i = 0; i <= target_x; ++i) {
          V[i] = memory.read_memory(I + 1);
        }
      } break;
      }
    } break;
    }
    if ((opcode & 0xF000) != JP && (opcode & 0xF000) != CALL) {
      pc += 2;
    }
  }

  void input() {}
};

#endif