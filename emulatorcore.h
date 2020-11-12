#include <iostream>
#include <array>
#include <vector>
#include <stack>
#include <fstream>
#include <iomanip>

// Main CPU emulator
// Will need to be wrapped by a more general Emulator
// class that also handles graphics, sound, and the main loop
class EmulatorCore {
    public:
    EmulatorCore();
    bool loadROM(std::string ROMLocation);
    bool loadFonts();
    void printHeap();

    protected:
    int executeCycle();
    
    private:
    std::vector<uint8_t> heap;
    std::stack<uint16_t> stack;
    std::array<uint8_t, 16> registers;
    uint8_t soundRegister;
    uint8_t timerRegister;
    uint16_t pc;
    uint16_t startDataSegment;
    int STACKMAX;
    int HEAPMAX;

    std::vector<std::vector<bool>> pixelMap;

    // Opcode functions below:

    // 0x0000
    void SYS(uint16_t addr);
    void CLS();
    void RET();

    // 0x1000
    void JMP(uint16_t addr);
};