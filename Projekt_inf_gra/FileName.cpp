#include <SFML/Graphics.hpp>
#include <vector>
#include <fstream>
#include <iostream>
#include <ctime>
#include <cstdlib>


using namespace sf;

// Definicje sta³ych
constexpr int szerokoscOkna{ 800 }, wysokoscOkna{ 600 };
constexpr float promienPilki{ 10.f }, predkoscPilki{ 4.0f };
constexpr float szerokoscPaletki{ 100.f }, wysokoscPaletki{ 20.f }, predkoscPaletki{ 8.0f };
constexpr float szerokoscCegielki{ 60.f }, wysokoscCegielki{ 20.f };
constexpr int liczbaCegielekX{ 11 }, liczbaCegielekY{ 4 };


//----------------------------------------------------------------------

enum PoziomTrudnosci { LATWY, SREDNI, TRUDNY };

class PoziomTrudnosciParametry {
public:
    static constexpr float predkoscPilkiLatwy{ 4.0f }, predkoscPilkiSredni{ 6.0f }, predkoscPilkiTrudny{ 8.0f };
    static constexpr float predkoscPaletkiLatwy{ 10.0f }, predkoscPaletkiSredni{ 8.0f }, predkoscPaletkiTrudny{ 6.0f };
    static constexpr int liczbaCegielekXLatwy{ 8 }, liczbaCegielekXSredni{ 11 }, liczbaCegielekXTrudny{ 14 };
    static constexpr int liczbaCegielekYLatwy{ 3 }, liczbaCegielekYSredni{ 4 }, liczbaCegielekYTrudny{ 5 };

    static float pobierzPredkoscPilki(PoziomTrudnosci poziom) {
        switch (poziom) {
        case LATWY: return predkoscPilkiLatwy;
        case SREDNI: return predkoscPilkiSredni;
        case TRUDNY: return predkoscPilkiTrudny;
        default: return predkoscPilkiLatwy;
        }
    }

    static float pobierzPredkoscPaletki(PoziomTrudnosci poziom) {
        switch (poziom) {
        case LATWY: return predkoscPaletkiLatwy;
        case SREDNI: return predkoscPaletkiSredni;
        case TRUDNY: return predkoscPaletkiTrudny;
        default: return predkoscPaletkiLatwy;
        }
    }

    static int pobierzLiczbeCegielekX(PoziomTrudnosci poziom) {
        switch (poziom) {
        case LATWY: return liczbaCegielekXLatwy;
        case SREDNI: return liczbaCegielekXSredni;
        case TRUDNY: return liczbaCegielekXTrudny;
        default: return liczbaCegielekXLatwy;
        }
    }

    static int pobierzLiczbeCegielekY(PoziomTrudnosci poziom) {
        switch (poziom) {
        case LATWY: return liczbaCegielekYLatwy;
        case SREDNI: return liczbaCegielekYSredni;
        case TRUDNY: return liczbaCegielekYTrudny;
        default: return liczbaCegielekYLatwy;
        }
    }
};

class WybórPoziomuTrudnoœci {
private:
    int wybranaOpcja;
    const int liczbaOpcji = 3;
    Text teksty[3];
    Font font;
    PoziomTrudnosci poziomTrudnosci;

public:
    WybórPoziomuTrudnoœci() : wybranaOpcja(0), poziomTrudnosci(LATWY) {
        if (!font.loadFromFile("Arial.ttf")) {
            std::cerr << "Nie uda³o siê za³adowaæ czcionki!" << std::endl;
        }

        teksty[0].setString("Latwy");
        teksty[1].setString("Sredni");
        teksty[2].setString("Trudny");

        for (int i = 0; i < liczbaOpcji; ++i) {
            teksty[i].setFont(font);
            teksty[i].setCharacterSize(24);
            teksty[i].setFillColor(Color::White);
            teksty[i].setPosition(300, 150 + i * 50);
        }
    }

    void rysuj(RenderWindow& okno) {
        for (int i = 0; i < liczbaOpcji; ++i) {
            if (i == wybranaOpcja) {
                teksty[i].setFillColor(Color::Green);
            }
            else {
                teksty[i].setFillColor(Color::White);
            }
            okno.draw(teksty[i]);
        }
    }

    void ruchWGore() {
        if (wybranaOpcja > 0) {
            wybranaOpcja--;
        }
    }

    void ruchWDol() {
        if (wybranaOpcja < liczbaOpcji - 1) {
            wybranaOpcja++;
        }
    }

    PoziomTrudnosci pobierzWybranyPoziom() {
        switch (wybranaOpcja) {
        case 0: return LATWY;
        case 1: return SREDNI;
        case 2: return TRUDNY;
        default: return LATWY;
        }
    }

    void ustawPoziomTrudnosci(PoziomTrudnosci poziom) {
        poziomTrudnosci = poziom;
    }

    PoziomTrudnosci getPoziomTrudnosci() const {
        return poziomTrudnosci;
    }
};




//---------------------------------------------------------------

class Pilka {
public:
    CircleShape ksztalt;
    Vector2f predkosc{ -predkoscPilki, -predkoscPilki };

    sf::Texture tekstura3, tekstura4, teksturaAnimacji; // Dodanie tekstury dla animacji
    sf::Sprite sprite;
    bool pokazTekstura3 = true; // Flaga do zmiany tekstur
    bool czyAnimacjaAktywna = false; // Flaga aktywnoœci animacji
    sf::Clock zegar, zegarAnimacji;  // Zegar dla zmiany tekstur i animacji
    float czasZmiany = 0.5f;         // Interwa³ zmiany tekstury
    float czasAnimacji = 0.1f;       // Czas trwania animacji kolizji

