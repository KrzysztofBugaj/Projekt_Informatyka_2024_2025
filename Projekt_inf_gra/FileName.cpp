#include <SFML/Graphics.hpp>
#include <vector>
#include <fstream>
#include <iostream>
#include <ctime>
#include <cstdlib>

//-------------------------------------------------------
using namespace sf;

// Definicje sta³ych
constexpr int szerokoscOkna{ 800 }, wysokoscOkna{ 600 };
constexpr float promienPilki{ 10.f }, predkoscPilki{ 8.f };
constexpr float szerokoscPaletki{ 60.f }, wysokoscPaletki{ 20.f }, predkoscPaletki{ 6.f };
constexpr float szerokoscCegielki{ 60.f }, wysokoscCegielki{ 20.f };
constexpr int liczbaCegielekX{ 11 }, liczbaCegielekY{ 4 };

// Klasa reprezentuj¹ca pi³kê
class Pilka {
public:
    CircleShape ksztalt;
    Vector2f predkosc{ -predkoscPilki, -predkoscPilki };

    Pilka(float x, float y) {
        ksztalt.setPosition(x, y);
        ksztalt.setRadius(promienPilki);
        ksztalt.setFillColor(Color::Red);
        ksztalt.setOrigin(promienPilki, promienPilki);
    }

    // Aktualizacja pozycji pi³ki
    void aktualizuj() {
        ksztalt.move(predkosc);

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
    }

    // Funkcje pomocnicze do uzyskania pozycji krawêdzi pi³ki
    float x() { return ksztalt.getPosition().x; }
    float y() { return ksztalt.getPosition().y; }
    float lewo() { return x() - ksztalt.getRadius(); }
    float prawo() { return x() + ksztalt.getRadius(); }
    float gora() { return y() - ksztalt.getRadius(); }
    float dol() { return y() + ksztalt.getRadius(); }
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

    Paletka(float x, float y) {
        ksztalt.setPosition(x, y);
        ksztalt.setSize({ szerokoscPaletki, wysokoscPaletki });
        ksztalt.setFillColor(Color::Red);
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

// Funkcja obs³uguj¹ca kolizjê miêdzy paletk¹ a pi³k¹
void testujKolizje(Paletka& paletka, Pilka& pilka) {
    if (!czyKolizja(paletka, pilka)) return;

    pilka.predkosc.y = -predkoscPilki;
    if (pilka.x() < paletka.x())
        pilka.predkosc.x = -predkoscPilki;
    else
        pilka.predkosc.x = predkoscPilki;
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
    static const int MAX_OPCJE = 4;
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
    Instrukcja(const std::string& ) {
        if (!tekstura.loadFromFile("arkanoid_readme_image1.png")) {
            std::cerr << "Nie uda³o siê za³adowaæ obrazka instrukcji!" << std::endl;
        }
        sprite.setTexture(tekstura);  // Przypisanie tekstury do sprite'a
        sprite.setPosition(0,0);  // Ustawienie pozycji obrazka
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

            plik.close();
            std::cout << "Stan gry wczytany z pliku: " << Nazwapliku << std::endl;
        }
        else {
            std::cerr << "Nie mo¿na otworzyæ pliku do odczytu: " << Nazwapliku << std::endl;
        }
    }
};


int main() {

    Cegielka::loadTextures();

    Instrukcja instrukcja("arkanoid_readme_image1.png");
    Instrukcja2 instrukcja2("arkanoid_readme_image1.png");

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

    KoniecGry ekranKoniecGry(szerokoscOkna, wysokoscOkna);

    okno.setFramerateLimit(60);

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
                        graRozpoczeta = true;
                        //koniecGry = false;
                    }
                    else if (wybranaOpcja == 1) {
                        pokazInstrukcje2 = true;
                    }
                    else if (wybranaOpcja == 3) {
                        okno.close();
                    }
                    else if (wybranaOpcja == 2) {


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


                        graRozpoczeta = true;



                    }
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
        }


        okno.clear(Color::Black);

        if (pokazInstrukcje) {

            instrukcja.rysuj(okno);
        }

        else if (pokazInstrukcje2) {

            instrukcja2.rysuj(okno);
        }

        else if (!graRozpoczeta && !koniecGry) {
            menu.rysuj(okno);
        }
        else if (koniecGry) {
            ekranKoniecGry.rysuj(okno);
        }
        else {
            pilka.aktualizuj();
            paletka.aktualizuj();

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


                StanGry stanGry;
                stanGry.pozycjaPilkiX = pilka.x();
                stanGry.pozycjaPilkiY = pilka.y();
                stanGry.pozycjaPaletkiX = paletka.x();
                stanGry.pozycjaPaletkiY = paletka.y();
                stanGry.predkoscPilkiX = pilka.predkosc.x;
                stanGry.predkoscPilkiY = pilka.predkosc.y;

                // Dodanie cegie³ek
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
                okno.draw(pilka.ksztalt);
                okno.draw(paletka.ksztalt);
                for (const auto& cegielka : cegielki) {
                    okno.draw(cegielka.ksztalt);
                }
            }
        }

        okno.display();
    }
        
    return 0;
}


