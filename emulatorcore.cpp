// Writing this interpreter in basic C++ (no libraries except std template)
// This means graphics implementation can be done in any library 
// (just needs to be able to handle the emulator object)
// Will decide later if this is to be done with SDL or QT.

#include "emulatorcore.h"

EmulatorCore::EmulatorCore() : 
pc{0x200}, 
startDataSegment{0x200}, 
dataSegment(0x1000, 0x0),
STACKMAX{0x10}, 
DATAMAX{0x1000},
pixelMap(64, std::vector<bool>(32, false)),
error{false},
wait{false} 
{
    for (auto& i: registers) {
        i = 0;
    }
    for (auto& i: keys) {
        i = false;
    }
    fontArray = {
    0xF0, 0x90, 0x90, 0x90, 0xF0,
    0x20, 0x60, 0x20, 0x20, 0x70,
    0xF0, 0x10, 0xF0, 0x80, 0xF0,
    0xF0, 0x10, 0xF0, 0x10, 0xF0,
    0x90, 0x90, 0xF0, 0x10, 0x10,
    0xF0, 0x80, 0xF0, 0x10, 0xF0,
    0xF0, 0x80, 0xF0, 0x90, 0xF0,
    0xF0, 0x10, 0x20, 0x40, 0x40,
    0xF0, 0x90, 0xF0, 0x90, 0xF0,
    0xF0, 0x90, 0xF0, 0x10, 0xF0,
    0xF0, 0x90, 0xF0, 0x90, 0x90,
    0xE0, 0x90, 0xE0, 0x90, 0xE0,
    0xF0, 0x80, 0x80, 0x80, 0xF0,
    0xE0, 0x90, 0x90, 0x90, 0xE0,
    0xF0, 0x80, 0xF0, 0x80, 0xF0,
    0xF0, 0x80, 0xF0, 0x80, 0x80
    };
}
// Load a ROM file into the dataSegment
bool EmulatorCore::loadROM(std::string ROMLocation) {
    std::ifstream file(ROMLocation, std::ios::binary);
    file.seekg(0, std::ios::end);
    int size = file.tellg();
    if (size > DATAMAX - 0x200) 
    {
        return false;
    }
    file.seekg(0, std::ios::beg);
    file.read(reinterpret_cast<char *>(&dataSegment[0x200]), size);
    if (file.bad() || file.fail()) 
    {
        return false;
    }
    return true;
}

// Initialise font sprites
void EmulatorCore::loadFonts() {
    memcpy(&dataSegment[0], fontArray.data(), 80);
    return;
}

void EmulatorCore::printHeap() {
    for (int i = 0; i < (0x1000)/0x10; i++) {
        std::cout << std::setw(3) << std::setfill('0') << std::hex << (i * 16) << ": ";
        for (int j = 0; j < 0x10; j++) {
            std::cout << std::setw(2) << std::setfill('0') << std::hex << (int) dataSegment[(i * 0x10) + j] << " ";
        }
        std::cout << std::endl;
    }
}

std::vector<std::vector<bool>> EmulatorCore::getPixels() {
    return pixelMap;
}

