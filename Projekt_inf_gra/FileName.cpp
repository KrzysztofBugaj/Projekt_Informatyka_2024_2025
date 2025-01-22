#include <SFML/Graphics.hpp>
#include <vector>
#include <fstream>
#include <iostream>
#include <ctime>
#include <cstdlib>


using namespace sf;

//Definicje sta³ych
constexpr int szerokoscOkna{ 800 }, wysokoscOkna{ 600 };
constexpr float promienPilki{ 10.f }, predkoscPilki{ 3.f };
constexpr float szerokoscPaletki{ 150.f }, wysokoscPaletki{ 20.f }, predkoscPaletki{ 10.0f };
constexpr float szerokoscCegielki{ 60.f }, wysokoscCegielki{ 20.f };
constexpr int liczbaCegielekX{ 11 }, liczbaCegielekY{ 4 };

constexpr float szerokoscCegly{ 240.f }, wysokoscCegly{ 20.f };
constexpr int liczbaCeglyX{ 1 }, liczbaCeglyY{ 1 };



enum PoziomTrudnosci { LATWY, SREDNI, TRUDNY };


class Pilka {
public:
    CircleShape ksztalt;
    Vector2f predkosc{ -predkoscPilki, -predkoscPilki };

    sf::Texture tekstura3, tekstura4, teksturaAnimacji;
    sf::Sprite sprite;
    bool pokazTekstura3 = true;
    bool czyAnimacjaAktywna = false;
    sf::Clock zegar, zegarAnimacji;
    float czasZmiany = 0.5f;
    float czasAnimacji = 0.2f;

    Pilka(float x, float y) {
        ksztalt.setPosition(x, y);
        ksztalt.setRadius(promienPilki);
        ksztalt.setFillColor(Color::Red);
        ksztalt.setOrigin(promienPilki, promienPilki);

        if (!tekstura3.loadFromFile("ballBlue.png") ||
            !tekstura4.loadFromFile("ballGrey.png") ||
            !teksturaAnimacji.loadFromFile("wybuch.png")) {
            std::cerr << "Nie uda³o siê za³adowaæ tekstur!" << std::endl;
            exit(EXIT_FAILURE);
        }

        sprite.setTexture(tekstura3);
        sprite.setPosition(x, y);
    }

    void aktywujAnimacje() {
        czyAnimacjaAktywna = true;
        zegarAnimacji.restart();
        sprite.setTexture(teksturaAnimacji);
        sprite.setOrigin(teksturaAnimacji.getSize().x / 2.0f, teksturaAnimacji.getSize().y / 2.0f);
        sprite.setScale(
            2.0f * promienPilki / teksturaAnimacji.getSize().x,
            2.0f * promienPilki / teksturaAnimacji.getSize().y

        );

        sprite.setPosition(ksztalt.getPosition());

    }

    void aktualizuj() {
        ksztalt.move(predkosc);
        sprite.setPosition(ksztalt.getPosition());

        if (czyAnimacjaAktywna) {
            if (zegarAnimacji.getElapsedTime().asSeconds() > czasAnimacji) {
                czyAnimacjaAktywna = false;
                sprite.setTexture(pokazTekstura3 ? tekstura3 : tekstura4);
                sprite.setScale(1.0f, 1.0f);
                sprite.setOrigin(promienPilki, promienPilki);
            }
            /*           return;*/
        }

        if (lewo() < 0)
            predkosc.x = predkoscPilki;
        else if (prawo() > szerokoscOkna)
            predkosc.x = -predkoscPilki;

        if (gora() < 0)
            predkosc.y = predkoscPilki;
        else if (dol() > wysokoscOkna)
            predkosc.y = -predkoscPilki;

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



class Prostokat {
public:
    RectangleShape ksztalt;

    float x() const { return ksztalt.getPosition().x; }
    float y() const { return ksztalt.getPosition().y; }
    float lewo() const { return x() - ksztalt.getSize().x / 2.f; }
    float prawo() const { return x() + ksztalt.getSize().x / 2.f; }
    float gora() const { return y() - ksztalt.getSize().y / 2.f; }
    float dol() const { return y() + ksztalt.getSize().y / 2.f; }
};

class Paletka : public Prostokat {
public:
    Vector2f predkosc;
    Texture tekstura5;

    Paletka(float x, float y) {
        ksztalt.setPosition(x, y);
        ksztalt.setSize({ szerokoscPaletki, wysokoscPaletki });
        ksztalt.setFillColor(Color::Red);

        if (!tekstura5.loadFromFile("paddleRed.png")) {

            std::cout << "Nie uda³o siê za³adowaæ tekstury!" << std::endl;
        }
        ksztalt.setTexture(&tekstura5);

        ksztalt.setOrigin(szerokoscPaletki / 2.f, wysokoscPaletki / 2.f);
    }

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

    static sf::Texture texture1;
    static sf::Texture texture2;

    Cegielka(float x, float y, bool aktywna = true) {
        int randomIndex = rand() % 2;

        if (randomIndex == 0) {
            ksztalt.setTexture(&texture1);
        }
        else {
            ksztalt.setTexture(&texture2);
        }

        ksztalt.setPosition(x, y);
        ksztalt.setSize({ szerokoscCegielki, wysokoscCegielki });
        ksztalt.setOrigin(szerokoscCegielki / 2.f, wysokoscCegielki / 2.f);

        this->aktywna = aktywna;
    }

    static void loadTextures() {
        if (!texture1.loadFromFile("block.png")) {
            std::cerr << "Nie uda³o siê za³adowaæ block1.png!" << std::endl;
        }
        if (!texture2.loadFromFile("default_block.png")) {
            std::cerr << "Nie uda³o siê za³adowaæ block2.png!" << std::endl;
        }
    }


};

sf::Texture Cegielka::texture1;
sf::Texture Cegielka::texture2;


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

    pilka.aktywujAnimacje();
}

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

