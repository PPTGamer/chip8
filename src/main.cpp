#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <stack>
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <nfd.hpp>
#include "chip8.h"

void handle_key_input(Chip8& chip8, sf::Event key_event)
{
    int key;
    switch (key_event.key.code)
    {
        case sf::Keyboard::Key::Num1: key = 0x1; break;
        case sf::Keyboard::Key::Num2: key = 0x2; break;
        case sf::Keyboard::Key::Num3: key = 0x3; break;
        case sf::Keyboard::Key::Num4: key = 0xC; break;
        case sf::Keyboard::Key::Q: key = 0x4; break;
        case sf::Keyboard::Key::W: key = 0x5; break;
        case sf::Keyboard::Key::E: key = 0x6; break;
        case sf::Keyboard::Key::R: key = 0xD; break;
        case sf::Keyboard::Key::A: key = 0x7; break;
        case sf::Keyboard::Key::S: key = 0x8; break;
        case sf::Keyboard::Key::D: key = 0x9; break;
        case sf::Keyboard::Key::F: key = 0xE; break;
        case sf::Keyboard::Key::Z: key = 0xA; break;
        case sf::Keyboard::Key::X: key = 0x0; break;
        case sf::Keyboard::Key::C: key = 0xB; break;
        case sf::Keyboard::Key::V: key = 0xF; break;
        default: return; break;
    }
    if (key_event.type == sf::Event::KeyPressed)
    {
        chip8.press_key(key);
    }
    else if (key_event.type == sf::Event::KeyReleased)
    {
        chip8.release_key(key);
    }
}

nfdresult_t request_file(NFD::UniquePath& outPath) {
    NFD::Guard nfdGuard;
    nfdfilteritem_t filterItem[1] = {{"CHIP-8 program", "ch8"}};
    nfdresult_t result = NFD::OpenDialog(outPath, filterItem, 1);
    return result;
}

int main() 
{   
    // Initialize hardware
    Chip8 chip8;
    // Initialize display
    sf::Color color_on = sf::Color::White;
    sf::RenderWindow window(sf::VideoMode(640, 320), "CHIP-8 Intepreter by PPTGamer"); 
    sf::RectangleShape rects[CHIP8_DISPLAY_HEIGHT][CHIP8_DISPLAY_WIDTH];
    float size = 10;
    for (int x = 0; x < CHIP8_DISPLAY_WIDTH; x++)
    {
        for (int y = 0; y < CHIP8_DISPLAY_HEIGHT; y++)
        {
            rects[y][x].setPosition({x * size, y * size});
            rects[y][x].setSize({size, size});
            rects[y][x].setFillColor(sf::Color::Black);
        }
    }
    // Initialize beeper
    sf::SoundBuffer sound_buffer;
    int num_samples = 44100; 
    std::int16_t samples[num_samples];
    //Fill samples with a pure A tone (440hrz)
    for (int i = 0; i < num_samples ; i++) {
        samples[i] = ( 5000 * sin(440.0f * (2.0f * 3.1415f) * i / 44100));
    }
    sound_buffer.loadFromSamples(samples, num_samples, 1, 44100);
    sf::Sound beeper(sound_buffer);
    beeper.setLoop(true);
    
    // load chip8 ROM
    NFD::UniquePath outPath;
    nfdresult_t nfd_result = request_file(outPath);
    if (nfd_result == NFD_OKAY)
    {
        std::cout << "Success!" << std::endl << outPath.get() << std::endl;
        std::ifstream file;
        file.open(outPath.get(), std::ios::binary);
        std::vector<std::uint8_t> bytes(std::istreambuf_iterator<char>(file), {});
        chip8.load_program(bytes);

        // start main loop
        sf::Clock main_clock;
        while (window.isOpen()) 
        { 
            sf::Event event; 
            while (window.pollEvent(event)) 
            { 
                if (event.type == sf::Event::Closed)
                {
                    window.close();
                }
                if (event.type == sf::Event::KeyPressed || event.type == sf::Event::KeyReleased)
                {
                    handle_key_input(chip8, event);
                }
                if (event.type == sf::Event::KeyPressed && event.key.control && event.key.code == sf::Keyboard::Key::D)
                {
                    chip8.mem_dump(std::cerr);
                }
            } 
            // update chip 8
            chip8.update(main_clock.restart());
            
            // render
            window.clear(); 
            auto display = chip8.get_display();
            bool sound_flag = chip8.get_sound();
            for (int x = 0; x < CHIP8_DISPLAY_WIDTH; x++)
            {
                for (int y = 0; y < CHIP8_DISPLAY_HEIGHT; y++)
                {
                    if (display[y][x])
                    {
                        rects[y][x].setFillColor(
                            {
                                (unsigned char)std::min(255, (int)(rects[y][x].getFillColor().r + 0.3 * color_on.r)),
                                (unsigned char)std::min(255, (int)(rects[y][x].getFillColor().g + 0.3 * color_on.g)),
                                (unsigned char)std::min(255, (int)(rects[y][x].getFillColor().b + 0.3 * color_on.b))
                            });
                    }
                    else
                    {
                        rects[y][x].setFillColor(
                            {
                                (unsigned char)std::max(0, rects[y][x].getFillColor().r - 10),
                                (unsigned char)std::max(0, rects[y][x].getFillColor().g - 10),
                                (unsigned char)std::max(0, rects[y][x].getFillColor().b - 10)
                            });
                    }
                    window.draw(rects[y][x]);
                }
            }
            window.display();
            if (sound_flag)
            {
                beeper.play();
            }
            else
            {
                beeper.stop();
            }
        } 
    }
    else if (nfd_result == NFD_CANCEL)
    {
        std::cout << "User pressed cancel." << std::endl;
    }
    else
    {
        std::cout << "Error: " << NFD::GetError() << std::endl;
    }
    return 0; 
}