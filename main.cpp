#include "emulatorcore.h"

int main() {
    EmulatorCore * emu = new EmulatorCore();
    emu->loadFonts();
    if(!emu->loadROM("chip8-roms-master\\demos\\Maze (alt) [David Winter, 199x].ch8")) 
    {
        std::cout << "Error loading ROM" << std::endl;
        return 1;
    }
    for (int i = 0; i < 1000; i++) {
        emu->executeCycle();
        if (emu->Error()) {
            std::cout << "Error: " << emu->getError() << std::endl;
        }
    }
}