    Pilka(float x, float y) {
        ksztalt.setPosition(x, y);
        ksztalt.setRadius(promienPilki);
        ksztalt.setFillColor(Color::Red);
        ksztalt.setOrigin(promienPilki, promienPilki);

        if (!tekstura3.loadFromFile("ballBlue.png") ||
            !tekstura4.loadFromFile("ballGrey.png") ||
            !teksturaAnimacji.loadFromFile("wybuch.png")) { // Tekstura animacji
            std::cerr << "Nie uda³o siê za³adowaæ tekstur!" << std::endl;
            exit(EXIT_FAILURE);
        }

        sprite.setTexture(tekstura3); // Ustaw pocz¹tkow¹ teksturê
        sprite.setPosition(x, y);
    }

    void aktywujAnimacje() {
        czyAnimacjaAktywna = true;
        zegarAnimacji.restart();
        sprite.setTexture(teksturaAnimacji); // Ustaw teksturê dla animacji
        sprite.setOrigin(teksturaAnimacji.getSize().x / 2.0f, teksturaAnimacji.getSize().y / 2.0f);
        sprite.setScale(
            2.0f * promienPilki / teksturaAnimacji.getSize().x,
            2.0f * promienPilki / teksturaAnimacji.getSize().y

        );
        sprite.setPosition(ksztalt.getPosition()); // Ustaw poprawn¹ pozycjê sprite'a


    }

    void aktualizuj() {
        ksztalt.move(predkosc);
        sprite.setPosition(ksztalt.getPosition());

        // Obs³uga animacji kolizji
        if (czyAnimacjaAktywna) {
            if (zegarAnimacji.getElapsedTime().asSeconds() > czasAnimacji) {
                czyAnimacjaAktywna = false; // Wy³¹cz animacjê po up³ywie czasu
                sprite.setTexture(pokazTekstura3 ? tekstura3 : tekstura4); // Przywróæ cykliczn¹ zmianê tekstur
                sprite.setScale(1.0f, 1.0f); // Przywróæ standardow¹ skalê
                sprite.setOrigin(promienPilki, promienPilki); // Przywróæ œrodek na œrodek pi³ki
            }
            return; // Przerwij funkcjê, aby nie wykonywaæ dalszych operacji
        }

        // Odbicie od œcian bocznych
        if (lewo() < 0)
            predkosc.x = predkoscPilki;
        else if (prawo() > szerokoscOkna)
            predkosc.x = -predkoscPilki;

        // Odbicie od górnej œciany
        if (gora() < 0)
            predkosc.y = predkoscPilki;
        else if (dol() > wysokoscOkna)
            predkosc.y = -predkoscPilki;

        // Zmiana tekstur cyklicznie, jeœli animacja nie jest aktywna
        if (zegar.getElapsedTime().asSeconds() > czasZmiany) {
            if (pokazTekstura3) {
                sprite.setTexture(tekstura4);
            }
            else {
                sprite.setTexture(tekstura3);
            }
            pokazTekstura3 = !pokazTekstura3;
            zegar.restart();
        }
    }


    float x() { return ksztalt.getPosition().x; }
    float y() { return ksztalt.getPosition().y; }
    float lewo() { return x() - ksztalt.getRadius(); }
    float prawo() { return x() + ksztalt.getRadius(); }
    float gora() { return y() - ksztalt.getRadius(); }
    float dol() { return y() + ksztalt.getRadius(); }


    void rysuj(sf::RenderWindow& window) {
        window.draw(sprite);
    }
};



// Klasa bazowa reprezentuj¹ca prostok¹t (u¿ywana przez paletkê i cegie³ki)
class Prostokat {
public:
    RectangleShape ksztalt;

    // Dodano 'const' na koñcu funkcji
    float x() const { return ksztalt.getPosition().x; }
    float y() const { return ksztalt.getPosition().y; }
    float lewo() const { return x() - ksztalt.getSize().x / 2.f; }
    float prawo() const { return x() + ksztalt.getSize().x / 2.f; }
    float gora() const { return y() - ksztalt.getSize().y / 2.f; }
    float dol() const { return y() + ksztalt.getSize().y / 2.f; }
};

// Klasa reprezentuj¹ca paletkê
class Paletka : public Prostokat {
public:
    Vector2f predkosc;
    Texture tekstura5; // Zmienna do przechowywania tekstury

    Paletka(float x, float y) {
        ksztalt.setPosition(x, y);
        ksztalt.setSize({ szerokoscPaletki, wysokoscPaletki });
        ksztalt.setFillColor(Color::Red);
        // Za³aduj teksturê i przypisz j¹ do obiektu ksztalt
        if (!tekstura5.loadFromFile("paddleRed.png")) {
            // Obs³uga b³êdu, jeœli tekstura siê nie za³aduje
            std::cout << "Nie uda³o siê za³adowaæ tekstury!" << std::endl;
        }
        ksztalt.setTexture(&tekstura5); // Przypisanie tekstury do kszta³tu

        ksztalt.setOrigin(szerokoscPaletki / 2.f, wysokoscPaletki / 2.f);
    }

