#include <iostream>
#include <vector>
#include <cmath>
#include <cassert>
#include <algorithm>

using namespace std;

// Funkcja pomocnicza do obliczania liczby bitów do przesunięcia
int calculateNbBits(int x, int nb_s, int r) {
    return (x + nb_s) >> r;
}

// Funkcja pomocnicza do obliczania wartości k[s] oraz nb[s]
int calculateK(int Ls, int R) {
    return R - (int)log2(Ls);
}

int calculateNb(int k, int r, int Ls) {
    return (k << r) - (Ls << k);
}

// Struktura reprezentująca stan encodera
struct EncoderState {
    int state; // Aktualny stan
    vector<int> encodingTable;
    vector<int> start;
    vector<int> symbol;
    vector<int> nbBits;

    EncoderState(int L, int R) {
        state = L;
        encodingTable.resize(2 * L, 0);
        start.resize(L, 0);
        symbol.resize(2 * L, 0);
        nbBits.resize(2 * L, 0);
    }

    // Funkcja do kodowania symbolu
    void encode(int s, int R, int L) {
        int k_s = calculateK(L, R);
        int nb_s = calculateNb(k_s, R + 1, L);
        int nbBits = calculateNbBits(state, nb_s, R); // Obliczamy liczbę bitów do przesunięcia
        int xtmp = state >> nbBits;

        // Znajdujemy odpowiednią kolumnę w encodingTable
        int index = start[s] + (xtmp);
        state = encodingTable[index];

        // Wypisujemy odpowiednią liczbę bitów (na razie w prosty sposób, żeby to zobaczyć)
        cout << "Encoded symbol: " << s << ", new state: " << state << endl;
    }
};

// Funkcja do przygotowania danych do kodowania
void prepareEncodingTable(EncoderState &encoder, int L, int R, vector<int> &symbols) {
    // Przygotowanie start i symbol (tutaj można dodać logikę)
    for (int i = 0; i < L; i++) {
        encoder.start[i] = i * 2; // Prosty sposób, by wypełnić start
        encoder.symbol[i] = i;
    }
    
    // Przygotowanie encodingTable (można to zoptymalizować)
    for (int i = 0; i < 2 * L; i++) {
        encoder.encodingTable[i] = (i + 1) % L; // przykładowe przejście
    }
}

// Główna funkcja, która demonstruje kodowanie
int main() {
    int L = 16; // Liczba stanów (przykład)
    int R = 4;  // Parametr R

    // Inicjalizowanie stanu enkodera
    EncoderState encoder(L, R);

    // Przygotowanie tabeli kodującej
    vector<int> symbols = {0, 1, 2}; // Symbol w alfabecie
    prepareEncodingTable(encoder, L, R, symbols);

    // Przykład kodowania ciągu symboli
    vector<int> inputSymbols = {2, 1, 2}; // Ciąg symboli do zakodowania
    for (int s : inputSymbols) {
        encoder.encode(s, R, L);
    }

    return 0;
}
