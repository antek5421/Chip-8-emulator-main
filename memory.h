#ifndef MEMORY_H
#define MEMORY_H

#include <cstdint>
#include <fstream>
#include <iosfwd>

class Memory {
public:
  Memory() {
    for (int i = 0; i < 4096; i++) { // Clear memory
      memory[i] = 0;
    }
  }

  uint8_t read_memory(uint16_t address) { return memory[address]; }

  void write_memory(uint16_t address, uint8_t value) {
    memory[address] = value;
  }

  // load rom
  void load_ROM(const char *filename) {
    std::ifstream rom(filename, std::ios::binary | std::ios::ate);
    if (rom.is_open()) {
      std::streampos size = rom.tellg();
      char *buffer = new char[size];
      rom.seekg(0, std::ios::beg);
      rom.read(buffer, size);
      rom.close();

      for (int i = 0; i < size; i++) {
        memory[0x200 + i] = buffer[i];
      }

      delete[] buffer;
    }
  }

private:
  uint8_t memory[4096]; // 4kb of memory
};

#endif