#ifndef CHIP8_H
#define CHIP8_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <stdbool.h>
#include <OpenGL/gl.h>
#include <stdint.h>


typedef struct {
    u_int16_t opcode;
    u_int8_t memory[4096];
    u_int8_t V[16];
    u_int16_t I;
    u_int16_t pc;
    u_int8_t gfx[64 * 32];
    u_int8_t delay_timer;
    u_int8_t sound_timer;
    u_int16_t stack[16];
    u_int16_t sp;
    u_int8_t key[16];
} chip8;

unsigned char chip8_fontset[] = {
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
    0xF0, 0x80, 0xF0, 0x80, 0x80, // F
};

void chip8_initialize(chip8 *machine)
{
    machine -> pc = 0x200;
    machine -> opcode = 0;
    machine -> I = 0;
    machine -> sp = 0;

    for (int i = 0; i < 2048; i++) {
        machine -> gfx[i] = 0;
    }

    for (int i = 0; i < 16; i++) {
        machine -> stack[i] = 0;
    }

    for (int i = 0; i < 16; i++) {
        machine -> V[i] = 0;
    }

    for (int i = 0; i < 4096; i++) {
        machine -> memory[i] = 0;
    }

    for(int i = 0; i < 80; i++) {
        machine -> memory[i + 80] = chip8_fontset[i];
    }

    machine -> sound_timer = 0;
    machine -> delay_timer = 0;
}

void incrementPC(chip8 *machine) {
    machine -> pc += 2;
}

void loadRom(char *file, chip8 *machine) {
    size_t size, curr;
    FILE *fp = fopen(file, "rb");

    if (!fp) {
        fprintf(stderr, "Bad File pointer");
        exit(EXIT_FAILURE);
    }

    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    if (size > (4096 - 0x200)) {
        fprintf(stderr, "Rom too big");
        exit(EXIT_FAILURE);
    }

    int uItem = sizeof(u_int8_t);
    size_t nItems = size/uItem;

    curr = 0;
    u_int8_t *pos = machine -> memory;
    pos += 0x200;
    curr = fread(pos, size, 1, fp);

    if (curr != 1) {
        fprintf(stderr, "Did not read entire file");
        exit(EXIT_FAILURE);
    }

    fclose(fp);
}

