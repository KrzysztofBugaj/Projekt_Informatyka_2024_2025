#include <SFML/Graphics.hpp>

int main() {
    // Ustawienia okna
    const unsigned int SZEROKOŒÆ_OKNA = 800; // Szerokoœæ okna w pikselach
    const unsigned int WYSOKOŒÆ_OKNA = 600; // Wysokoœæ okna w pikselach
    const std::string TYTU£_OKNA = "Proste okno w SFML"; // Tytu³ okna

    // Tworzenie okna
    sf::RenderWindow okno(sf::VideoMode(SZEROKOŒÆ_OKNA, WYSOKOŒÆ_OKNA), TYTU£_OKNA);

    // G³ówna pêtla programu
    while (okno.isOpen()) {
        sf::Event zdarzenie;

        // Obs³uga zdarzeñ (np. zamkniêcie okna)
        while (okno.pollEvent(zdarzenie)) {
            if (zdarzenie.type == sf::Event::Closed) {
                okno.close(); // Zamkniêcie okna
            }
        }

        // Rysowanie na ekranie
        okno.clear(sf::Color::Black); // Wyczyœæ ekran (np. ustaw czarne t³o)

        // Tu mo¿na dodaæ kod rysuj¹cy dodatkowe elementy

        okno.display(); // Wyœwietlenie zawartoœci bufora na ekranie
    }

    return 0;
}