    pilka.aktywujAnimacje();
}

class KoniecGry {
public:
    Text tekstKoniecGry;
    Font czcionka;


    KoniecGry(float szerokosc, float wysokosc) {
        if (!czcionka.loadFromFile("freshface.ttf")) {
            exit(EXIT_FAILURE);
        }

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
        okno.draw(tloSprite);
        okno.draw(tekstKoniecGry);

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


    sf::Texture tloTekstura;
    sf::Sprite tloSprite;
};


class Menu {
public:
    Menu(float szerokosc, float wysokosc) {
        if (!czcionka.loadFromFile("freshface.ttf")) {
            exit(EXIT_FAILURE);
        }

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

        opcje[4].setFont(czcionka);
        opcje[4].setFillColor(Color::Blue);
        opcje[4].setString("Wyjdz");
        opcje[4].setPosition(Vector2f(szerokosc / 2 - opcje[4].getGlobalBounds().width / 2, wysokosc / (MAX_OPCJE + 1) * 4));
        //------------------------------
        opcje[3].setFont(czcionka);
        opcje[3].setFillColor(Color::Blue);
        opcje[3].setString("Poziom");
        opcje[3].setPosition(Vector2f(szerokosc / 2 - opcje[3].getGlobalBounds().width / 2, wysokosc / (MAX_OPCJE + 1) * 3.5));
        //-----------------------------

        wybranaOpcja = 0;
    }

    void rysuj(RenderWindow& okno) {

        okno.draw(tloSprite);

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


    sf::Texture tloTekstura;
    sf::Sprite tloSprite;
};



class Instrukcja {
public:

    Instrukcja(const std::string&) {
        if (!tekstura.loadFromFile("arkanoid_readme_image1.png")) {
            std::cerr << "Nie uda³o siê za³adowaæ obrazka instrukcji!" << std::endl;
        }
        sprite.setTexture(tekstura);
        sprite.setPosition(0, 0);
    }

    void rysuj(RenderWindow& okno) {
        okno.draw(sprite);
    }

private:
    sf::Texture tekstura;
    sf::Sprite sprite;
};

class Instrukcja2 {
public:

    Instrukcja2(const std::string&) {
        if (!tekstura.loadFromFile("arkanoid_readme_image1.png")) {
            std::cerr << "Nie uda³o siê za³adowaæ obrazka instrukcji!" << std::endl;
        }
        sprite.setTexture(tekstura);
        sprite.setPosition(0, 0);
    }

    void rysuj(RenderWindow& okno) {
        okno.draw(sprite);
    }

private:
    sf::Texture tekstura;
    sf::Sprite sprite;
};


class StanGry {
public:

    float pozycjaPaletkiX;
    float pozycjaPaletkiY;

    float czasGry;

    struct Cegielka {
        float x, y;
        bool aktywna;
    };
    std::vector<Cegielka> cegielki;


