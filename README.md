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
```
gdzie nazwa_pliku.txt to ścieżka do pliku tekstowego, który chcesz skompresować.

### **Dekompresja pliku binarnego**

Aby uruchomić proces dekompresji, wykonaj polecenie:
```bash
make run MODE=d INPUT=nazwa_pliku_compressed.bin
```
gdzie nazwa_pliku_compressed.bin to ścieżka do wcześniej skompresowanego pliku binarnego.

### **Porównanie plików**
Aby porównać oryginalny plik z plikiem zdekompresowanym, użyj polecenia:
```bash
make diff INPUT=nazwa_pliku
```
gdzie nazwa_pliku to nazwa oryginalnego pliku tekstowego.

---

> [!WARNING]
> Na potrzeby demonstracji metoda kompresuje tylko pliki zawierające ciągi cyfr - należy się upewnić, że plik nie zawiera innych znaków

---

## **Pliki w katalogu**

* tans.cpp - Główny plik źródłowy programu zawierający implementację algorytmu tANS.
* Makefile - Plik makefile umożliwiający kompilację i uruchamianie programu.
* README.md - Plik z dokumentacją projektu w formacie Markdown.

---

## **Czyszczenie katalogu**

Aby usunąć pliki tymczasowe i binarne, wykonaj polecenie:
``` bash
make clean
```