#include <SFML/Graphics.hpp>

int main() {
    // Tworzenie okna o wymiarach 800x600 pikseli
    sf::RenderWindow okno(sf::VideoMode(800, 600), "Test SFML");

    // P�tla gry
    while (okno.isOpen()) {
        sf::Event zdarzenie;
        while (okno.pollEvent(zdarzenie)) {
            if (zdarzenie.type == sf::Event::Closed) {
                okno.close(); // Zamkni�cie okna
            }
        }

        okno.clear(sf::Color::Blue); // Wyczy�� ekran (ustaw t�o na niebieskie)
        okno.display();              // Wy�wietl zawarto�� okna
    }

    return 0;
}