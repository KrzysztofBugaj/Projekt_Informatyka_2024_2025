#include <SFML/Graphics.hpp>

int main() {
    // Tworzenie okna o wymiarach 800x600 pikseli
    sf::RenderWindow okno(sf::VideoMode(800, 600), "Test SFML");

    // Pêtla gry
    while (okno.isOpen()) {
        sf::Event zdarzenie;
        while (okno.pollEvent(zdarzenie)) {
            if (zdarzenie.type == sf::Event::Closed) {
                okno.close(); // Zamkniêcie okna
            }
        }

        okno.clear(sf::Color::Blue); // Wyczyœæ ekran (ustaw t³o na niebieskie)
        okno.display();              // Wyœwietl zawartoœæ okna
    }

    return 0;
}