void chip8_emulateCycle(chip8 *machine) {
    srand(time(NULL));
    int graphicsSize = 64 * 32;
    u_int8_t x, y, byte;
    machine -> opcode = machine -> memory[machine ->pc] << 8 | machine -> memory[machine -> pc + 1];

    unsigned char first = machine -> opcode >> 12;

    //switch (first) {}
    if (first == 0x0) {
        if (machine -> opcode == 0x00E0) {
            for (int i = 0; i < graphicsSize; i++) {
                machine -> gfx[i] = 0;
            }
        }
        else if(machine -> opcode == 0x00EE) {
            machine -> sp -= 1;
            machine -> pc = machine -> stack[machine -> sp];
        }
        incrementPC(machine);
    }
    else if (first == 0x1) {
        machine -> pc = machine -> opcode & 0x0FFF;
    }
    else if (first == 0x2) {
        machine -> stack[machine -> sp] = machine -> pc;
        machine -> sp += 1;
        machine -> pc = machine -> opcode & 0x0FFF;
    }
    else if (first == 0x3) {
       x = (machine -> opcode & 0x0F00) >> 8;

        if (machine -> V[x] == (machine -> opcode & 0x0FF)) {
            incrementPC(machine);
        }
        incrementPC(machine);
    }
    else if (first == 0x3) {
        x = (machine -> opcode & 0x0F00) >> 8;
        byte = (machine -> opcode & 0x00FF);
        if (machine -> V[x] == byte) {
            incrementPC(machine);
        }
        incrementPC(machine);
    }
    else if (first == 0x4) {
        x = (machine -> opcode & 0x0F00) >> 8;
        byte = (machine -> opcode & 0x00FF);
        if (machine -> V[x] != byte) {
            incrementPC(machine);
        }
        incrementPC(machine);
    }
    else if (first == 0x5) {
        x = (machine -> opcode & 0x0F00) >> 8;
        y = (machine -> opcode & 0x00F0) >> 4;
        if (machine -> V[x] == machine -> V[y]) {
            incrementPC(machine);
        }
        incrementPC(machine);
    }
    else if (first == 0x6) {
        x = (machine -> opcode & 0x0F00) >> 8;
        byte = machine -> opcode & 0x00FF;
        machine -> V[x] = byte;
        incrementPC(machine);
    }
    else if (first == 0x7) {
        //if error think of overflow
        x = (machine -> opcode & 0x0F00) >> 8;
        byte = machine -> opcode & 0x00FF;
        machine -> V[x] += byte;
        incrementPC(machine);
    }
    else if (first == 0x8) {
        x = (machine -> opcode & 0x0F00) >> 8;
        y = (machine -> opcode & 0x00F0) >> 4;
        byte = (machine -> opcode & 0x000F);
        
        if (byte == 0) {
            machine -> V[x] = (machine -> V[x] | machine -> V[y]);
        }
        else if (byte == 1) {
            machine -> V[x] |= machine -> V[y];
        }
        else if (byte == 2) {
            machine -> V[x] &= machine -> V[y];
        }
        else if (byte == 3) {
            machine -> V[x] ^= machine -> V[y];
        }
        else if (byte == 4) {
            u_int16_t sum = machine -> V[x];
            sum += machine -> V[y];

            if (sum > 255) {
                machine -> V[0xF] = 1;
            }
            else {
                machine -> V[0xF] = 0;
            }

            machine -> V[x] = (u_int8_t)(sum & 0x00FF);
        }
        else if (byte == 5) {
            if (machine -> V[x] > machine -> V[y]) {
                machine -> V[0xF] = 1;
            }
            else {
                machine -> V[0xF] = 0;
            }
            machine -> V[x] -= machine -> V[y];
        }
        else if (byte == 6) {
            if ((machine -> V[x] & 0x0001) != 0) {
                machine -> V[0xF] = 1;
            }
            else {
                machine -> V[0xF] = 0;
            }
            machine -> V[x] >>= 1;
        }
        else if (byte == 7) {
            if (machine -> V[y] > machine -> V[x]) {
                machine -> V[0xF] = 1;
            }
            else {
                machine -> V[0xF] = 0;
            }
            machine -> V[x] = machine -> V[y] - machine -> V[x];
        }
        else if (byte == 14) {
            if ((machine -> V[x] & 0x80) != 0) {
                machine -> V[0xF] = 1;
            }
            else {
                machine -> V[0xF] = 0;
            }
            machine -> V[x] <<= 1;
        }
        else {
            fprintf(stderr, "Error translating last byte of opcode for msb 8");
        }
        incrementPC(machine);
    }
    else if (first == 0x9) {
        x = (machine -> opcode & 0x0F00) >> 8;
        y = (machine -> opcode & 0x00F0) >> 4;
        if (machine -> V[x] != machine -> V[y]) {
            incrementPC(machine);
        }
        incrementPC(machine);
    }
    else if (first == 0xA) {
        machine -> I = machine -> opcode & 0x0FFF;
        incrementPC(machine);
    }
    else if (first == 0xB) {
        machine -> pc = ((machine -> opcode & 0x0FFF) + ((u_int16_t)machine -> V[0]));
        incrementPC(machine);
    }
    else if (first == 0xC) {
        x = (machine -> opcode & 0x0F00) >> 8;
        machine -> V[x] = (u_int8_t)(((u_int32_t)rand()) & (machine -> opcode & 0x00FF));
        incrementPC(machine);
    }
    else if (first == 0xD) {
        machine -> V[0xF] = 0;
        x = (machine -> opcode & 0x0F00) >> 8;
        y = (machine -> opcode & 0x00F0) >> 4;
        u_int n = (machine -> opcode & 0x000F);

        u_int8_t xRegister = machine -> V[x];
        u_int8_t yRegister = machine -> V[y];

        size_t xx = 0;
        size_t yy = 0;

        while (yy < n) {
            u_int8_t pixel = machine -> memory[machine -> I + yy];
            xx = 0;
            while (xx < 8) {

                u_int8_t msb = 0x80;
                if ((pixel & (msb >> (((u_int)xx) & 0x7))) != 0) {
                    u_int8_t tX = (xRegister + xx) % 64;
                    u_int8_t tY = (yRegister + yy) % 32;

                    // think abt maybe parentheses
                    u_int16_t idx = tX + tY * 64;

                    machine -> gfx[idx] ^= 1;
                    if (machine -> gfx[idx] == 0) {
                        machine -> V[0xF] = 1;
                    }
                }
                xx += 1;
            }
            yy += 1;
        }
        incrementPC(machine);
    }
    else if (first == 0xE) {
        x = (machine -> opcode & 0x0F00) >> 8;
        u_int n = (machine -> opcode & 0x00FF);

        if (n == 0x9E) {
            if (machine -> key[machine -> V[x]] == 1) {
                incrementPC(machine);
            }
        }
        else if (n == 0xA1) {
            if (machine -> key[machine -> V[x]] == 0) {
                incrementPC(machine);
            }
        }

        incrementPC(machine);
    }
    else if (first == 0xF) {
        x = (machine -> opcode & 0x0F00) >> 8;

        if ((machine -> opcode & 0x00FF) == 0x07) {
            machine -> V[x] = machine -> delay_timer;
        }
        else if ((machine -> opcode & 0x00FF) == 0x0A) {
            bool wait = true;
            while (wait) {
                for(int i = 0; i < 16; i++) {
                    if (machine -> key[i] != 0) {
                        machine -> V[x] = (u_int8_t)machine -> key[i];
                        wait = false;
                        break;
                    }
                }
            }
        }
        else if ((machine -> opcode & 0x00FF) == 0x15) {
            machine -> delay_timer = machine -> V[x];
        }
        else if ((machine -> opcode & 0x00FF) == 0x18){
            machine -> sound_timer = machine -> V[x];
        }
        else if ((machine -> opcode & 0x00FF) == 0x1E) {
            machine -> I += machine -> V[x];
        }
        else if ((machine -> opcode & 0x00FF) == 0x29) {
            if (machine -> V[x]) {
                machine -> I = machine -> V[x] * 0x5;
            }
        }
        else if ((machine -> opcode & 0x00FF) == 0x33) {
            machine -> memory[machine -> I] = machine -> V[x] / 100;
            machine -> memory[machine -> I + 1] = (machine -> V[x] / 10) % 10;
            machine -> memory[machine -> I + 2] = machine -> V[x] % 10;


        }
        else if ((machine -> opcode & 0x00FF) == 0x55) {
            for(int i = 0; i <= x; i++) {
                int curr = i + machine -> I;
                machine -> memory[curr] = machine -> V[i];
            }
        }
        else if ((machine -> opcode & 0x00FF) == 0x65) {
            for(int i = 0; i <= x; i++) {
                int curr = i + machine -> I;
                machine -> V[i] = machine -> memory[curr];
            }
        }
        else {
            fprintf(stderr, "Failed to translat F opcode");
            exit(-1);
        }
        incrementPC(machine);
    }
    else {
        fprintf(stderr, "opcode error, could not translate");
        exit(EXIT_FAILURE);
    }

    if(machine -> delay_timer > 0) {
        machine -> delay_timer -= 1;
    }
    if(machine -> sound_timer > 0) {
        machine -> sound_timer -= 1;
    }
}
#endif