    void zapiszStanDoPliku(const std::string& Nazwapliku) {
        std::ofstream plik(Nazwapliku);
        if (plik.is_open()) {
            ;

            plik << pozycjaPaletkiX << " " << pozycjaPaletkiY << std::endl;

            plik << cegielki.size() << std::endl;
            for (const auto& cegielka : cegielki) {
                plik << cegielka.x << " " << cegielka.y << " " << cegielka.aktywna << std::endl;
            }

            plik << czasGry << std::endl;

            plik.close();
            std::cout << "Stan gry zapisany do pliku: " << Nazwapliku << std::endl;
        }
        else {
            std::cerr << "Nie mo¿na otworzyæ pliku do zapisu: " << Nazwapliku << std::endl;
        }
    }

    void odczytajStanZPliku(const std::string& Nazwapliku) {
        std::ifstream plik(Nazwapliku);
        if (plik.is_open()) {

            plik >> pozycjaPaletkiX >> pozycjaPaletkiY;

            cegielki.clear();
            size_t liczbaCegielek;
            plik >> liczbaCegielek;
            for (size_t i = 0; i < liczbaCegielek; ++i) {
                Cegielka cegielka;
                plik >> cegielka.x >> cegielka.y >> cegielka.aktywna;
                cegielki.push_back(cegielka);
            }

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

    Zegar() {
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

        czasGry += zegar.restart();

        tekstCzasu.setString("Czas: " + std::to_string(static_cast<int>(czasGry.asSeconds())));
    }

    void rysuj(sf::RenderWindow& okno) {
        okno.draw(tekstCzasu);
    }

    void resetuj() {
        zegar.restart();
        czasGry = sf::Time::Zero;
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

        if (!tloTekstura.loadFromFile("kosmos.png")) {
            std::cerr << "Nie uda³o siê za³adowaæ tekstury t³a!" << std::endl;
            exit(EXIT_FAILURE);
        }

        tloSprite.setTexture(tloTekstura);

        tekstWygrana.setFont(czcionka);
        tekstWygrana.setString("Wygrana");
        tekstWygrana.setCharacterSize(100);
        tekstWygrana.setFillColor(Color::Red);
        tekstWygrana.setPosition(szerokosc / 2 - tekstWygrana.getGlobalBounds().width / 2, wysokosc / 4);

        tekstInstrukcja.setFont(czcionka);
        tekstInstrukcja.setString("Naciœnij Enter, aby wróciæ do menu");
        tekstInstrukcja.setCharacterSize(30);
        tekstInstrukcja.setFillColor(Color::Blue);
        tekstInstrukcja.setPosition(szerokosc / 2 - tekstInstrukcja.getGlobalBounds().width / 2, wysokosc / 2);
    }

    void rysuj(RenderWindow& okno) {

        okno.draw(tloSprite);
        okno.draw(tekstWygrana);
        okno.draw(tekstInstrukcja);
    }

private:
    sf::Texture tloTekstura;
    sf::Sprite tloSprite;
};

class Opuscprogram {
public:
    Text tekstOpuscprogram;
    Text tekstInstrukcja;
    Font czcionka;

    Opuscprogram(float szerokosc, float wysokosc) {
        if (!czcionka.loadFromFile("freshface.ttf")) {
            exit(EXIT_FAILURE);
        }

        if (!tloTekstura.loadFromFile("kosmos.png")) {
            std::cerr << "Nie uda³o siê za³adowaæ tekstury t³a!" << std::endl;
            exit(EXIT_FAILURE);
        }

        tloSprite.setTexture(tloTekstura);

        tekstOpuscprogram.setFont(czcionka);
        tekstOpuscprogram.setString("Chcesz wyjsc z gry?");
        tekstOpuscprogram.setCharacterSize(100);
        tekstOpuscprogram.setFillColor(Color::Red);
        tekstOpuscprogram.setPosition(szerokosc / 2 - tekstOpuscprogram.getGlobalBounds().width / 2, wysokosc / 4);

        tekstInstrukcja.setFont(czcionka);
        tekstInstrukcja.setString("Tak - kliknij T / Nie - kliknij Escape");
        tekstInstrukcja.setCharacterSize(30);
        tekstInstrukcja.setFillColor(Color::Blue);
        tekstInstrukcja.setPosition(szerokosc / 2 - tekstInstrukcja.getGlobalBounds().width / 2, wysokosc / 2);
    }

    void rysuj(RenderWindow& okno) {

        okno.draw(tloSprite);
        okno.draw(tekstOpuscprogram);
        okno.draw(tekstInstrukcja);
    }

private:
    sf::Texture tloTekstura;
    sf::Sprite tloSprite;
};


class Cegla : public Prostokat {
public:

    static sf::Texture texture1;


    Cegla(float x, float y) {

        ksztalt.setTexture(&texture1);


        ksztalt.setPosition(x, y);
        ksztalt.setSize({ szerokoscCegly, wysokoscCegly });
        ksztalt.setOrigin(szerokoscCegly / 2.f, wysokoscCegly / 2.f);

    }

    static void loadTextures() {
        if (!texture1.loadFromFile("platform.png")) {
            std::cerr << "Nie uda³o siê za³adowaæ block1.png!" << std::endl;
        }
    }
    void rysuj(sf::RenderWindow& okno) {
        okno.draw(ksztalt);
    }
};


sf::Texture Cegla::texture1;


void testujKolizje(Cegla& cegla, Pilka& pilka) {
    if (!czyKolizja(cegla, pilka)) return;

    float nakladanieLewo{ pilka.prawo() - cegla.lewo() };
    float nakladaniePrawo{ cegla.prawo() - pilka.lewo() };
    float nakladanieGora{ pilka.dol() - cegla.gora() };
    float nakladanieDol{ cegla.dol() - pilka.gora() };

    bool pilkaZLewej(abs(nakladanieLewo) < abs(nakladaniePrawo));
    bool pilkaZGory(abs(nakladanieGora) < abs(nakladanieDol));

    float minimalneNakladanieX{ pilkaZLewej ? nakladanieLewo : nakladaniePrawo };
    float minimalneNakladanieY{ pilkaZGory ? nakladanieGora : nakladanieDol };

    if (abs(minimalneNakladanieX) < abs(minimalneNakladanieY))
        pilka.predkosc.x = pilkaZLewej ? -predkoscPilki : predkoscPilki;
    else
        pilka.predkosc.y = pilkaZGory ? -predkoscPilki : predkoscPilki;

    pilka.aktywujAnimacje();
}


int main() {

    Cegielka::loadTextures();
    Cegla::loadTextures();

    Instrukcja instrukcja("arkanoid_readme_image1.png");
    Instrukcja2 instrukcja2("arkanoid_readme_image1.png");

    Zegar zegar;

    RenderWindow okno(VideoMode(szerokoscOkna, wysokoscOkna), "Arkanoid");

    Menu menu(szerokoscOkna, wysokoscOkna);
    Pilka pilka(szerokoscOkna / 2, wysokoscOkna / 2);
    Paletka paletka(szerokoscOkna / 2, wysokoscOkna - 50);
    KoniecGry ekranKoniecGry(szerokoscOkna, wysokoscOkna);
    Wygrana ekranWygrana(szerokoscOkna, wysokoscOkna);

    Opuscprogram opuscProgram(szerokoscOkna, wysokoscOkna);


    std::vector<Cegielka> cegielki;


    bool graRozpoczeta = false;
    bool pokazInstrukcje = false;
    bool pokazInstrukcje2 = false;
    bool koniecGry = false;
    bool wygrana = false;
    bool poziomWybierany = false;
    bool ustawpoziom = false;

    bool taknie = false;



    PoziomTrudnosci poziomTrudnosci = LATWY;

    std::vector<Cegla> cegly;
    cegly.emplace_back(200.f, 200.f);
    cegly.emplace_back(600.f, 250.f);


    auto generujCegielki = [&](PoziomTrudnosci poziom) {
        cegielki.clear();

        int liczbaCegielekX = 0;
        int liczbaCegielekY = 0;

        switch (poziom) {
        case LATWY:
            liczbaCegielekX = 11;
            liczbaCegielekY = 3;
            break;
        case SREDNI:
            liczbaCegielekX = 11;
            liczbaCegielekY = 4;
            break;
        case TRUDNY:
            liczbaCegielekX = 11;
            liczbaCegielekY = 5;
            break;
        }

        for (int iX = 0; iX < liczbaCegielekX; ++iX) {
            for (int iY = 0; iY < liczbaCegielekY; ++iY) {
                cegielki.emplace_back(
                    (iX + 1) * (szerokoscCegielki + 3) + 22,
                    (iY + 2) * (wysokoscCegielki + 3)
                );
            }
        }
        };

    generujCegielki(poziomTrudnosci);

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
                        generujCegielki(poziomTrudnosci);
                    }
                    else if (wybranaOpcja == 1) {
                        pokazInstrukcje2 = true;
                    }
                    else if (wybranaOpcja == 4) {
                        okno.close();

                    }
                    else if (wybranaOpcja == 2) {

                        zegar.resetuj();
                        StanGry stanGry;
                        stanGry.odczytajStanZPliku("stan_gry.txt");

                        cegielki.clear();
                        for (const auto& stanCegielki : stanGry.cegielki) {
                            cegielki.emplace_back(
                                stanCegielki.x,
                                stanCegielki.y,
                                stanCegielki.aktywna
                            );
                        }

                        paletka.ksztalt.setPosition(stanGry.pozycjaPaletkiX, stanGry.pozycjaPaletkiY);

                        zegar.czasGry = sf::seconds(stanGry.czasGry);

                        graRozpoczeta = true;

                    }

                    else if (wybranaOpcja == 3) {
                        poziomWybierany = true;
                    }

                }
                if (zdarzenie.type == sf::Event::KeyPressed && zdarzenie.key.code == sf::Keyboard::Escape) {
                    if (pokazInstrukcje2) {
                        graRozpoczeta = false;
                        koniecGry = false;
                        pokazInstrukcje2 = false;
                    }
                }