    // Aktualizacja pozycji paletki na podstawie wciskanych klawiszy
    void aktualizuj() {
        ksztalt.move(predkosc);

        if (Keyboard::isKeyPressed(Keyboard::Key::Left) && lewo() > 0)
            predkosc.x = -predkoscPaletki;
        else if (Keyboard::isKeyPressed(Keyboard::Key::Right) && prawo() < szerokoscOkna)
            predkosc.x = predkoscPaletki;
        else
            predkosc.x = 0;
    }
};


class Cegielka : public Prostokat {
public:
    bool zniszczona{ false };
    bool aktywna{ true };

    // Statyczne zmienne przechowuj¹ce tekstury
    static sf::Texture texture1;
    static sf::Texture texture2;

    // Konstruktor przyjmuj¹cy trzy argumenty: x, y, aktywna
    Cegielka(float x, float y, bool aktywna = true) {
        // Losowanie tekstury
        int randomIndex = rand() % 2;  // Losuj 0 lub 1

        if (randomIndex == 0) {
            ksztalt.setTexture(&texture1);
        }
        else {
            ksztalt.setTexture(&texture2);
        }

        ksztalt.setPosition(x, y);
        ksztalt.setSize({ szerokoscCegielki, wysokoscCegielki });
        ksztalt.setOrigin(szerokoscCegielki / 2.f, wysokoscCegielki / 2.f);

        this->aktywna = aktywna;  // Ustawienie wartoœci aktywnoœci cegie³ki
    }

    // Statyczna funkcja do za³adowania tekstur
    static void loadTextures() {
        if (!texture1.loadFromFile("block.png")) {
            std::cerr << "Nie uda³o siê za³adowaæ block1.png!" << std::endl;
        }
        if (!texture2.loadFromFile("default_block.png")) {
            std::cerr << "Nie uda³o siê za³adowaæ block2.png!" << std::endl;
        }
    }


};

// Inicjalizacja statycznych zmiennych tekstur
sf::Texture Cegielka::texture1;
sf::Texture Cegielka::texture2;




// Funkcja szablonowa do sprawdzania kolizji
template <class T1, class T2>
bool czyKolizja(T1& obiektA, T2& obiektB) {
    return obiektA.prawo() >= obiektB.lewo() && obiektA.lewo() <= obiektB.prawo() &&
        obiektA.dol() >= obiektB.gora() && obiektA.gora() <= obiektB.dol();
}


void testujKolizje(Paletka& paletka, Pilka& pilka) {
    if (!czyKolizja(paletka, pilka)) return;

    pilka.predkosc.y = -predkoscPilki;
    if (pilka.x() < paletka.x())
        pilka.predkosc.x = -predkoscPilki;
    else
        pilka.predkosc.x = predkoscPilki;

    pilka.aktywujAnimacje(); // Aktywuj animacjê po kolizji
}


// Funkcja obs³uguj¹ca kolizjê miêdzy cegie³k¹ a pi³k¹
void testujKolizje(Cegielka& cegielka, Pilka& pilka) {
    if (!czyKolizja(cegielka, pilka)) return;
    cegielka.zniszczona = true;

    float nakladanieLewo{ pilka.prawo() - cegielka.lewo() };
    float nakladaniePrawo{ cegielka.prawo() - pilka.lewo() };
    float nakladanieGora{ pilka.dol() - cegielka.gora() };
    float nakladanieDol{ cegielka.dol() - pilka.gora() };

    bool pilkaZLewej(abs(nakladanieLewo) < abs(nakladaniePrawo));
    bool pilkaZGory(abs(nakladanieGora) < abs(nakladanieDol));

    float minimalneNakladanieX{ pilkaZLewej ? nakladanieLewo : nakladaniePrawo };
    float minimalneNakladanieY{ pilkaZGory ? nakladanieGora : nakladanieDol };

    if (abs(minimalneNakladanieX) < abs(minimalneNakladanieY))
        pilka.predkosc.x = pilkaZLewej ? -predkoscPilki : predkoscPilki;
    else
        pilka.predkosc.y = pilkaZGory ? -predkoscPilki : predkoscPilki;

    pilka.aktywujAnimacje(); // Aktywuj animacjê po kolizji
}

// Klasa do wyœwietlania ekranu "Koniec Gry"
class KoniecGry {
public:
    Text tekstKoniecGry;
    Font czcionka;


    KoniecGry(float szerokosc, float wysokosc) {
        if (!czcionka.loadFromFile("freshface.ttf")) {
            exit(EXIT_FAILURE);
        }

        // £adowanie tekstury t³a
        if (!tloTekstura.loadFromFile("kosmos.png")) {
            std::cerr << "Nie uda³o siê za³adowaæ tekstury t³a!" << std::endl;
            exit(EXIT_FAILURE);
        }

        tloSprite.setTexture(tloTekstura);

        tekstKoniecGry.setFont(czcionka);
        tekstKoniecGry.setString("Koniec Gry");
        tekstKoniecGry.setCharacterSize(100);
        tekstKoniecGry.setFillColor(Color::Red);
        tekstKoniecGry.setPosition(szerokosc / 2 - tekstKoniecGry.getGlobalBounds().width / 2, wysokosc / 6);

        opcje[0].setFont(czcionka);
        opcje[0].setString("Nowa gra");
        opcje[0].setCharacterSize(30);
        opcje[0].setFillColor(Color::Blue);
        opcje[0].setPosition(szerokosc / 2 - opcje[0].getGlobalBounds().width / 2, wysokosc / 2.5);

        opcje[1].setFont(czcionka);
        opcje[1].setString("Powrot do menu");
        opcje[1].setCharacterSize(30);
        opcje[1].setFillColor(Color::Blue);
        opcje[1].setPosition(szerokosc / 2 - opcje[1].getGlobalBounds().width / 2, wysokosc / 2);

        opcje[2].setFont(czcionka);
        opcje[2].setString("Wyjdz");
        opcje[2].setCharacterSize(30);
        opcje[2].setFillColor(Color::Blue);
        opcje[2].setPosition(szerokosc / 2 - opcje[2].getGlobalBounds().width / 2, wysokosc / 1.5);

        wybranaOpcja = 0;

    }

