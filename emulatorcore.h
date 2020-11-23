#include <iostream>
#include <array>
#include <vector>
#include <stack>
#include <fstream>
#include <iomanip>
#include <cstring>

// Main CPU emulator
// Will need to be wrapped by a more general Emulator
// class that also handles graphics, sound, and the main loop
class EmulatorCore {
    public:
    EmulatorCore();
    bool loadROM(std::string ROMLocation);
    void loadFonts();
    void printHeap();
    std::array<bool, 16> setKeys(std::array<bool, 16> input);
    std::array<bool, 16> getKeys();
    std::vector<std::vector<bool>> getPixels();
    int executeCycle();
    bool Error();
    uint16_t getError();
    
    private:
    std::vector<uint8_t> dataSegment;
    std::stack<uint16_t> stack;
    std::array<uint8_t, 16> registers;
    std::array<bool, 16> keys;
    std::array<uint8_t, 80> fontArray;
    uint8_t soundRegister;
    uint8_t timerRegister;
    uint16_t IRegister;
    uint16_t pc;
    uint16_t startDataSegment;
    int STACKMAX;
    int DATAMAX;

    bool wait;
    bool error;
    uint16_t failedInstruction;

    std::vector<std::vector<bool>> pixelMap;

    void badInstruction(uint16_t instruction);

    // Opcode functions below:

    // 0x0nnn
    void SYS(uint16_t addr);
    // 0x00E0
    void CLS();
    // 0x00EE
    void RET();
    // 0x1nnn
    void JMP(uint16_t addr);
    // 0x2nnn
    void CALL(uint16_t addr);
    // 0x3xkk
    void JEC(uint8_t register1, uint8_t constant);
    // 0x4xkk
    void JNEC(uint8_t register1, uint8_t constant);
    // 0x5xy0
    void JE(uint8_t register1, uint8_t register2);
    // 0x6xkk
    void LDC(uint8_t register1, uint8_t constant);
    // 0x7xkk
    void ADDC(uint8_t register1, uint8_t constant);
    // 0x8xy0
    void LD(uint8_t register1, uint8_t register2);
    // 0x8xy1
    void OR(uint8_t register1, uint8_t register2);
    // 0x8xy2
    void AND(uint8_t register1, uint8_t register2);
    // 0x8xy3
    void XOR(uint8_t register1, uint8_t register2);
    // 0x8xy4
    void ADD(uint8_t register1, uint8_t register2);
    // 0x8xy5
    void SUB(uint8_t register1, uint8_t register2);
    // 0x8xy6
    void SHR(uint8_t register1, uint8_t register2);
    // 0x8xy7
    void SUBN(uint8_t register1, uint8_t register2);
    // 0x8xyE
    void SHL(uint8_t register1, uint8_t register2);
    // 0x9xy0
    void JNE(uint8_t register1, uint8_t register2);
    // 0xAnnn
    void LDA(uint16_t addr);
    // 0xBnnn
    void JMPA(uint16_t addr);
    // 0xCxkk
    void RND(uint8_t register1, uint8_t constant);
    // 0xDxyn
    void DRAW(uint8_t register1, uint8_t register2, uint8_t size);
    // 0xEx9E
    void JK(uint8_t register1);
    // 0xExA1
    void JNK(uint8_t register1);
    // 0xFx07
    void SDLY(uint8_t register1);
    // 0xFx0A
    void LDK(uint8_t register1);
    // 0xFx15
    void LDLY(uint8_t register1);
    // 0xFx18
    void LSND(uint8_t register1);
    // 0xFx1E
    void ADDI(uint8_t register1);
    // 0xFx29
    void LDSPRT(uint8_t register1);
    // 0xFx33
    void LDBCD(uint8_t register1);
    // 0xFx55
    void SR(uint8_t register1);
    // 0xFx65
    void LDR(uint8_t register1);
};