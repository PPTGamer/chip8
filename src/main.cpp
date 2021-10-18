
#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <stack>
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <nfd.hpp>
#include "chip8.h"

nfdresult_t request_file(NFD::UniquePath& outPath) {
    // initialize NFD
    NFD::Guard nfdGuard;

    // prepare filters for the dialog
    nfdfilteritem_t filterItem[1] = {{"CHIP-8 program", "ch8"}};

    // show the dialog
    nfdresult_t result = NFD::OpenDialog(outPath, filterItem, 1);

    // NFD::Guard will automatically quit NFD.
    return result;
}

int main() 
{   
    // Initialize hardware
    Chip8 chip8;
    sf::RenderWindow window(sf::VideoMode(200, 200), "CHIP-8 Intepreter by PPTGamer"); 
    sf::RectangleShape rects[CHIP8_DISPLAY_HEIGHT][CHIP8_DISPLAY_WIDTH];
    float size = 3;
    for (int x = 0; x < CHIP8_DISPLAY_WIDTH; x++)
    {
        for (int y = 0; y < CHIP8_DISPLAY_HEIGHT; y++)
        {
            rects[y][x].setPosition({x * size, y * size});
            rects[y][x].setSize({size, size});
            rects[y][x].setFillColor(sf::Color(0,255,0));
        }
    }

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
                if (event.type == sf::Event::KeyPressed)
                {
                    
                }
                if (event.type == sf::Event::KeyPressed)
                {

                }
            } 
            // update chip 8
            chip8.update(main_clock.restart());
            
            // render
            window.clear(); 
            auto display = chip8.get_display();
            bool sound = chip8.get_sound();
            for (int x = 0; x < CHIP8_DISPLAY_WIDTH; x++)
            {
                for (int y = 0; y < CHIP8_DISPLAY_HEIGHT; y++)
                {
                    if (display[y][x])
                    {
                        rects[y][x].setFillColor(sf::Color::White);
                    }
                    else
                    {
                        rects[y][x].setFillColor(sf::Color::Black);
                    }
                    window.draw(rects[y][x]);
                }
            }
            window.display(); 
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