    void rysuj(RenderWindow& okno) {
        // Rysuj t³o
        okno.draw(tloSprite);
        okno.draw(tekstKoniecGry);


        // Rysuj opcje menu
        for (int i = 0; i < MAX_OPCJE; ++i) {
            okno.draw(opcje[i]);
        }
    }



    void ruchWGore() {
        if (wybranaOpcja - 1 >= 0) {
            opcje[wybranaOpcja].setFillColor(Color::Blue);
            wybranaOpcja--;
            opcje[wybranaOpcja].setFillColor(Color::Red);
        }
    }

    void ruchWDol() {
        if (wybranaOpcja + 1 < MAX_OPCJE) {
            opcje[wybranaOpcja].setFillColor(Color::Blue);
            wybranaOpcja++;
            opcje[wybranaOpcja].setFillColor(Color::Red);
        }
    }

    int pobierzWybranaOpcje() {
        return wybranaOpcja;
    }

private:
    static const int MAX_OPCJE = 3;
    int wybranaOpcja;
    Text opcje[MAX_OPCJE];


    // Nowe pola dla t³a
    sf::Texture tloTekstura;
    sf::Sprite tloSprite;
};





// Klasa obs³uguj¹ca menu g³ówne
class Menu {
public:
    Menu(float szerokosc, float wysokosc) {
        if (!czcionka.loadFromFile("freshface.ttf")) {
            exit(EXIT_FAILURE);
        }

        // £adowanie tekstury t³a
        if (!tloTekstura.loadFromFile("banner.png")) {
            std::cerr << "Nie uda³o siê za³adowaæ tekstury t³a!" << std::endl;
            exit(EXIT_FAILURE);
        }

        tloSprite.setTexture(tloTekstura);

        opcje[0].setFont(czcionka);
        opcje[0].setFillColor(Color::Blue);
        opcje[0].setString("Start");
        opcje[0].setPosition(Vector2f(szerokosc / 2 - opcje[0].getGlobalBounds().width / 2, wysokosc / (MAX_OPCJE + 1) * 2));

        opcje[1].setFont(czcionka);
        opcje[1].setFillColor(Color::Blue);
        opcje[1].setString("Pomoc");
        opcje[1].setPosition(Vector2f(szerokosc / 2 - opcje[1].getGlobalBounds().width / 2, wysokosc / (MAX_OPCJE + 1) * 2.5));

        opcje[2].setFont(czcionka);
        opcje[2].setFillColor(Color::Blue);
        opcje[2].setString("Wczytaj gre");
        opcje[2].setPosition(Vector2f(szerokosc / 2 - opcje[2].getGlobalBounds().width / 2, wysokosc / (MAX_OPCJE + 1) * 3));

        opcje[3].setFont(czcionka);
        opcje[3].setFillColor(Color::Blue);
        opcje[3].setString("Wyjdz");
        opcje[3].setPosition(Vector2f(szerokosc / 2 - opcje[3].getGlobalBounds().width / 2, wysokosc / (MAX_OPCJE + 1) * 3.5));
        //------------------------------
        opcje[4].setFont(czcionka);
        opcje[4].setFillColor(Color::Blue);
        opcje[4].setString("Poziom");
        opcje[4].setPosition(Vector2f(szerokosc / 2 - opcje[4].getGlobalBounds().width / 2, wysokosc / (MAX_OPCJE + 1) * 4));
        //-----------------------------

        wybranaOpcja = 0;
    }

    void rysuj(RenderWindow& okno) {
        // Rysuj t³o
        okno.draw(tloSprite);

        // Rysuj opcje menu
        for (int i = 0; i < MAX_OPCJE; ++i) {
            okno.draw(opcje[i]);
        }
    }

    void ruchWGore() {
        if (wybranaOpcja - 1 >= 0) {
            opcje[wybranaOpcja].setFillColor(Color::Blue);
            wybranaOpcja--;
            opcje[wybranaOpcja].setFillColor(Color::Red);
        }
    }

    void ruchWDol() {
        if (wybranaOpcja + 1 < MAX_OPCJE) {
            opcje[wybranaOpcja].setFillColor(Color::Blue);
            wybranaOpcja++;
            opcje[wybranaOpcja].setFillColor(Color::Red);
        }
    }

    int pobierzWybranaOpcje() {
        return wybranaOpcja;
    }

private:
    static const int MAX_OPCJE = 5;
    int wybranaOpcja;
    Text opcje[MAX_OPCJE];
    Font czcionka;

    // Nowe pola dla t³a
    sf::Texture tloTekstura;
    sf::Sprite tloSprite;
};



