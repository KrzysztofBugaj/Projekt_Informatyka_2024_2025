#include <SFML/Graphics.hpp>
#include <vector>

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

    float x() { return ksztalt.getPosition().x; }
    float y() { return ksztalt.getPosition().y; }
    float lewo() { return x() - ksztalt.getSize().x / 2.f; }
    float prawo() { return x() + ksztalt.getSize().x / 2.f; }
    float gora() { return y() - ksztalt.getSize().y / 2.f; }
    float dol() { return y() + ksztalt.getSize().y / 2.f; }
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

// Klasa reprezentuj¹ca cegie³kê
class Cegielka : public Prostokat {
public:
    bool zniszczona{ false };

    Cegielka(float x, float y) {
        ksztalt.setPosition(x, y);
        ksztalt.setSize({ szerokoscCegielki, wysokoscCegielki });
        ksztalt.setFillColor(Color::Yellow);
        ksztalt.setOrigin(szerokoscCegielki / 2.f, wysokoscCegielki / 2.f);
    }
};

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
    Text tekstRestart;
    Text tekstPowrotDoMenu;
    Text tekstWyjscie;
    Font czcionka;

    KoniecGry(float szerokosc, float wysokosc) {
        if (!czcionka.loadFromFile("arial.ttf")) {
            exit(EXIT_FAILURE);
        }

        tekstKoniecGry.setFont(czcionka);
        tekstKoniecGry.setString("Koniec Gry");
        tekstKoniecGry.setCharacterSize(50);
        tekstKoniecGry.setFillColor(Color::Red);
        tekstKoniecGry.setPosition(szerokosc / 2 - tekstKoniecGry.getGlobalBounds().width / 2, wysokosc / 4);

        tekstRestart.setFont(czcionka);
        tekstRestart.setString("Naciœnij R, aby zrestartowaæ");
        tekstRestart.setCharacterSize(30);
        tekstRestart.setFillColor(Color::White);
        tekstRestart.setPosition(szerokosc / 2 - tekstRestart.getGlobalBounds().width / 2, wysokosc / 2.5);

        tekstPowrotDoMenu.setFont(czcionka);
        tekstPowrotDoMenu.setString("Naciœnij M, aby wróciæ do menu");
        tekstPowrotDoMenu.setCharacterSize(30);
        tekstPowrotDoMenu.setFillColor(Color::White);
        tekstPowrotDoMenu.setPosition(szerokosc / 2 - tekstPowrotDoMenu.getGlobalBounds().width / 2, wysokosc / 2);

        tekstWyjscie.setFont(czcionka);
        tekstWyjscie.setString("Naciœnij E, aby wyjœæ");
        tekstWyjscie.setCharacterSize(30);
        tekstWyjscie.setFillColor(Color::White);
        tekstWyjscie.setPosition(szerokosc / 2 - tekstWyjscie.getGlobalBounds().width / 2, wysokosc / 1.5);
    }

    void rysuj(RenderWindow& okno) {
        okno.draw(tekstKoniecGry);
        okno.draw(tekstRestart);
        okno.draw(tekstPowrotDoMenu);
        okno.draw(tekstWyjscie);
    }
};

// Klasa obs³uguj¹ca menu g³ówne
class Menu {
public:
    Menu(float szerokosc, float wysokosc) {
        if (!czcionka.loadFromFile("arial.ttf")) {
            exit(EXIT_FAILURE);
        }

        opcje[0].setFont(czcionka);
        opcje[0].setFillColor(Color::Red);
        opcje[0].setString("Start");
        opcje[0].setPosition(Vector2f(szerokosc / 2 - opcje[0].getGlobalBounds().width / 2, wysokosc / (MAX_OPCJE + 1) * 1));

        opcje[1].setFont(czcionka);
        opcje[1].setFillColor(Color::White);
        opcje[1].setString("Instrukcje");
        opcje[1].setPosition(Vector2f(szerokosc / 2 - opcje[1].getGlobalBounds().width / 2, wysokosc / (MAX_OPCJE + 1) * 2));

        opcje[2].setFont(czcionka);
        opcje[2].setFillColor(Color::White);
        opcje[2].setString("Wyjœcie");
        opcje[2].setPosition(Vector2f(szerokosc / 2 - opcje[2].getGlobalBounds().width / 2, wysokosc / (MAX_OPCJE + 1) * 3));

        wybranaOpcja = 0;
    }

    void rysuj(RenderWindow& okno) {
        for (int i = 0; i < MAX_OPCJE; ++i) {
            okno.draw(opcje[i]);
        }
    }

    void ruchWGore() {
        if (wybranaOpcja - 1 >= 0) {
            opcje[wybranaOpcja].setFillColor(Color::White);
            wybranaOpcja--;
            opcje[wybranaOpcja].setFillColor(Color::Red);
        }
    }

    void ruchWDol() {
        if (wybranaOpcja + 1 < MAX_OPCJE) {
            opcje[wybranaOpcja].setFillColor(Color::White);
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
    Font czcionka;
};

// Funkcja rysuj¹ca ekran instrukcji
void rysujInstrukcje(RenderWindow& okno, Font& czcionka) {
    Text tekstInstrukcje;
    tekstInstrukcje.setFont(czcionka);
    tekstInstrukcje.setFillColor(Color::White);
    tekstInstrukcje.setString("Sterowanie:\n\n"
        "- U¿yj strza³ek w lewo i w prawo, aby poruszaæ paletk¹.\n"
        "- Naciœnij Spacjê, aby wstrzymaæ grê.\n"
        "- Zbij wszystkie cegie³ki!\n\n"
        "Naciœnij Enter, aby wróciæ do menu g³ównego.");
    tekstInstrukcje.setCharacterSize(24);
    tekstInstrukcje.setPosition(50, 150);

    okno.draw(tekstInstrukcje);
}


int main() {
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
                        koniecGry = false;
                    }
                    else if (wybranaOpcja == 1) {
                        pokazInstrukcje = true;
                    }
                    else if (wybranaOpcja == 2) {
                        okno.close();
                    }
                }
                else if (zdarzenie.key.code == Keyboard::Escape) {
                    if (graRozpoczeta || pokazInstrukcje || koniecGry) {
                        graRozpoczeta = false;
                        pokazInstrukcje = false;
                        koniecGry = false;
                    }
                }

                if (koniecGry) {
                    if (zdarzenie.key.code == Keyboard::R) {
                        // Restart gry
                        graRozpoczeta = true;
                        koniecGry = false;
                        pilka.ksztalt.setPosition(szerokoscOkna / 2, wysokoscOkna / 2);
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
                    else if (zdarzenie.key.code == Keyboard::M) {
                        // Powrót do menu
                        graRozpoczeta = false;
                        koniecGry = false;
                    }
                    else if (zdarzenie.key.code == Keyboard::E) {
                        // Wyjœcie
                        okno.close();
                    }
                }
            }
        }

        okno.clear(Color::Black);

        if (pokazInstrukcje) {
            Font czcionka;
            if (czcionka.loadFromFile("arial.ttf")) {
                rysujInstrukcje(okno, czcionka);
            }
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


