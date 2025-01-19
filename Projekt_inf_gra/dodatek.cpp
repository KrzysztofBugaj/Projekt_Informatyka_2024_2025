//#include <SFML/Graphics.hpp>
//#include <vector>
//#include <fstream>
//#include <iostream>
//
//
//using namespace sf;
//
// Definicje sta�ych
//constexpr int szerokoscOkna{ 800 }, wysokoscOkna{ 600 };
//constexpr float promienPilki{ 10.f }, predkoscPilki{ 8.f };
//constexpr float szerokoscPaletki{ 60.f }, wysokoscPaletki{ 20.f }, predkoscPaletki{ 6.f };
//constexpr float szerokoscCegielki{ 60.f }, wysokoscCegielki{ 20.f };
//constexpr int liczbaCegielekX{ 11 }, liczbaCegielekY{ 4 };
//
// Klasa reprezentuj�ca pi�k�
//class Pilka {
//public:
//    CircleShape ksztalt;
//    Vector2f predkosc{ -predkoscPilki, -predkoscPilki };
//
//    Pilka(float x, float y) {
//        ksztalt.setPosition(x, y);
//        ksztalt.setRadius(promienPilki);
//        ksztalt.setFillColor(Color::Red);
//        ksztalt.setOrigin(promienPilki, promienPilki);
//    }
//
//     Aktualizacja pozycji pi�ki
//    void aktualizuj() {
//        ksztalt.move(predkosc);
//
//         Odbicie od �cian bocznych
//        if (lewo() < 0)
//            predkosc.x = predkoscPilki;
//        else if (prawo() > szerokoscOkna)
//            predkosc.x = -predkoscPilki;
//
//         Odbicie od g�rnej �ciany
//        if (gora() < 0)
//            predkosc.y = predkoscPilki;
//        else if (dol() > wysokoscOkna)
//            predkosc.y = -predkoscPilki;
//    }
//
//     Funkcje pomocnicze do uzyskania pozycji kraw�dzi pi�ki
//    float x() { return ksztalt.getPosition().x; }
//    float y() { return ksztalt.getPosition().y; }
//    float lewo() { return x() - ksztalt.getRadius(); }
//    float prawo() { return x() + ksztalt.getRadius(); }
//    float gora() { return y() - ksztalt.getRadius(); }
//    float dol() { return y() + ksztalt.getRadius(); }
//};
//
// Klasa bazowa reprezentuj�ca prostok�t (u�ywana przez paletk� i cegie�ki)
//class Prostokat {
//public:
//    RectangleShape ksztalt;
//
//     Dodano 'const' na ko�cu funkcji
//    float x() const { return ksztalt.getPosition().x; }
//    float y() const { return ksztalt.getPosition().y; }
//    float lewo() const { return x() - ksztalt.getSize().x / 2.f; }
//    float prawo() const { return x() + ksztalt.getSize().x / 2.f; }
//    float gora() const { return y() - ksztalt.getSize().y / 2.f; }
//    float dol() const { return y() + ksztalt.getSize().y / 2.f; }
//};
//
// Klasa reprezentuj�ca paletk�
//class Paletka : public Prostokat {
//public:
//    Vector2f predkosc;
//
//    Paletka(float x, float y) {
//        ksztalt.setPosition(x, y);
//        ksztalt.setSize({ szerokoscPaletki, wysokoscPaletki });
//        ksztalt.setFillColor(Color::Red);
//        ksztalt.setOrigin(szerokoscPaletki / 2.f, wysokoscPaletki / 2.f);
//    }
//
//     Aktualizacja pozycji paletki na podstawie wciskanych klawiszy
//    void aktualizuj() {
//        ksztalt.move(predkosc);
//
//        if (Keyboard::isKeyPressed(Keyboard::Key::Left) && lewo() > 0)
//            predkosc.x = -predkoscPaletki;
//        else if (Keyboard::isKeyPressed(Keyboard::Key::Right) && prawo() < szerokoscOkna)
//            predkosc.x = predkoscPaletki;
//        else
//            predkosc.x = 0;
//    }
//};
//
//class Cegielka : public Prostokat {
//public:
//    bool zniszczona{ false };
//    bool aktywna{ true };
//
//     Konstruktor przyjmuj�cy trzy argumenty: x, y, aktywna
//    Cegielka(float x, float y, bool aktywna = true) {
//        ksztalt.setPosition(x, y);
//        ksztalt.setSize({ szerokoscCegielki, wysokoscCegielki });
//        ksztalt.setFillColor(Color::Yellow);
//        ksztalt.setOrigin(szerokoscCegielki / 2.f, wysokoscCegielki / 2.f);
//
//        this->aktywna = aktywna;  // Ustawienie warto�ci aktywno�ci cegie�ki
//    }
//};
//
//
//
// Funkcja szablonowa do sprawdzania kolizji
//template <class T1, class T2>
//bool czyKolizja(T1& obiektA, T2& obiektB) {
//    return obiektA.prawo() >= obiektB.lewo() && obiektA.lewo() <= obiektB.prawo() &&
//        obiektA.dol() >= obiektB.gora() && obiektA.gora() <= obiektB.dol();
//}
//
// Funkcja obs�uguj�ca kolizj� mi�dzy paletk� a pi�k�
//void testujKolizje(Paletka& paletka, Pilka& pilka) {
//    if (!czyKolizja(paletka, pilka)) return;
//
//    pilka.predkosc.y = -predkoscPilki;
//    if (pilka.x() < paletka.x())
//        pilka.predkosc.x = -predkoscPilki;
//    else
//        pilka.predkosc.x = predkoscPilki;
//}
//
// Funkcja obs�uguj�ca kolizj� mi�dzy cegie�k� a pi�k�
//void testujKolizje(Cegielka& cegielka, Pilka& pilka) {
//    if (!czyKolizja(cegielka, pilka)) return;
//    cegielka.zniszczona = true;
//
//    float nakladanieLewo{ pilka.prawo() - cegielka.lewo() };
//    float nakladaniePrawo{ cegielka.prawo() - pilka.lewo() };
//    float nakladanieGora{ pilka.dol() - cegielka.gora() };
//    float nakladanieDol{ cegielka.dol() - pilka.gora() };
//
//    bool pilkaZLewej(abs(nakladanieLewo) < abs(nakladaniePrawo));
//    bool pilkaZGory(abs(nakladanieGora) < abs(nakladanieDol));
//
//    float minimalneNakladanieX{ pilkaZLewej ? nakladanieLewo : nakladaniePrawo };
//    float minimalneNakladanieY{ pilkaZGory ? nakladanieGora : nakladanieDol };
//
//    if (abs(minimalneNakladanieX) < abs(minimalneNakladanieY))
//        pilka.predkosc.x = pilkaZLewej ? -predkoscPilki : predkoscPilki;
//    else
//        pilka.predkosc.y = pilkaZGory ? -predkoscPilki : predkoscPilki;
//}
//
// Klasa do wy�wietlania ekranu "Koniec Gry"
//class KoniecGry {
//public:
//    Text tekstKoniecGry;
//    Text tekstRestart;
//    Text tekstPowrotDoMenu;
//    Text tekstWyjscie;
//    Font czcionka;
//
//    KoniecGry(float szerokosc, float wysokosc) {
//        if (!czcionka.loadFromFile("arial.ttf")) {
//            exit(EXIT_FAILURE);
//        }
//
//        tekstKoniecGry.setFont(czcionka);
//        tekstKoniecGry.setString("Koniec Gry");
//        tekstKoniecGry.setCharacterSize(50);
//        tekstKoniecGry.setFillColor(Color::Red);
//        tekstKoniecGry.setPosition(szerokosc / 2 - tekstKoniecGry.getGlobalBounds().width / 2, wysokosc / 4);
//
//        tekstRestart.setFont(czcionka);
//        tekstRestart.setString("Naci�nij R, aby zrestartowa�");
//        tekstRestart.setCharacterSize(30);
//        tekstRestart.setFillColor(Color::White);
//        tekstRestart.setPosition(szerokosc / 2 - tekstRestart.getGlobalBounds().width / 2, wysokosc / 2.5);
//
//        tekstPowrotDoMenu.setFont(czcionka);
//        tekstPowrotDoMenu.setString("Naci�nij M, aby wr�ci� do menu");
//        tekstPowrotDoMenu.setCharacterSize(30);
//        tekstPowrotDoMenu.setFillColor(Color::White);
//        tekstPowrotDoMenu.setPosition(szerokosc / 2 - tekstPowrotDoMenu.getGlobalBounds().width / 2, wysokosc / 2);
//
//        tekstWyjscie.setFont(czcionka);
//        tekstWyjscie.setString("Naci�nij E, aby wyj��");
//        tekstWyjscie.setCharacterSize(30);
//        tekstWyjscie.setFillColor(Color::White);
//        tekstWyjscie.setPosition(szerokosc / 2 - tekstWyjscie.getGlobalBounds().width / 2, wysokosc / 1.5);
//    }
//
//    void rysuj(RenderWindow& okno) {
//        okno.draw(tekstKoniecGry);
//        okno.draw(tekstRestart);
//        okno.draw(tekstPowrotDoMenu);
//        okno.draw(tekstWyjscie);
//    }
//};
//
// Klasa obs�uguj�ca menu g��wne
//class Menu {
//public:
//    Menu(float szerokosc, float wysokosc) {
//        if (!czcionka.loadFromFile("arial.ttf")) {
//            exit(EXIT_FAILURE);
//        }
//
//        opcje[0].setFont(czcionka);
//        opcje[0].setFillColor(Color::Red);
//        opcje[0].setString("Start");
//        opcje[0].setPosition(Vector2f(szerokosc / 2 - opcje[0].getGlobalBounds().width / 2, wysokosc / (MAX_OPCJE + 1) * 1));
//
//        opcje[1].setFont(czcionka);
//        opcje[1].setFillColor(Color::White);
//        opcje[1].setString("Instrukcje");
//        opcje[1].setPosition(Vector2f(szerokosc / 2 - opcje[1].getGlobalBounds().width / 2, wysokosc / (MAX_OPCJE + 1) * 2));
//
//        opcje[2].setFont(czcionka);
//        opcje[2].setFillColor(Color::White);
//        opcje[2].setString("Wyj�cie");
//        opcje[2].setPosition(Vector2f(szerokosc / 2 - opcje[2].getGlobalBounds().width / 2, wysokosc / (MAX_OPCJE + 1) * 3));
//
//        opcje[3].setFont(czcionka);
//        opcje[3].setFillColor(Color::White);
//        opcje[3].setString("Wczytaj gre");
//        opcje[3].setPosition(Vector2f(szerokosc / 2 - opcje[2].getGlobalBounds().width / 2, wysokosc / (MAX_OPCJE + 1) * 4));
//
//        wybranaOpcja = 0;
//    }
//
//    void rysuj(RenderWindow& okno) {
//        for (int i = 0; i < MAX_OPCJE; ++i) {
//            okno.draw(opcje[i]);
//        }
//    }
//
//    void ruchWGore() {
//        if (wybranaOpcja - 1 >= 0) {
//            opcje[wybranaOpcja].setFillColor(Color::White);
//            wybranaOpcja--;
//            opcje[wybranaOpcja].setFillColor(Color::Red);
//        }
//    }
//
//    void ruchWDol() {
//        if (wybranaOpcja + 1 < MAX_OPCJE) {
//            opcje[wybranaOpcja].setFillColor(Color::White);
//            wybranaOpcja++;
//            opcje[wybranaOpcja].setFillColor(Color::Red);
//        }
//    }
//
//    int pobierzWybranaOpcje() {
//        return wybranaOpcja;
//    }
//
//private:
//    static const int MAX_OPCJE = 4;
//    int wybranaOpcja;
//    Text opcje[MAX_OPCJE];
//    Font czcionka;
//};
//
// Funkcja rysuj�ca ekran instrukcji
//void rysujInstrukcje(RenderWindow& okno, Font& czcionka) {
//    Text tekstInstrukcje;
//    tekstInstrukcje.setFont(czcionka);
//    tekstInstrukcje.setFillColor(Color::White);
//    tekstInstrukcje.setString("Sterowanie:\n\n"
//        "- U�yj strza�ek w lewo i w prawo, aby porusza� paletk�.\n"
//        "- Naci�nij Spacj�, aby wstrzyma� gr�.\n"
//        "- Zbij wszystkie cegie�ki!\n\n"
//        "Naci�nij Enter, aby wr�ci� do menu g��wnego.");
//    tekstInstrukcje.setCharacterSize(24);
//    tekstInstrukcje.setPosition(50, 150);
//
//    okno.draw(tekstInstrukcje);
//}
//
//
//class StanGry {
//public:
//     Pozycje pi�ki
//    float pozycjaPilkiX;
//    float pozycjaPilkiY;
//
//     Pozycje paletki
//    float pozycjaPaletkiX;
//    float pozycjaPaletkiY;
//
//     Pr�dko�� pi�ki
//    float predkoscPilkiX;
//    float predkoscPilkiY;
//
//     Dane cegie�ek (pozycje i aktywno��)
//    struct Cegielka {
//        float x, y;
//        bool aktywna;
//    };
//    std::vector<Cegielka> cegielki;
//
//     Funkcja zapisuj�ca stan gry do pliku
//    void zapiszStanDoPliku(const std::string& Nazwapliku) {
//        std::ofstream plik(Nazwapliku);
//        if (plik.is_open()) {
//             Zapis pi�ki
//            plik << pozycjaPilkiX << " " << pozycjaPilkiY << std::endl;
//            plik << predkoscPilkiX << " " << predkoscPilkiY << std::endl;
//
//             Zapis paletki
//            plik << pozycjaPaletkiX << " " << pozycjaPaletkiY << std::endl;
//
//             Zapis cegie�ek
//            plik << cegielki.size() << std::endl; // Liczba cegie�ek
//            for (const auto& cegielka : cegielki) {
//                plik << cegielka.x << " " << cegielka.y << " " << cegielka.aktywna << std::endl;
//            }
//
//            plik.close();
//            std::cout << "Stan gry zapisany do pliku: " << Nazwapliku << std::endl;
//        }
//        else {
//            std::cerr << "Nie mo�na otworzy� pliku do zapisu: " << Nazwapliku << std::endl;
//        }
//    }
//
//     Funkcja odczytuj�ca stan gry z pliku
//    void odczytajStanZPliku(const std::string& Nazwapliku) {
//        std::ifstream plik(Nazwapliku);
//        if (plik.is_open()) {
//             Odczyt pi�ki
//            plik >> pozycjaPilkiX >> pozycjaPilkiY;
//            plik >> predkoscPilkiX >> predkoscPilkiY;
//
//             Odczyt paletki
//            plik >> pozycjaPaletkiX >> pozycjaPaletkiY;
//
//             Odczyt cegie�ek
//            cegielki.clear();
//            size_t liczbaCegielek;
//            plik >> liczbaCegielek; // Odczytaj liczb� cegie�ek
//            for (size_t i = 0; i < liczbaCegielek; ++i) {
//                Cegielka cegielka;
//                plik >> cegielka.x >> cegielka.y >> cegielka.aktywna;
//                cegielki.push_back(cegielka);
//            }
//
//            plik.close();
//            std::cout << "Stan gry wczytany z pliku: " << Nazwapliku << std::endl;
//        }
//        else {
//            std::cerr << "Nie mo�na otworzy� pliku do odczytu: " << Nazwapliku << std::endl;
//        }
//    }
//};
//
//
//
//class WprowadzNick {
//public:
//    std::string nick;
//    Text tekstNick;
//    Font czcionka;
//
//    WprowadzNick() {
//        if (!czcionka.loadFromFile("arial.ttf")) {
//            exit(EXIT_FAILURE);
//        }
//
//        tekstNick.setFont(czcionka);
//        tekstNick.setCharacterSize(30);
//        tekstNick.setFillColor(Color::White);
//        tekstNick.setPosition(250, 250);
//    }
//
//    void wprowadzanie(RenderWindow& okno) {
//        Event event;
//        while (okno.pollEvent(event)) {
//            if (event.type == Event::Closed) {
//                okno.close();
//            }
//            if (event.type == Event::TextEntered) {
//                 Obs�uga backspace
//                if (event.text.unicode == '\b' && nick.size() > 0) { // Backspace
//                    nick.pop_back();
//                }
//                 Dodawanie nowego znaku
//                else if (event.text.unicode < 128) {
//                    if (nick.size() < 20) { // Ograniczenie d�ugo�ci nicku
//                        nick += static_cast<char>(event.text.unicode);
//                    }
//                }
//            }
//             Zako�czenie wprowadzania po naci�ni�ciu Enter
//            if (event.type == Event::KeyPressed && event.key.code == Keyboard::Enter) {
//                 Przechodzimy dalej po wpisaniu nicku
//                break;
//            }
//        }
//
//         Wy�wietlanie tekstu
//        tekstNick.setString("Wpisz sw�j nick: " + nick);
//        okno.clear();
//        okno.draw(tekstNick);
//        okno.display();
//    }
//
//    std::string pobierzNick() const {
//        return nick;
//    }
//};
//
//
//int main() {
//    RenderWindow okno(VideoMode(szerokoscOkna, wysokoscOkna), "Arkanoid");
//
//    std::string nickGracza;
//    WprowadzNick wprowadzNick;
//
//    P�tla wprowadzania nicku
//        while (okno.isOpen()) {
//            Event zdarzenie;
//            while (okno.pollEvent(zdarzenie)) {
//                if (zdarzenie.type == Event::Closed) {
//                    okno.close();
//                }
//
//
//                if (zdarzenie.type == Event::KeyPressed) {
//                    if (zdarzenie.key.code == Keyboard::Enter) {
//                        Zako�czenie wprowadzania nicku
//                            nickGracza = wprowadzNick.pobierzNick();
//                        wprowadzanieNicku = false; // Przechodzimy do g��wnej gry
//                    }
//                }
//
//                wprowadzNick.wprowadzanie(okno); // Wy�wietlanie okna wprowadzania nicku
//                okno.clear(Color::Black);
//            }
//
//
//
//            if (zdarzenie.type == Event::KeyPressed && zdarzenie.key.code == Keyboard::Enter) {
//                if (!wprowadzNick.nick.empty()) {
//                    nickGracza = wprowadzNick.pobierzNick();
//                    break; // Przechodzi do g��wnej gry po naci�ni�ciu Enter
//                }
//            }
//
//        }
//
//    Inicjalizacja menu, paletki, pi�ki i cegie�ek
//        Menu menu(szerokoscOkna, wysokoscOkna);
//    Pilka pilka(szerokoscOkna / 2, wysokoscOkna / 2);
//    Paletka paletka(szerokoscOkna / 2, wysokoscOkna - 50);
//    std::vector<Cegielka> cegielki;
//
//    for (int iX = 0; iX < liczbaCegielekX; ++iX) {
//        for (int iY = 0; iY < liczbaCegielekY; ++iY) {
//            cegielki.emplace_back(
//                (iX + 1) * (szerokoscCegielki + 3) + 22,
//                (iY + 2) * (wysokoscCegielki + 3)
//            );
//        }
//    }
//
//    bool graRozpoczeta = false;
//    bool pokazInstrukcje = false;
//    bool koniecGry = false;
//
//    KoniecGry ekranKoniecGry(szerokoscOkna, wysokoscOkna);
//
//    okno.setFramerateLimit(60);
//
//    while (okno.isOpen()) {
//        Event zdarzenie;
//        while (okno.pollEvent(zdarzenie)) {
//            if (zdarzenie.type == Event::Closed) {
//                okno.close();
//            }
//
//            if (zdarzenie.type == Event::KeyPressed) {
//                if (zdarzenie.key.code == Keyboard::Up) {
//                    menu.ruchWGore();
//                }
//                else if (zdarzenie.key.code == Keyboard::Down) {
//                    menu.ruchWDol();
//                }
//                else if (zdarzenie.key.code == Keyboard::Enter) {
//                    int wybranaOpcja = menu.pobierzWybranaOpcje();
//                    if (wybranaOpcja == 0) {
//
//                        graRozpoczeta = true;
//                        koniecGry = false;
//                    }
//                    else if (wybranaOpcja == 1) {
//                        pokazInstrukcje = true;
//                    }
//                    else if (wybranaOpcja == 2) {
//                        okno.close();
//                    }
//                    else if (wybranaOpcja == 3) {
//
//
//                        StanGry stanGry;
//                        stanGry.odczytajStanZPliku("stan_gry.txt");
//
//                        Przywracanie stanu cegie�ek
//                            cegielki.clear();
//                        for (const auto& stanCegielki : stanGry.cegielki) {
//                            cegielki.emplace_back(
//                                stanCegielki.x,
//                                stanCegielki.y,
//                                stanCegielki.aktywna
//                            );
//                        }
//
//                        pilka.ksztalt.setPosition(stanGry.pozycjaPilkiX, stanGry.pozycjaPilkiY);
//                        pilka.predkosc = { stanGry.predkoscPilkiX, stanGry.predkoscPilkiY };
//                        paletka.ksztalt.setPosition(stanGry.pozycjaPaletkiX, stanGry.pozycjaPaletkiY);
//
//
//                        graRozpoczeta = true;
//
//
//
//                    }
//                }
//                else if (zdarzenie.key.code == Keyboard::Escape) {
//                    if (graRozpoczeta || pokazInstrukcje || koniecGry) {
//                        graRozpoczeta = false;
//                        pokazInstrukcje = false;
//                        koniecGry = false;
//                    }
//                }
//
//                if (koniecGry) {
//                    if (zdarzenie.key.code == Keyboard::R) {
//                        Restart gry
//                            graRozpoczeta = true;
//                        koniecGry = false;
//                        pilka.ksztalt.setPosition(szerokoscOkna / 2, wysokoscOkna / 2);
//                        paletka.ksztalt.setPosition(szerokoscOkna / 2, wysokoscOkna - 50);
//                        cegielki.clear();
//                        for (int iX = 0; iX < liczbaCegielekX; ++iX) {
//                            for (int iY = 0; iY < liczbaCegielekY; ++iY) {
//                                cegielki.emplace_back(
//                                    (iX + 1) * (szerokoscCegielki + 3) + 22,
//                                    (iY + 2) * (wysokoscCegielki + 3)
//                                );
//                            }
//                        }
//                    }
//                    else if (zdarzenie.key.code == Keyboard::M) {
//                        Powr�t do menu
//                            graRozpoczeta = false;
//                        koniecGry = false;
//                    }
//                    else if (zdarzenie.key.code == Keyboard::E) {
//                        Wyj�cie
//                            okno.close();
//                    }
//                }
//            }
//        }
//
//        okno.clear(Color::Black);
//
//        if (pokazInstrukcje) {
//            Font czcionka;
//            if (czcionka.loadFromFile("arial.ttf")) {
//                rysujInstrukcje(okno, czcionka);
//            }
//        }
//        else if (!graRozpoczeta && !koniecGry) {
//            menu.rysuj(okno);
//        }
//        else if (koniecGry) {
//            ekranKoniecGry.rysuj(okno);
//        }
//        else {
//            pilka.aktualizuj();
//            paletka.aktualizuj();
//
//            testujKolizje(paletka, pilka);
//
//            for (auto& cegielka : cegielki) {
//                testujKolizje(cegielka, pilka);
//            }
//
//            cegielki.erase(
//                std::remove_if(cegielki.begin(), cegielki.end(),
//                    [](const Cegielka& cegielka) { return cegielka.zniszczona; }),
//                cegielki.end()
//            );
//
//            if (pilka.dol() > wysokoscOkna) {
//
//
//                StanGry stanGry;
//                stanGry.pozycjaPilkiX = pilka.x();
//                stanGry.pozycjaPilkiY = pilka.y();
//                stanGry.pozycjaPaletkiX = paletka.x();
//                stanGry.pozycjaPaletkiY = paletka.y();
//                stanGry.predkoscPilkiX = pilka.predkosc.x;
//                stanGry.predkoscPilkiY = pilka.predkosc.y;
//
//                Dodanie cegie�ek
//                    stanGry.cegielki.clear();
//                for (const auto& cegielka : cegielki) {
//                    StanGry::Cegielka stanCegielki = {
//                        cegielka.x(),
//                        cegielka.y(),
//                        cegielka.aktywna // Zak�adam, �e masz pole aktywna
//                    };
//                    stanGry.cegielki.push_back(stanCegielki);
//                }
//
//                stanGry.zapiszStanDoPliku("stan_gry.txt");
//
//                Przegrana
//                    pilka.ksztalt.setPosition(szerokoscOkna / 2, wysokoscOkna / 2);
//                pilka.predkosc = { -predkoscPilki, -predkoscPilki };
//                paletka.ksztalt.setPosition(szerokoscOkna / 2, wysokoscOkna - 50);
//                cegielki.clear();
//
//                for (int iX = 0; iX < liczbaCegielekX; ++iX) {
//                    for (int iY = 0; iY < liczbaCegielekY; ++iY) {
//                        cegielki.emplace_back(
//                            (iX + 1) * (szerokoscCegielki + 3) + 22,
//                            (iY + 2) * (wysokoscCegielki + 3)
//                        );
//                    }
//                }
//
//                graRozpoczeta = false;
//                koniecGry = true;
//
//
//
//            }
//            else {
//                okno.draw(pilka.ksztalt);
//                okno.draw(paletka.ksztalt);
//                for (const auto& cegielka : cegielki) {
//                    okno.draw(cegielka.ksztalt);
//                }
//                Wy�wietl nick gracza w prawym g�rnym rogu
//                    Text nickText;
//                nickText.setFont(wprowadzNick.czcionka);
//                nickText.setString("Gracz: " + nickGracza);
//                nickText.setCharacterSize(20);
//                nickText.setFillColor(Color::White);
//                nickText.setPosition(szerokoscOkna - 100, 10);
//                okno.draw(nickText);
//            }
//        }
//
//        okno.display();
//    }
//
//    return 0;
//}
//    
