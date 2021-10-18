#ifndef CHIP8
#define CHIP8
#include <cstdint>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <stack>
#include <array>
#include <vector>
#include <random>
#include <SFML/System.hpp>

const int CHIP8_DISPLAY_WIDTH = 64;
const int CHIP8_DISPLAY_HEIGHT = 32;

class Chip8
{
public:
    enum class Exception
    {
        INVALID_INSTRUCTION, 
        STACK_UNDERFLOW,
        MEMORY_OUT_OF_BOUNDS,
        INPUT_OUT_OF_BOUNDS,
    };
    Chip8();
    ~Chip8();
    void init();
    void load_program(std::vector<std::uint8_t>& bytes, std::uint16_t loc = 0x200);
    void press_key(int);
    void release_key(int);
    void update(sf::Time delta_t); // update timers
    std::array<std::array<bool, CHIP8_DISPLAY_WIDTH>, CHIP8_DISPLAY_HEIGHT>  get_display();
    bool get_sound();
    void mem_dump(std::ostream& out);
private:
    class Instruction // a 16-bit CHIP-8 instruction
    {
    private:
        std::uint16_t raw_instruction;
        /* 
        For a 16-bit instruction
        0xFFFF
          O
           X
            Y
             N
            NN
           NNN
        */
    public:
        Instruction(std::uint16_t ins) : raw_instruction(ins){};
        ~Instruction(){}
        inline std::uint16_t ins() const {return raw_instruction;}
        inline int opcode() const {return ((raw_instruction & 0xF000) >> 12);}
        inline int X() const {return ((raw_instruction & 0x0F00) >> 8);}
        inline int Y() const {return ((raw_instruction & 0x00F0) >> 4);}
        inline int N() const {return raw_instruction & 0x000F;}
        inline std::uint16_t NN() const {return raw_instruction & 0x00FF;}
        inline std::uint16_t NNN() const {return raw_instruction & 0x0FFF;}
    };

    // emulation parameters
    static const sf::Time one_over_60;
    std::uint16_t font_addr;
    sf::Time clock_speed_t; // instructions per second 
    sf::Time clock_elapsed_t;
    sf::Time timer_elapsed_t;
    static std::random_device RNG_random_device;
    static std::mt19937 RNG_gen;
    std::uniform_int_distribution<int> RNG_distrib;
    
    // Memory unit
    std::uint8_t MEM[4096];

    // Control unit
    std::uint16_t PC; // program counter
    std::uint16_t I; // index register, refers to memory locations
    std::uint8_t timer_delay;
    std::uint8_t timer_sound;
    std::uint8_t V[16]; // 8-bit registers
    std::stack<std::uint16_t> exec_stack;
    bool interrupt;
    int block; // -1 means no block, non-negative values indicate the register in which to record a keypress (FX0A)
    void FDE();
    void raise(Exception e);

    std::uint16_t current_PC;

    // Input unit
    bool key_reg[16];

    // Display
    std::array<std::array<bool, CHIP8_DISPLAY_WIDTH>, CHIP8_DISPLAY_HEIGHT> display;
    void display_sprite(int x, int y, int num_bytes);
};

#endif /* CHIP8 */
