2.6.0.0
-----
* Obsługa lokalizacji (polskiej i angielskiej).

2.5.1.0
-----
* Poprawki w tworzeniu elementów wtyczki.

2.5.0.0
-----
* Poprawki w procesie ładowania wtyczki.
* Zmiana tekstu oraz pozycji elementu restartu.
* Zastąpienie własnej ikony ikoną z kompozycji.

2.4.0.0
-----
* Drobna optymalizacja kodu.
* Kompilacja pod poprawionymi nagłówkami dla wersji 64-bitowej.
* Zmniejszenie rozmiaru wtyczki 64-bitowej poprzez użycie kompresji LZMA.
* Usunięcie funkcji AQQRESTARTER_SYSTEM_RESTART.

2.3.1.0
-----
* Wtyczka posiada własną ikonę pokazywaną w ustawieniach komunikatora na liście wtyczek.
* Usunięcie pliku wspomagającego restart.
* Usunięcie notyfikacji AQQRESTARTER_SYSTEM_RESTARTING.
* Zmniejszenie rozmiaru wtyczki 32-bitowej poprzez użycie kompresji LZMA.
* Brak kompresji wtyczki 64-bitowej.

2.3.0.0
-----
* Dodanie 64-bitowej wersji wtyczki.
* Autologowanie na profile niezabezpieczone hasłem bez opcji autologowania.
* Zmiany w pliku wspomagającym restart.
* Inne mniej ważne zmiany w kodzie wtyczki.

2.2.0.0
-----
* Przepisanie wtyczki pod kompilator Embarcadero C++ Builder XE3.

2.1.3.0
-----
* Wykonywanie akcji za użytkownika przez plik wspomagający restart dopiero po faktycznym pokazaniu się okna logowania.
* Plik wspomagającym restart nie ukrywa już okna logowania.
* Niewpisywanie hasła do profilu, gdy nie jest on zabezpieczony.

2.1.2.0
-----
* Poprawne ładowanie ikony wtyczki do interfejsu komunikatora.
* Poprawna zmiana ikony wtyczki po zmianie aktywnej kompozycji.

2.1.1.0
-----
* Zmiany w pliku wspomagającym restart AQQ.
* Optymalizacja kodu.

2.1.0.0
-----
* Zmiany w pliku wspomagającym restart AQQ.

2.0.4.0
-----
* Poprawne usuwanie pliku wspomagającego restart AQQ.
* Dodano notyfikację AQQRESTARTER_SYSTEM_RESTARTING wywoływaną przed restartem AQQ.

2.0.3.0
-----
* Inteligentne ustalanie pozycji elementu w głównym menu AQQ.

2.0.2.0
-----
* Przepisanie wtyczki pod kompilator CodeGear C++ Builder 2010.
* SDK: Dodano funkcję AQQRESTARTER_SYSTEM_RESTART = "AQQRestarter/System/Restart".

2.0.1.0
-----
* Ukrywanie okna logowania i chmurki informacyjnej poprzez WinAPI.
* Wtyczka zamyka AQQ i uruchamia je ponownie poprzez funkcję z SDK AQQ.

2.0.0.0
-----
* Przepisanie wtyczki pod kompilator CodeGear C++ Builder 2009.
* Nowa instancja AQQ jest uruchamiana dopiero wtedy gdy poprzednia zostanie zamknięta.

1.0.4.2
-----
* Poprawiono drobny wyciek pamięci.
* Proces AQQ nie jest już killowany (zapobiega to niezapisywaniu ustawień AQQ).
* Zmieniono tryb uruchamiania ponownie AQQ.

1.0.4.1
-----
* Poprawka w usuwania pliku Restarter.exe.
* Przy zmianie kompozycji sprawdzany jest jej katolog w poszukiwaniu ikonki dla wtyczki.

1.0.4.0
-----
* Ewidentny brak okna logowania nawet przy profilu zabezpieczonym na hasło.

1.0.3.0
-----
* Dodanie angielskiej lokalizacji.

1.0.2.2
-----
* Mała poprawka w kodzie wtyczki. 

1.0.2.1
-----
* Usunięto pobieranie ścieżki AQQ poprzez PID procesu (ścieżka jest zapisywana do pliku).

1.0.2.0
-----
* Zmieniono całkowicie sposób restartu - teraz okno logowania nie ma prawa się pokazać.

1.0.1.2
-----
* Przywrócenie starej pozycji elementu w menu Program (z wersji 1.0.0.0).
* Małe poprawki w kodzie.
* Wtyczka sprawdza przy ładowaniu czy w katalogu aktywnej kompozycji w folderze Icons znajduje się plik "AQQRestarter.png", jeżeli tak ustawia go jako grafikę przycisku.

1.0.1.0
-----
* Przystosowanie pozycji ikonki do wersji AQQ 2.0.5.15+.
* Wtyczka sprawdza czy w katalogu prywatnym wtyczek jest plik "AQQRestarter.png", jeżeli tak ustawia go jako grafikę przycisku.

1.0.0.0
-----
* Zmiana sposobu restartowania (taka jak we wtyczce MD kameroka).

0.0.4.4
-----
* Poprawiono błąd przy wyładowywaniu wtyczki oraz bug z elementem w tray’u.

0.0.4.2
-----
* Brak ekranu logowania po restarcie.

0.0.4.0
-----
* Zmienienie sposobu restartu AQQ! Od teraz już nie zostaje ikonka w tray’u.
* Zmiana nazwy wtyczki na “AQQ Restarter”.

0.0.3.0
-----
* Zmiana pozycja ikonki w menu Program.

0.0.2.0
-----
* Dodanie własnej ikonki dla tworzonego elementu w AQQ.

0.0.1.0
-----
* Pierwsza publiczna wersja.