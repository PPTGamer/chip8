
#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <stack>
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include "chip8.h"

int main() 
{    
    Chip8 chip8;
    sf::RenderWindow window(sf::VideoMode(200, 200), "SFML works!"); 
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

    std::ifstream file;
    file.open("IBM Logo.ch8", std::ios::binary);
    std::vector<std::uint8_t> bytes(std::istreambuf_iterator<char>(file), {});
    chip8.load_program(bytes);

    sf::Clock main_clock;
    while (window.isOpen()) 
    { 
        sf::Event event; 
        while (window.pollEvent(event)) 
        { 
            if (event.type == sf::Event::Closed) 
                window.close();
        } 
        chip8.update(main_clock.restart());
        window.clear(); 
        
        auto display = chip8.get_display();
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
 
    return 0; 
}