                else if (zdarzenie.key.code == Keyboard::F1) {
                    if (pokazInstrukcje) {
                        pokazInstrukcje = false;
                        graRozpoczeta = true;
                    }
                    else if (graRozpoczeta) {
                        pokazInstrukcje = true;
                        graRozpoczeta = false;
                    }
                }
                if (zdarzenie.type == sf::Event::KeyPressed && zdarzenie.key.code == sf::Keyboard::Escape) {
                    if (taknie) {
                        taknie = false;
                        graRozpoczeta = true;
                    }
                    else if (graRozpoczeta) {
                        taknie = true;
                        graRozpoczeta = false;
                    }
                }
                else if (zdarzenie.key.code == Keyboard::T) {
                    if (taknie) {
                        okno.close();
                    }
                }
            }

            if (zdarzenie.type == sf::Event::KeyPressed && zdarzenie.key.code == sf::Keyboard::F2) {
                StanGry stanGry;

                stanGry.pozycjaPaletkiX = paletka.x();
                stanGry.pozycjaPaletkiY = paletka.y();

                stanGry.czasGry = zegar.pobierzCzas();

                stanGry.cegielki.clear();
                for (const auto& cegielka : cegielki) {
                    StanGry::Cegielka stanCegielki = {
                        cegielka.x(),
                        cegielka.y(),
                        cegielka.aktywna
                    };
                    stanGry.cegielki.push_back(stanCegielki);
                }

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
                        }
                        else if (wybranaOpcja == 1) {
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
                    if (zdarzenie.key.code == sf::Keyboard::Enter) {

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

                        generujCegielki(poziomTrudnosci);


                        poziomWybierany = false;
                    }
                }
            }



