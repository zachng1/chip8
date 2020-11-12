#include "emulatorcore.h"

int main() {
    EmulatorCore * emu = new EmulatorCore();
    if(!emu->loadROM("chip8-roms-master/programs/Chip8 Picture.ch8")) 
    {
        std::cout << "Error loading ROM" << std::endl;
        return 1;
    }
    // emu->printHeap();
}