int EmulatorCore::executeCycle() {

    uint16_t instruction = dataSegment[pc];
    instruction <<= 8;
    instruction |= dataSegment[pc + 1];
    std::cout << std::hex << instruction << std::endl;

    // because these are always in the same place we can calculate them all
    // ahead of determining which function to call
    uint8_t register1 = (instruction & 0x0F00) >> 8;
    uint8_t register2 = (instruction & 0x00F0) >> 4;
    uint8_t constant = instruction & 0x00FF;
    uint16_t address = instruction & 0x0FFF;
    
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
            SYS(address);
            break;
        }
        break;

        // Jump instruction
        case 0x1000:
        JMP(address);
        break;

        // Call instruction
        case 0x2000:
        CALL(address);
        break;

        // Skip if constant equal instruction
        case 0x3000:
        JEC(register1, constant);
        break;

        // Skip if constant not equal instruction
        case 0x4000:
        JNEC(register1, constant);
        break;

        // Skip if registers equal instruction
        case 0x5000:
        switch (instruction & 0x000F) {
            case (0x0000):
            JE(register1, register2);
            break;
            default:
            badInstruction(instruction);
        }
        break;

        // Load register constant instruction
        case 0x6000:
        LDC(register1, constant);
        break;

        // Add register constant instruction
        case 0x7000:
        ADDC(register1, constant);
        break;

        // Register mathematics instructions
        case 0x8000:
        switch (instruction & 0x000F) {
            case 0x0000:
            LD(register1, register2);
            break;
            case 0x0001:
            OR(register1, register2);
            break;
            case 0x0002:
            AND(register1, register2);
            break;
            case 0x0003:
            XOR(register1, register2);
            break;
            case 0x0004:
            ADD(register1, register2);
            break;
            case 0x0005:
            SUB(register1, register2);
            break;
            case 0x0006:
            SHR(register1, register2);
            break;
            case 0x0007:
            SUBN(register1, register2);
            break;
            case 0x000E:
            SHL(register1, register2);
            break;
            default:
            badInstruction(instruction);
            break;
        }
        break;

        // Skip if registers not equal instruction
        case 0x9000:
        switch (instruction & 0x000F) {
            case 0x0000:
            JNE(register1, register2);
            break;
            default:
            badInstruction(instruction);
            break;
        }
        break;

        // Load address constant
        case 0xa000:
        LDA(address);
        break;

        // Jump to address + v0 instruction
        case 0xb000:
        JMPA(address);
        break;

        // Load register random instruction
        case 0xc000:
        RND(register1, constant);
        break;

        // Draw instruction
        case 0xd000:
        DRAW(register1, register2, constant & 0x0F);
        break;

        // Skip if key pressed/not pressed instructions
        case 0xe000:
        switch (instruction & 0x00FF) {
            case 0x009E:
            JK(register1);
            break;
            case 0x00A1:
            JNK(register1);
            break;
            default:
            badInstruction(instruction);
            break;
        }
        break;

        // Timers, key presses, memory access etc. instructions
        case 0xf000:
        switch (instruction & 0x00FF) {
            case 0x0007:
            SDLY(register1);
            break;
            case 0x000A:
            LDK(register1);
            break;
            case 0x0015:
            LDLY(register1);
            break;
            case 0x0018:
            LSND(register1);
            break;
            case 0x001E:
            ADDI(register1);
            break;
            case 0x0029:
            LDSPRT(register1);
            break;
            case 0x0033:
            LDBCD(register1);
            break;
            case 0x0055:
            SR(register1);
            break;
            case 0x0065:
            LDR(register1);
            break;
            default:
            badInstruction(instruction);
            break;
        }
        break;
    }
    pc += 2;
    return 0;
}

bool EmulatorCore::Error() {
    return error;
}
uint16_t EmulatorCore::getError() {
    return failedInstruction;
}

void EmulatorCore::badInstruction(uint16_t instruction) {
    error = true;
    failedInstruction = instruction;
    return;
}

void EmulatorCore::SYS(uint16_t addr) {
    return;
}

void EmulatorCore::CLS() {
    for (auto& i: pixelMap) {
        for (auto&& j: i) {
            j = false;
        }
    }
    return;
}

void EmulatorCore::RET() {
    pc = stack.top();
    stack.pop();
    return;
}

void EmulatorCore::JMP(uint16_t addr) {
    pc = addr - 2;
    return;
}

void EmulatorCore::CALL(uint16_t addr) {
    // TODO: ensure StackMax is followed
    stack.push(pc);
    pc = addr - 2;
    return;
}

void EmulatorCore::JEC(uint8_t register1, uint8_t constant) {
    if (registers[register1] == constant) {
        pc += 2;
    }
    return;
}

void EmulatorCore::JNEC(uint8_t register1, uint8_t constant) {
    if (registers[register1] != constant) {
        pc += 2;
    }
    return;
}

void EmulatorCore::JE(uint8_t register1, uint8_t register2) {
    if (registers[register1] == registers[register2]) {
        pc += 2;
    }
    return;
}

void EmulatorCore::LDC(uint8_t register1, uint8_t constant) {
    registers[register1] = constant;
    return;
}

void EmulatorCore::ADDC(uint8_t register1, uint8_t constant) {
    registers[register1] += constant;
    return;
}

void EmulatorCore::LD(uint8_t register1, uint8_t register2) {
    registers[register1] = registers[register2];
    return;
}

void EmulatorCore::OR(uint8_t register1, uint8_t register2) {
    registers[register1] |= registers[register2];
    return;
}

void EmulatorCore::AND(uint8_t register1, uint8_t register2) {
    registers[register1] &= registers[register2];
    return;
}

void EmulatorCore::XOR(uint8_t register1, uint8_t register2) {
    registers[register1] ^= registers[register2];
    return;
}