class Instrukcja {
public:
    // Konstruktor wczytuj¹cy teksturê i tworz¹cy sprite
    Instrukcja(const std::string&) {
        if (!tekstura.loadFromFile("arkanoid_readme_image1.png")) {
            std::cerr << "Nie uda³o siê za³adowaæ obrazka instrukcji!" << std::endl;
        }
        sprite.setTexture(tekstura);  // Przypisanie tekstury do sprite'a
        sprite.setPosition(0, 0);  // Ustawienie pozycji obrazka
    }

    // Funkcja do rysowania instrukcji
    void rysuj(RenderWindow& okno) {
        okno.draw(sprite);  // Rysowanie sprite'a z instrukcj¹
    }

private:
    sf::Texture tekstura;  // Zmienna przechowuj¹ca teksturê
    sf::Sprite sprite;     // Zmienna przechowuj¹ca sprite (do wyœwietlania obrazka)
};

class Instrukcja2 {
public:
    // Konstruktor wczytuj¹cy teksturê i tworz¹cy sprite
    Instrukcja2(const std::string&) {
        if (!tekstura.loadFromFile("arkanoid_readme_image1.png")) {
            std::cerr << "Nie uda³o siê za³adowaæ obrazka instrukcji!" << std::endl;
        }
        sprite.setTexture(tekstura);  // Przypisanie tekstury do sprite'a
        sprite.setPosition(0, 0);  // Ustawienie pozycji obrazka
    }

    // Funkcja do rysowania instrukcji
    void rysuj(RenderWindow& okno) {
        okno.draw(sprite);  // Rysowanie sprite'a z instrukcj¹
    }

private:
    sf::Texture tekstura;  // Zmienna przechowuj¹ca teksturê
    sf::Sprite sprite;     // Zmienna przechowuj¹ca sprite (do wyœwietlania obrazka)
};


class StanGry {
public:
    // Pozycje pi³ki
    float pozycjaPilkiX;
    float pozycjaPilkiY;

    // Pozycje paletki
    float pozycjaPaletkiX;
    float pozycjaPaletkiY;

    // Prêdkoœæ pi³ki
    float predkoscPilkiX;
    float predkoscPilkiY;

    // Czas gry
    float czasGry;  // Czas gry w sekundach

    // Dane cegie³ek (pozycje i aktywnoœæ)
    struct Cegielka {
        float x, y;
        bool aktywna;
    };
    std::vector<Cegielka> cegielki;

    // Funkcja zapisuj¹ca stan gry do pliku
    void zapiszStanDoPliku(const std::string& Nazwapliku) {
        std::ofstream plik(Nazwapliku);
        if (plik.is_open()) {
            // Zapis pi³ki
            plik << pozycjaPilkiX << " " << pozycjaPilkiY << std::endl;
            plik << predkoscPilkiX << " " << predkoscPilkiY << std::endl;

            // Zapis paletki
            plik << pozycjaPaletkiX << " " << pozycjaPaletkiY << std::endl;

            // Zapis cegie³ek
            plik << cegielki.size() << std::endl; // Liczba cegie³ek
            for (const auto& cegielka : cegielki) {
                plik << cegielka.x << " " << cegielka.y << " " << cegielka.aktywna << std::endl;
            }

            // Zapis czasu gry
            plik << czasGry << std::endl;

            plik.close();
            std::cout << "Stan gry zapisany do pliku: " << Nazwapliku << std::endl;
        }
        else {
            std::cerr << "Nie mo¿na otworzyæ pliku do zapisu: " << Nazwapliku << std::endl;
        }
    }

    // Funkcja odczytuj¹ca stan gry z pliku
    void odczytajStanZPliku(const std::string& Nazwapliku) {
        std::ifstream plik(Nazwapliku);
        if (plik.is_open()) {
            // Odczyt pi³ki
            plik >> pozycjaPilkiX >> pozycjaPilkiY;
            plik >> predkoscPilkiX >> predkoscPilkiY;

            // Odczyt paletki
            plik >> pozycjaPaletkiX >> pozycjaPaletkiY;

            // Odczyt cegie³ek
            cegielki.clear();
            size_t liczbaCegielek;
            plik >> liczbaCegielek; // Odczytaj liczbê cegie³ek
            for (size_t i = 0; i < liczbaCegielek; ++i) {
                Cegielka cegielka;
                plik >> cegielka.x >> cegielka.y >> cegielka.aktywna;
                cegielki.push_back(cegielka);
            }

            // Odczyt czasu gry
            plik >> czasGry;

            plik.close();
            std::cout << "Stan gry wczytany z pliku: " << Nazwapliku << std::endl;
        }
        else {
            std::cerr << "Nie mo¿na otworzyæ pliku do odczytu: " << Nazwapliku << std::endl;
        }
    }
};



class Zegar {
public:
    sf::Text tekstCzasu;
    sf::Clock zegar;
    sf::Time czasGry;
    sf::Font czcionka;

    Zegar() {  // Domyœlnie gra trwa
        if (!czcionka.loadFromFile("freshface.ttf")) {
            std::cerr << "Nie uda³o siê za³adowaæ czcionki!" << std::endl;
            exit(EXIT_FAILURE);
        }
        tekstCzasu.setFont(czcionka);
        tekstCzasu.setCharacterSize(24);
        tekstCzasu.setFillColor(sf::Color::White);
        tekstCzasu.setPosition(10, 10);
    }

    void aktualizuj() {
        // Dodaj up³ywaj¹cy czas do ca³kowitego czasu gry, tylko jeœli gra trwa
        czasGry += zegar.restart();

        // Aktualizuj tekst czasu
        tekstCzasu.setString("Czas: " + std::to_string(static_cast<int>(czasGry.asSeconds())));
    }

