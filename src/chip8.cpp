#include "chip8.h"

const sf::Time Chip8::one_over_60 = sf::seconds(1.0/60.0);

Chip8::Chip8()
{
    init();
}

Chip8::~Chip8()
{

}

void Chip8::init()
{
    // Clock rate: 700 CHIP-8 instructions per second, configurable
    clock_speed_t = sf::seconds(1.0/700.0);

    // Clear memory, stack, display and registers
    memset(MEM, 0, sizeof MEM);
    memset(V, 0, sizeof V);
    exec_stack = {};
    display = {};
    PC = 0;
    current_PC = 0;
    I = 0;

    // initialize font cache at 0x050 to 0x09F
    uint16_t font_cache[] = {
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
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };
    for (int i = 0; i < 80; i++)
    {
        MEM[0x050 + i] = font_cache[i];
    }
}
void Chip8::load_program(std::vector<std::uint8_t>& bytes, std::uint16_t loc)
{
    std::uint16_t curr_loc = loc;
    for (auto&& byte : bytes)
    {
        MEM[curr_loc] = byte;
        curr_loc++;
    }
    PC = loc;
    current_PC = loc;
}
void Chip8::update(sf::Time delta_t)
{
    timer_elapsed_t += delta_t;
    while (timer_elapsed_t >= one_over_60)
    {
        timer_delay = ((timer_delay > 0) ? timer_delay - 1 : 0);
        timer_delay = ((timer_sound > 0) ? timer_sound - 1 : 0);
        timer_elapsed_t -= one_over_60;
    }
    clock_elapsed_t += delta_t;
    while (clock_elapsed_t >= clock_speed_t)
    {
        FDE();
        clock_elapsed_t -= clock_speed_t;
    }
}

void Chip8::FDE()
{
    // Fetch current instruction.
    current_PC = PC;
    std::uint16_t ins_U = MEM[PC];
    std::uint16_t ins_L = MEM[PC + 1];
    PC += 2;
    // Decode current instruction
    Instruction ins((ins_U << 8) | ins_L);
    // Execute current instruction
    switch (ins.opcode())
    {
        case 0x0:
            switch (ins.NNN()) 
            {
                case 0x0E0: // 00E0: clear screen
                    display = {};
                    std::cerr << "clear screen" << std::endl;
                    break;
                case 0x0EE: // 00EE: return from subroutine
                    if (exec_stack.empty())
                    {
                        raise(Chip8::Exception::STACK_UNDERFLOW);
                    }
                    else
                    {
                        PC = exec_stack.top();
                        exec_stack.pop();
                    }
                    break;
                default:
                    raise(Chip8::Exception::INVALID_INSTRUCTION);
            }
            break;
        case 0x1: // 1NNN: Jump to NNN
            PC = ins.NNN();
            break;
        case 0x2: // 2NNN: Subroutine starting at NNN
            exec_stack.push(PC);
            PC = ins.NNN();
            break;
        case 0x3: // 3XNN: skip if VX == NN
            if (V[ins.X()] == ins.NN()) PC += 2;
            break;
        case 0x4: // 4XNN: skip if VX != NN
            if (V[ins.X()] != ins.NN()) PC += 2;
            break;
        case 0x5: // 5XY0: skip if VX == VY
            if (ins.N() != 0)
            {
                raise(Chip8::Exception::INVALID_INSTRUCTION);
            }
            else
            {
                if (V[ins.X()] == V[ins.Y()]) PC += 2;
            }
            break;
        case 0x6: // 6XNN: set VX := NN
            V[ins.X()] = ins.NN();
            std::cerr << "set V" << ins.X() << " := " << std::hex << (int)ins.NN() << std::endl;
            break;
        case 0x7: // 7XNN: add VX += NN
            V[ins.X()] += ins.NN();
            std::cerr << "add V" << ins.X() << " += " << std::hex << (int)ins.NN() << std::endl;
            break;
        case 0x8:
            raise(Chip8::Exception::INVALID_INSTRUCTION);
            break;
        case 0x9: // 9XY0: skip if VX != VY
            if (ins.N() != 0)
            {
                raise(Chip8::Exception::INVALID_INSTRUCTION);
            }
            else
            {
                if (V[ins.X()] != V[ins.Y()]) PC += 2;
            }

            break;
        case 0xA: // ANNN: set the index register I to NNN
            I = ins.NNN();
            std::cerr << "set I := " << std::hex << (int)ins.NNN() << std::endl;
            break;
        case 0xB:
            raise(Chip8::Exception::INVALID_INSTRUCTION);
            break;
        case 0xC:
            raise(Chip8::Exception::INVALID_INSTRUCTION);
            break;
        case 0xD: // DXYN: display sprite to screen
            display_sprite(V[ins.X()], V[ins.Y()], ins.N());
            std::cerr << "display I, with bytes " << std::hex << (int)ins.N() << std::endl;
            break;
        case 0xE:
            raise(Chip8::Exception::INVALID_INSTRUCTION);
            break;
        case 0xF:
            raise(Chip8::Exception::INVALID_INSTRUCTION);
            break;
    }
}

void Chip8::display_sprite(int x, int y, int num_bytes)
{
    x %= CHIP8_DISPLAY_WIDTH;
    y %= CHIP8_DISPLAY_HEIGHT;
    V[0xF] = 0;
    for (int dy = 0; dy < num_bytes; dy++)
    {
        if (I + dy >= 4096)
        {
            raise(Chip8::Exception::MEMORY_OUT_OF_BOUNDS);
            break;
        }
        std::uint8_t current_byte = MEM[I + dy];
        // draw byte row
        for (int dx = 0; dx < 8; dx++)
        {
            int nx = x + dx;
            int ny = y + dy;
            if (nx < 0 || ny < 0 || nx >= CHIP8_DISPLAY_WIDTH || ny >= CHIP8_DISPLAY_HEIGHT) continue;

            bool sprite_bit = (((current_byte >> (7 - dx)) & 1) != 0) ;
            display[ny][nx] ^= sprite_bit;
            if (sprite_bit == 1 && display[ny][nx] == 0) V[0xF] = 1;
        }
    }
}

void Chip8::press_key(int key)
{
    if (key < 0 || key >= 16) raise(Chip8::Exception::INPUT_OUT_OF_BOUNDS);
    else
    {
        key_reg[key] = true;
    }
}

void  Chip8::release_key(int key)
{
    if (key < 0 || key >= 16) raise(Chip8::Exception::INPUT_OUT_OF_BOUNDS);
    else
    {
        key_reg[key] = false;
    }
}

std::array<std::array<bool, CHIP8_DISPLAY_WIDTH>, CHIP8_DISPLAY_HEIGHT> Chip8::get_display()
{
    return display;
}

bool Chip8::get_sound()
{
    return timer_sound > 0;
}

void Chip8::raise(Chip8::Exception e)
{
    switch (e)
    {
    case Chip8::Exception::INVALID_INSTRUCTION:
        
        break;
    case Chip8::Exception::STACK_UNDERFLOW:
        
        break;
    case Chip8::Exception::MEMORY_OUT_OF_BOUNDS:
        
        break;
    case Chip8::Exception::INPUT_OUT_OF_BOUNDS:
        
        break;
    default:
        std::cerr << "at PC:0x" << std::hex << std::setfill('0') << std::setw(3) << current_PC << std::endl;
        break;
    }
}