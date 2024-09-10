
#include "cpu.h"
#include "display.h"
#include <SDL2/SDL_events.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <stdlib.h>
#include <thread>

// TODO: move it to chip8 class
void input(Chip8 *chip8) {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_KEYDOWN) {
      for (int i = 0; i < 16; i++) {
        if (event.key.keysym.sym == KEYMAP[i]) {
          chip8->keyboard[i] = 1;
        }
      }
    }
    if (event.type == SDL_KEYUP) {
      for (int i = 0; i < 16; i++) {
        if (event.key.keysym.sym == KEYMAP[i]) {
          chip8->keyboard[i] = 0;
        }
      }
    }
    if (event.type == SDL_QUIT)
      return;
  }
}

int main(int argc, char *argv[]) {
  Chip8 *chip8 = new Chip8;
  Display display;
  SDL_Window *chip8_screen;
  SDL_Renderer *chip8_renderer;
  SDL_Texture *chip8_texture;

  uint32_t *pixel_buffer =
      (uint32_t *)malloc((SCREEN_HEIGHT * SCREEN_WIDTH) * sizeof(uint32_t));

  SDL_Init(SDL_INIT_EVERYTHING);
  display.init_window(&chip8_screen, &chip8_renderer, &chip8_texture);

  chip8->memory.load_ROM(argv[1]);

  SDL_Event event;
  while (true) {
    chip8->delay_timer++;

    // Quit button
    while (SDL_PollEvent(&event))
      if (event.type == SDL_QUIT)
        return 0;

    input(chip8);
    chip8->Cycle();

    // Render graphics
    if (chip8->render_flag) {
      display.buffer_graphics(chip8, pixel_buffer, chip8_renderer);
      display.draw_graphics(pixel_buffer, chip8_renderer, chip8_texture);
      chip8->render_flag = false;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(16)); // 60hz
  }

  delete (chip8);
}