void EmulatorCore::ADD(uint8_t register1, uint8_t register2) {
    registers[0xF] = (((int)registers[register1] + (int)registers[register2]) > 0xFF) ? 1 : 0;
    registers[register1] += registers[register2];
    return;
}

void EmulatorCore::SUB(uint8_t register1, uint8_t register2) {
    registers[0xF] = registers[register1] > registers[register2] ? 1 : 0;
    registers[register1] -= registers[register2];
    return;
}

void EmulatorCore::SHR(uint8_t register1, uint8_t register2) {
    registers[0xF] = registers[register1] & 0x1 ? 1 : 0;
    registers[register1] >>= 1;
    return;
}

void EmulatorCore::SUBN(uint8_t register1, uint8_t register2) {
    registers[0xF] = registers[register2] > registers[register1] ? 1 : 0;
    registers[register1] = registers[register2] - registers[register1];
    return;
}

void EmulatorCore::SHL(uint8_t register1, uint8_t register2) {
    registers[0xF] = registers[register1] & 0x10 ? 1 : 0;
    registers[register1] <<= 1;
    return;
}

void EmulatorCore::JNE(uint8_t register1, uint8_t register2) {
    if (registers[register1] == registers[register2]) {
        pc += 2;
    }
    return;
}

void EmulatorCore::LDA(uint16_t addr) {
    IRegister = addr;
    return;
}

void EmulatorCore::JMPA(uint16_t addr) {
    pc = addr + registers[0x0] - 2;
    return;
}

void EmulatorCore::RND(uint8_t register1, uint8_t constant) {
    registers[register1] = std::rand() & 0xFF;
    registers[register1] &= constant;
    return;
}

void EmulatorCore::DRAW(uint8_t register1, uint8_t register2, uint8_t size) {
    uint8_t y = 0;
    for (uint16_t i = IRegister; i < IRegister + size; i++) {
        for (uint8_t x = registers[register1]; x < 0x8; x++) {
            //access the xth bit of the current byte
            //if 1, then corresponding coordinates on the map (x, y)
            //are set true.
            //x is register1 to begin with, incremented up to 8 (bits in a byte)
            //y is register2 to begin with, increments until size is reached
            pixelMap[x][registers[register2] + y] = (dataSegment[i] >> (7 - x)) ? true : false;
        }
        y++;
    }
}

void EmulatorCore::JK(uint8_t register1) {
    if (registers[register1] < 0x10 && keys[registers[register1]]) {
        pc += 2;
    }
    return;
}

void EmulatorCore::JNK(uint8_t register1) {
    if (registers[register1] < 0x10 && !keys[registers[register1]]) {
        pc += 2;
    }
    return;
}

void EmulatorCore::SDLY(uint8_t register1) {
    registers[register1] = timerRegister;
    return;
}

void EmulatorCore::LDK(uint8_t register1) {
    // This will pick up the first, i.e. lowest key pressed
    // Not sure what happens if multiple keys are pressed
    if (wait) {
        for (int i = 0; i < keys.size(); i++) {
            if (keys[i]) {
                registers[register1] = i;
                wait = false;
            }
        }
    }
    else
    {
        pc -=2;
        wait = true;
    }
    return;
}

void EmulatorCore::LDLY(uint8_t register1) {
    timerRegister = registers[register1];
    return;
}

void EmulatorCore::LSND(uint8_t register1) {
    soundRegister = registers[register1];
    return;
}

void EmulatorCore::ADDI(uint8_t register1) {
    IRegister += registers[register1];
    return;
}

void EmulatorCore::LDSPRT(uint8_t register1) {
    if (register1 > 0x10) return;
    else {
        IRegister = register1 * 5;
        return;
    }
}

void EmulatorCore::LDBCD(uint8_t register1) {
    dataSegment[IRegister] = (registers[register1] / 100) % 10;
    dataSegment[IRegister + 1] = (registers[register1] / 10) % 10;
    dataSegment[IRegister + 2] = (registers[register1] / 1) % 10;
}

void EmulatorCore::SR(uint8_t register1) {
    if (register1 > 0x10) register1 = 0x10;
    for (int i = 0; i < register1; i++) {
        dataSegment[IRegister + i] = registers[i];
    }
    return;
}

void EmulatorCore::LDR(uint8_t register1) {
    if (register1 > 0x10) register1 = 0x10;
    for (int i = 0; i < register1; i++) {
        registers[i] = dataSegment[IRegister + i];
    }
    return;
}