    void rysuj(sf::RenderWindow& okno) {
        okno.draw(tekstCzasu);
    }

    void resetuj() {
        zegar.restart();       // Restartuje zegar SFML, aby zacz¹³ liczyæ czas od nowa
        czasGry = sf::Time::Zero;  // Zresetowanie czasu gry
    }



    float pobierzCzas() const {
        return czasGry.asSeconds();
    }
};

class Wygrana {
public:
    Text tekstWygrana;
    Text tekstInstrukcja;
    Font czcionka;

    Wygrana(float szerokosc, float wysokosc) {
        if (!czcionka.loadFromFile("freshface.ttf")) {
            exit(EXIT_FAILURE);
        }

        // £adowanie tekstury t³a
        if (!tloTekstura.loadFromFile("kosmos.png")) {
            std::cerr << "Nie uda³o siê za³adowaæ tekstury t³a!" << std::endl;
            exit(EXIT_FAILURE);
        }

        tloSprite.setTexture(tloTekstura);

        // Konfiguracja tekstu "Wygrana"
        tekstWygrana.setFont(czcionka);
        tekstWygrana.setString("Wygrana");
        tekstWygrana.setCharacterSize(100);
        tekstWygrana.setFillColor(Color::Red);
        tekstWygrana.setPosition(szerokosc / 2 - tekstWygrana.getGlobalBounds().width / 2, wysokosc / 4);

        // Konfiguracja tekstu instrukcji
        tekstInstrukcja.setFont(czcionka);
        tekstInstrukcja.setString("Naciœnij Enter, aby wróciæ do menu");
        tekstInstrukcja.setCharacterSize(30);
        tekstInstrukcja.setFillColor(Color::Blue);
        tekstInstrukcja.setPosition(szerokosc / 2 - tekstInstrukcja.getGlobalBounds().width / 2, wysokosc / 2);
    }

    void rysuj(RenderWindow& okno) {
        // Rysuj t³o
        okno.draw(tloSprite);

        // Rysuj teksty
        okno.draw(tekstWygrana);
        okno.draw(tekstInstrukcja);
    }

private:
    sf::Texture tloTekstura;
    sf::Sprite tloSprite;
};


void zresetujStany(bool& graRozpoczeta, bool& koniecGry, bool& pokazInstrukcje, bool& pokazInstrukcje2, bool& wygrana) {
    graRozpoczeta = false;
    koniecGry = false;
    pokazInstrukcje = false;
    pokazInstrukcje2 = false;
    wygrana = false;
}


