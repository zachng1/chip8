// Writing this interpreter in basic C++ (no libraries except std template)
// This means graphics implementation can be done in any library 
// (just needs to be able to handle the emulator object)
// Will decide later if this is to be done with SDL or QT.

#include "emulatorcore.h"

EmulatorCore::EmulatorCore() : 
pc{0x200}, 
startDataSegment{0x200}, 
heap(0x1000, 0x0), 
STACKMAX{0x10}, 
HEAPMAX{0x1000},
pixelMap(64, std::vector<bool>(32, false)) 
{

}
// Load a ROM file into the heap
bool EmulatorCore::loadROM(std::string ROMLocation) {
    std::ifstream file(ROMLocation, std::ios::binary);
    file.seekg(0, std::ios::end);
    int size = file.tellg();
    if (size > HEAPMAX - 0x200) 
    {
        return false;
    }
    file.seekg(0, std::ios::beg);
    file.read(reinterpret_cast<char *>(&heap[0x200]), size);
    if (file.bad() || file.fail()) 
    {
        return false;
    }
    return true;
}
void EmulatorCore::printHeap() {
    for (int i = 0; i < (0x1000 - 0x200)/0x10; i++) {
        std::cout << std::setw(3) << std::setfill('0') << std::hex << 0x200 + (i * 16) << ": ";
        for (int j = 0; j < 0x10; j++) {
            std::cout << std::setw(2) << std::setfill('0') << std::hex << (int) heap[0x200 + (i * 0x10) + j] << " ";
        }
        std::cout << std::endl;
    }
}
int EmulatorCore::executeCycle() {
    if (pc % 2 != 0) {
        return 1; // PC unaligned
    }
    uint16_t instruction = heap[pc];
    instruction <<= 8;
    instruction |= heap[pc + 1];
    switch (instruction & 0xF000) {
        // System instructions
        case 0x0000:
        switch (instruction & 0x0FFF) {
            case 0x00E0:
            CLS();
            break;
            case 0x00EE:
            RET();
            break;
            default:
            SYS(instruction & 0x0FFF);
            break;
        }

        // Jump instruction
        case 0x1000:
        JMP(instruction & 0x0FFF);
        break;

        // Call instruction
        case 0x2000:

        // Skip if constant equal instruction
        case 0x3000:

        // Skip if constant not equal instruction
        case 0x4000:

        // Skip if registers equal instruction
        case 0x5000:

        // Load register constant instruction
        case 0x6000:

        // Add register constant instruction
        case 0x7000:

        // Register mathematics instructions
        case 0x8000:

        // Skip if registers not equal instruction
        case 0x9000:

        // Load address constant
        case 0xa000:

        // Jump to address + v0 instruction
        case 0xb000:

        // Load register random instruction
        case 0xc000:

        // Draw instruction
        case 0xd000:

        // Skip if key pressed/not pressed instructions
        case 0xe000:

        // Timers, key presses, memory access etc. instructions
        case 0xf000:
    }
    pc += 2;
    return 0;
}