# **Program do kompresji danych oparty na algorytmie tANS**

## **Opis**
Katalog zawiera program w języku C++, który realizuje kompresję danych za pomocą metody opartej na algorytmie tANS (tablica o stałym rozmiarze).

Program umożliwia:
1. Kompresję pliku tekstowego (*.txt) do formatu binarnego (*.bin).
2. Dekompresję pliku binarnego (*.bin) do formatu tekstowego (*.txt).
3. Porównanie oryginalnego pliku z plikiem zdekompresowanym.

---

## **Uruchamianie programu**

Katalog zawiera plik `Makefile` umożliwiający kompilację, uruchamianie oraz czyszczenie środowiska projektu. Obsługa programu przebiega w następujący sposób:

### **Kompresja pliku tekstowego**
Aby uruchomić proces kompresji, wykonaj polecenie w terminalu:
```bash
make run MODE=c INPUT=nazwa_pliku.txt