int main() {

    Cegielka::loadTextures();

    Instrukcja instrukcja("arkanoid_readme_image1.png");
    Instrukcja2 instrukcja2("arkanoid_readme_image1.png");

    Zegar zegar; // Tworzenie zegara gry

    RenderWindow okno(VideoMode(szerokoscOkna, wysokoscOkna), "Arkanoid");

    // Inicjalizacja menu, paletki, pi³ki i cegie³ek
    Menu menu(szerokoscOkna, wysokoscOkna);
    Pilka pilka(szerokoscOkna / 2, wysokoscOkna / 2);
    Paletka paletka(szerokoscOkna / 2, wysokoscOkna - 50);
    std::vector<Cegielka> cegielki;

    for (int iX = 0; iX < liczbaCegielekX; ++iX) {
        for (int iY = 0; iY < liczbaCegielekY; ++iY) {
            cegielki.emplace_back(
                (iX + 1) * (szerokoscCegielki + 3) + 22,
                (iY + 2) * (wysokoscCegielki + 3)
            );
        }
    }

    bool graRozpoczeta = false;
    bool pokazInstrukcje = false;
    bool pokazInstrukcje2 = false;
    bool koniecGry = false;
    bool wygrana = false;
    bool resetGry = false;

    bool poziomWybierany = false;



    KoniecGry ekranKoniecGry(szerokoscOkna, wysokoscOkna);
    Wygrana ekranWygrana(szerokoscOkna, wysokoscOkna);


    //-------------------------------
    WybórPoziomuTrudnoœci wyborPoziomu; // Tworzymy obiekt wyboru poziomu trudnoœci
    PoziomTrudnosci poziomTrudnosci = LATWY; // Domyœlnie ustawiamy poziom na ³atwy

    // Funkcja pomocnicza do generowania cegie³ek
    auto generujCegielki = [&](PoziomTrudnosci poziom) {
        cegielki.clear(); // Wyczyœæ istniej¹ce cegie³ki

        int liczbaCegielekX = 0;
        int liczbaCegielekY = 0;

        // Zmiana liczby cegie³ek w zale¿noœci od poziomu trudnoœci
        switch (poziom) {
        case LATWY:
            liczbaCegielekX = 8;
            liczbaCegielekY = 3;
            break;
        case SREDNI:
            liczbaCegielekX = 10;
            liczbaCegielekY = 4;
            break;
        case TRUDNY:
            liczbaCegielekX = 12;
            liczbaCegielekY = 5;
            break;
        }

        // Generowanie cegie³ek w oparciu o wybrany poziom
        for (int iX = 0; iX < liczbaCegielekX; ++iX) {
            for (int iY = 0; iY < liczbaCegielekY; ++iY) {
                cegielki.emplace_back(
                    (iX + 1) * (szerokoscCegielki + 3) + 22,
                    (iY + 2) * (wysokoscCegielki + 3)
                );
            }
        }
        };

    //-------------------------------

    okno.setFramerateLimit(120);

    while (okno.isOpen()) {
        Event zdarzenie;
        while (okno.pollEvent(zdarzenie)) {
            if (zdarzenie.type == Event::Closed) {
                okno.close();
            }
            if (zdarzenie.type == Event::KeyPressed) {
                if (zdarzenie.key.code == Keyboard::Up) {
                    menu.ruchWGore();
                }
                else if (zdarzenie.key.code == Keyboard::Down) {
                    menu.ruchWDol();
                }
                else if (zdarzenie.key.code == Keyboard::Enter) {
                    int wybranaOpcja = menu.pobierzWybranaOpcje();
                    if (wybranaOpcja == 0) {
                        zegar.resetuj();
                        graRozpoczeta = true;
                        koniecGry = false;
                    }
                    else if (wybranaOpcja == 1) {
                        pokazInstrukcje2 = true;
                    }
                    else if (wybranaOpcja == 3) {
                        okno.close();

                    }
                    else if (wybranaOpcja == 2) {

                        zegar.resetuj();
                        StanGry stanGry;
                        stanGry.odczytajStanZPliku("stan_gry.txt");

                        // Przywracanie stanu cegie³ek
                        cegielki.clear();
                        for (const auto& stanCegielki : stanGry.cegielki) {
                            cegielki.emplace_back(
                                stanCegielki.x,
                                stanCegielki.y,
                                stanCegielki.aktywna
                            );
                        }

                        pilka.ksztalt.setPosition(stanGry.pozycjaPilkiX, stanGry.pozycjaPilkiY);
                        pilka.predkosc = { stanGry.predkoscPilkiX, stanGry.predkoscPilkiY };
                        paletka.ksztalt.setPosition(stanGry.pozycjaPaletkiX, stanGry.pozycjaPaletkiY);

                        // Ustawienie czasu gry z pliku
                        zegar.czasGry = sf::seconds(stanGry.czasGry);  // Odczytaj czas i przypisz do zegara

                        graRozpoczeta = true;



                    }
                    //-----------------------------------------
                    else if (wybranaOpcja == 4) {
                        poziomWybierany = true;
                    }
                    //-----------------------------------------------
                }
                //---------------------------------------------
                else if (zdarzenie.key.code == Keyboard::Escape) {
                    if (/*graRozpoczeta ||*/ pokazInstrukcje2 /*|| koniecGry*/) {
                        graRozpoczeta = false;
                        pokazInstrukcje2 = false;
                        koniecGry = false;
                    }
                }

                else if (zdarzenie.key.code == Keyboard::F1) {
                    if (pokazInstrukcje) {
                        // Jeœli instrukcja jest w³¹czona, ukryj j¹ i wznowij grê
                        pokazInstrukcje = false;
                        graRozpoczeta = true;
                    }
                    else if (graRozpoczeta) {
                        // Jeœli gra jest aktywna, wstrzymaj j¹ i poka¿ instrukcjê
                        pokazInstrukcje = true;
                        graRozpoczeta = false;
                    }
                }
            }

            if (zdarzenie.type == sf::Event::KeyPressed && zdarzenie.key.code == sf::Keyboard::F2) {
                // Przygotowanie stanu gry do zapisania
                StanGry stanGry;
                stanGry.pozycjaPilkiX = pilka.x();
                stanGry.pozycjaPilkiY = pilka.y();
                stanGry.pozycjaPaletkiX = paletka.x();
                stanGry.pozycjaPaletkiY = paletka.y();
                stanGry.predkoscPilkiX = pilka.predkosc.x;
                stanGry.predkoscPilkiY = pilka.predkosc.y;

                // Zapisanie aktualnego czasu gry
                stanGry.czasGry = zegar.pobierzCzas();

                // Zapisanie stanu cegie³ek
                stanGry.cegielki.clear();
                for (const auto& cegielka : cegielki) {
                    StanGry::Cegielka stanCegielki = {
                        cegielka.x(),
                        cegielka.y(),
                        cegielka.aktywna
                    };
                    stanGry.cegielki.push_back(stanCegielki);
                }

                // Zapis stanu gry do pliku
                stanGry.zapiszStanDoPliku("stan_gry.txt");
                std::cout << "Stan gry zapisany do pliku stan_gry.txt" << std::endl;
            }


            if (koniecGry) {
                if (zdarzenie.type == Event::KeyPressed) {
                    if (zdarzenie.key.code == Keyboard::Up) {
                        ekranKoniecGry.ruchWGore();
                    }
                    else if (zdarzenie.key.code == Keyboard::Down) {
                        ekranKoniecGry.ruchWDol();
                    }
                    else if (zdarzenie.key.code == Keyboard::Enter) {
                        int wybranaOpcja = ekranKoniecGry.pobierzWybranaOpcje();
                        if (wybranaOpcja == 0) {
                            zegar.resetuj();
                            graRozpoczeta = true;
                            koniecGry = false;
                            pokazInstrukcje = false;
                            pokazInstrukcje2 = false;
                            //koniecGry = false;
                        }
                        else if (wybranaOpcja == 1) {
                            //powrot do menu
                            graRozpoczeta = false;
                            koniecGry = false;
                            pokazInstrukcje = false;
                            pokazInstrukcje2 = false;
                        }
                        else if (wybranaOpcja == 2) {
                            okno.close();

                        }
                    }

                }
            }
            if (poziomWybierany) {
                if (zdarzenie.type == sf::Event::KeyPressed) {
                    if (zdarzenie.key.code == sf::Keyboard::Up) {
                        wyborPoziomu.ruchWGore();
                    }
                    else if (zdarzenie.key.code == sf::Keyboard::Down) {
                        wyborPoziomu.ruchWDol();
                    }
                    else if (zdarzenie.key.code == sf::Keyboard::Enter) {
                        // Cyclicznie zmieniamy poziom trudnoœci
                        if (poziomTrudnosci == LATWY) {
                            poziomTrudnosci = SREDNI;
                            std::cout << "Wybrano poziom trudnosci: Sredni" << std::endl;
                        }
                        else if (poziomTrudnosci == SREDNI) {
                            poziomTrudnosci = TRUDNY;
                            std::cout << "Wybrano poziom trudnosci: Trudny" << std::endl;
                        }
                        else if (poziomTrudnosci == TRUDNY) {
                            poziomTrudnosci = LATWY;
                            std::cout << "Wybrano poziom trudnosci: Latwy" << std::endl;
                        }

                        // Ustawienie liczby cegie³ek w zale¿noœci od poziomu trudnoœci
                        generujCegielki(poziomTrudnosci);  // Funkcja generuj¹ca cegie³ki

                        // Zatwierdzenie wyboru poziomu trudnoœci i przejœcie do kolejnej czêœci gry
                        poziomWybierany = false; // Zatwierdzamy wybór poziomu trudnoœci
                    }
                }
            }




            if (cegielki.empty()) {
                wygrana = true;
                graRozpoczeta = false;
                pokazInstrukcje = false;
                pokazInstrukcje2 = false;
                koniecGry = false;
            }


            if (wygrana) {
                if (zdarzenie.type == sf::Event::KeyPressed && zdarzenie.key.code == sf::Keyboard::Enter) {
                    // Zresetuj flagi
                    wygrana = false;
                    graRozpoczeta = false;
                    resetGry = true;
                    koniecGry = false;


                }
                zegar.resetuj();
                // Przegrana
                pilka.ksztalt.setPosition(szerokoscOkna / 2, wysokoscOkna / 2);
                pilka.predkosc = { -predkoscPilki, -predkoscPilki };
                paletka.ksztalt.setPosition(szerokoscOkna / 2, wysokoscOkna - 50);
                cegielki.clear();

                for (int iX = 0; iX < liczbaCegielekX; ++iX) {
                    for (int iY = 0; iY < liczbaCegielekY; ++iY) {
                        cegielki.emplace_back(
                            (iX + 1) * (szerokoscCegielki + 3) + 22,
                            (iY + 2) * (wysokoscCegielki + 3)
                        );
                    }
                }
            }




        }


        okno.clear(Color::Black);

        if (pokazInstrukcje) {

            instrukcja.rysuj(okno);
        }

        else if (pokazInstrukcje2) {

            instrukcja2.rysuj(okno);
        }

        else if (wygrana) {
            ekranWygrana.rysuj(okno);
        }

        else if (koniecGry) {
            zegar.resetuj();
            ekranKoniecGry.rysuj(okno);
        }
        else if (resetGry) {

            menu.rysuj(okno);
            wygrana = false;
            graRozpoczeta = false;
            resetGry = false;
            koniecGry = false;


        }

        if (poziomWybierany) {
            wyborPoziomu.rysuj(okno);
        }
        //else {
        //    // Tutaj mo¿esz rozpocz¹æ grê z wybranym poziomem trudnoœci
        //    // Na przyk³ad zmieniaj¹c prêdkoœæ pi³ki, liczbê cegie³ek, itp.
        //}


        else if (!graRozpoczeta && !koniecGry) {
            menu.rysuj(okno);
        }

        else if (graRozpoczeta) {
            pilka.aktualizuj();
            paletka.aktualizuj();
            zegar.aktualizuj();

            testujKolizje(paletka, pilka);

            for (auto& cegielka : cegielki) {
                testujKolizje(cegielka, pilka);
            }

            cegielki.erase(
                std::remove_if(cegielki.begin(), cegielki.end(),
                    [](const Cegielka& cegielka) { return cegielka.zniszczona; }),
                cegielki.end()
            );

            if (pilka.dol() > wysokoscOkna) {

                // Przegrana
                pilka.ksztalt.setPosition(szerokoscOkna / 2, wysokoscOkna / 2);
                pilka.predkosc = { -predkoscPilki, -predkoscPilki };
                paletka.ksztalt.setPosition(szerokoscOkna / 2, wysokoscOkna - 50);
                cegielki.clear();

                for (int iX = 0; iX < liczbaCegielekX; ++iX) {
                    for (int iY = 0; iY < liczbaCegielekY; ++iY) {
                        cegielki.emplace_back(
                            (iX + 1) * (szerokoscCegielki + 3) + 22,
                            (iY + 2) * (wysokoscCegielki + 3)
                        );
                    }
                }
                graRozpoczeta = false;
                koniecGry = true;



            }
            else {
                pilka.rysuj(okno);
                okno.draw(paletka.ksztalt);
                zegar.rysuj(okno);
                for (const auto& cegielka : cegielki) {
                    okno.draw(cegielka.ksztalt);
                }
            }
        }

        okno.display();
    }

    return 0;
}

