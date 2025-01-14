#include <SFML/Graphics.hpp>

int main() {
    // Ustawienia okna
    const unsigned int SZEROKO��_OKNA = 800; // Szeroko�� okna w pikselach
    const unsigned int WYSOKO��_OKNA = 600; // Wysoko�� okna w pikselach
    const std::string TYTU�_OKNA = "Proste okno w SFML"; // Tytu� okna

    // Tworzenie okna
    sf::RenderWindow okno(sf::VideoMode(SZEROKO��_OKNA, WYSOKO��_OKNA), TYTU�_OKNA);

    // G��wna p�tla programu
    while (okno.isOpen()) {
        sf::Event zdarzenie;

        // Obs�uga zdarze� (np. zamkni�cie okna)
        while (okno.pollEvent(zdarzenie)) {
            if (zdarzenie.type == sf::Event::Closed) {
                okno.close(); // Zamkni�cie okna
            }
        }

        // Rysowanie na ekranie
        okno.clear(sf::Color::Black); // Wyczy�� ekran (np. ustaw czarne t�o)

        // Tu mo�na doda� kod rysuj�cy dodatkowe elementy

        okno.display(); // Wy�wietlenie zawarto�ci bufora na ekranie
    }

    return 0;
}