            if (cegielki.empty()) {
                wygrana = true;
                graRozpoczeta = false;
                koniecGry = false;
            }


            if (wygrana) {
                if (zdarzenie.type == sf::Event::KeyPressed && zdarzenie.key.code == sf::Keyboard::Enter) {
                    wygrana = false;
                    graRozpoczeta = false;
                    koniecGry = false;

                }
                zegar.resetuj();
                pilka.ksztalt.setPosition(szerokoscOkna / 2, wysokoscOkna / 2);
                pilka.predkosc = { -predkoscPilki, -predkoscPilki };
                paletka.ksztalt.setPosition(szerokoscOkna / 2, wysokoscOkna - 50);
                cegielki.clear();

                generujCegielki(poziomTrudnosci);
            }

        }


        okno.clear(Color::Black);

        if (pokazInstrukcje) {

            instrukcja.rysuj(okno);
        }

        else if (pokazInstrukcje2) {

            instrukcja2.rysuj(okno);
        }

        else if (taknie) {
            opuscProgram.rysuj(okno);
        }

        else if (wygrana) {
            ekranWygrana.rysuj(okno);
        }

        else if (koniecGry) {
            zegar.resetuj();
            ekranKoniecGry.rysuj(okno);
        }

        else if (!graRozpoczeta && !koniecGry) {
            menu.rysuj(okno);
        }

        else if (graRozpoczeta) {

            for (auto& cegla : cegly) {
                testujKolizje(cegla, pilka);
            }

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

                pilka.ksztalt.setPosition(szerokoscOkna / 2, wysokoscOkna / 2);
                pilka.predkosc = { -predkoscPilki, -predkoscPilki };
                paletka.ksztalt.setPosition(szerokoscOkna / 2, wysokoscOkna - 50);
                cegielki.clear();

                generujCegielki(poziomTrudnosci);

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

                for (const auto& cegla : cegly) {
                    okno.draw(cegla.ksztalt);
                }

            }
        }

        okno.display();
    }

    return 0;
}

