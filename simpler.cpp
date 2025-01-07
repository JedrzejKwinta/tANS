#include <iostream>
#include <algorithm>
#include <numeric>
#include <vector>
#include <map>
#include <cmath>
#include <random>

class ANS {
    public:
        ANS(std::string IN)
        {
            INPUT = IN;
            inSize = IN.size();
            mkSymbol(IN);
            mkNb();
            prepareEncodingTable();
        }

        void mkSymbol(const std::string& input) {
            // Dynamiczne obliczenie \( R \) i częstotliwości
            R = chooseOptimalR(input, 0.1); // maxError = 0.1
            L = std::pow(2, R);

            // Generowanie symboli na podstawie częstotliwości
            std::vector<int> temp;
            for (const auto& [symbol, freq] : frequency) {
                for (int i = 0; i < freq; ++i) {
                    temp.push_back(symbol);
                }
            }

            // Tasowanie
            std::random_device rd;
            std::mt19937 g(rd());
            std::shuffle(temp.begin(), temp.end(), g);

            symbol = std::move(temp);

            // Weryfikacja
            if (symbol.size() != L) {
                std::cerr << "ERROR: Symbol size mismatch. Expected " << L 
                        << ", got " << symbol.size() << "\n";
                exit(EXIT_FAILURE);
            }
        }


        int chooseOptimalR(const std::string& input, double maxError) {
            for (char c : input) {
                int digit = c - '0';
                counts[digit]++;
            }

            for (int R = 4; R <= 16; ++R) {
                int L = std::pow(2, R);
                frequency.clear();

                // Skalowanie częstotliwości
                int totalFrequency = 0;
                for (const auto& [symbol, count] : counts) {
                    int f = std::round(static_cast<double>(count) * L / input.size());
                    if (f == 0) f = 1;
                    frequency[symbol] = f;
                    totalFrequency += f;
                }

                // Korekta liczności
                int delta = L - totalFrequency;
                while (delta != 0) {
                    for (auto& [key, value] : frequency) {
                        if (delta == 0) break;
                        if (delta > 0) {
                            value++;
                            delta--;
                        } else if (value > 1) {
                            value--;
                            delta++;
                        }
                    }
                }

                // Obliczanie błędu odwzorowania
                double error = 0.0;
                for (const auto& [symbol, count] : counts) {
                    double Ps = static_cast<double>(count) / input.size();
                    double approxPs = static_cast<double>(frequency[symbol]) / L;
                    error += std::abs(Ps - approxPs);
                }

                if (error <= maxError) {
                    return R;
                }
            }

            // Jeśli nie znaleziono odpowiedniego \( R \), zwracamy maksymalne \( R \).
            return 16;
        }

        void spread() {
            int step = std::floor((5.0 / 8.0) * L) + 3;

            // Upewnij się, że step i L są względnie pierwsze
            while (std::gcd(step, L) != 1) {
                ++step;
            }
            std::cerr << "witam " << std::gcd(step, L) << " stepujący step: " << step << std::endl;
            // Wstępne wypełnienie symbolami
            std::vector<int> tempSymbols(L, -1);
            std::vector<int> temp;
            int i = 0;
            /*for (const auto& [symbol, count] : frequency) {
                for (int j = 0; j < count; j++) {
                    if (tempSymbols[i] != -1) {
                        std::cerr << "ERROR: Overwriting symbol at position " << i << "\n";
                        //exit(EXIT_FAILURE); // Wyjdź, jeśli wykryjesz problem
                    }
                    tempSymbols[i] = symbol;
                    i = (i + step) % L;
                }
            }*/

            for(const auto& pair : frequency)
            {
                for(int k = 0; k < pair.second; k++)
                {
                    temp.push_back(pair.first);
                }
            }

            

            // Upewnij się, że wszystkie pozycje zostały wypełnione
            /*for (int k = 0; k < L; ++k) {
                if (tempSymbols[k] == -1) {
                    std::cerr << "ERROR: Unfilled position at " << k << "\n";
                    exit(EXIT_FAILURE); // Wyjdź, jeśli wykryjesz problem
                }
            }*/

            // Tasowanie dla losowości
            std::random_device rd;
            std::mt19937 g(rd());
            std::shuffle(temp.begin(), temp.end(), g);

            symbol = std::move(temp);  // Przeniesienie do właściwego wektora
            
        }

        void prepareEncodingTable() {
            int r = R + 1;
            L = std::pow(2, R);
            std::vector<int> next(10, -1);
            start.resize(10, -1); encodingTable.resize(L, -1);
            int sumL = 0;

            for(const auto& pair: frequency)
            {
                start[pair.first] = -pair.second + sumL;
                sumL += pair.second;
                next[pair.first] = pair.second;
            }

            //std::cerr << "\npreparing encoding table:\n" << std::endl;
            for(int x = L; x < 2*L; x++)
            {
                //std::cerr << "x = " << x <<  ", L = " << L << std::endl;
                //std::cerr << symbol[x - L] << std::endl;
                int s = symbol[x-L];
                int index = start[s] + (next[s])++;
                //std::cerr << "index: " << index << std::endl;
                encodingTable[index] = x;
            }

        }

        void useBits(int x, int nbBits) {
            // Wyciągnij nbBits najmniej znaczących bitów
            int extractedBits = x & ((1 << nbBits) - 1);

            // Tutaj można wykorzystać extractedBits, np. zapisać do strumienia wyjściowego.
            std::cout << "Extracted bits: " << std::bitset<32>(extractedBits) << " (" << extractedBits << ")\n";

            // Przykład: dodanie do strumienia bitowego (tu tylko symulacja)
            for (int i = nbBits - 1; i >= 0; --i) {
                bitStream.push_back((extractedBits >> i) & 1);
            }
        }

        void mkNb()
        {
            int r = R + 1;
            k.resize(10, -1); nb.resize(10, -1);
            for(const auto& s: frequency)
            {
                k[s.first] = R - std::floor(std::log2(s.second));
                nb[s.first] = (k[s.first] << r) - (s.second << k[s.first]);
            }
        } 

        int encode(int s, int x)
        { 
            int r = R + 1;
            int nbBits = (x + nb[s]) >> r;
            useBits(x, nbBits);
            x = encodingTable[start[s] + (x >> nbBits)];
            return x;
        }

        void encodeString(const std::string& input) {
            int x = L; // Stan początkowy, zwykle równy \( L \).

            for (char c : input) {
                int s = c - '0'; // Zamiana znaku na cyfrę
                x = encode(s, x); // Kodowanie symbolu
            }

            // Na koniec wypisz zakodowane bity.
            std::cout << "\nEncoded string in bits:\n";
            for (int bit : bitStream) {
                std::cout << bit;
            }
            std::cout << std::endl;
        }

        void Print()
        {
            std::cerr << "\nCOUNTS:\n";
            for (const auto& pair : counts) 
                std::cerr << "\t" << pair.first << ": " << pair.second << std::endl;

            std::cout << "\nSYMBOL:\n";
            for(auto u: symbol)
            {
                std::cout << u << " ";
            }

            std::cout << "\n\nFREQ:\n";
            for(const auto& pair: frequency)
            {
                std::cout << "\t" << pair.first << ": " << pair.second << std::endl;
            }

            std::cerr << "\n\nSTART:\n";
            for(const auto& pair : start) std::cerr << "\t" << pair << std::endl;

            std::cerr << "\n\nENCODINGTABLE:\n";
            for(auto u : encodingTable)
            {
                std::cerr << "\t" << u << std::endl;
            }

            std::cout << "\nL = " << L << std::endl;
        }

    private:
        std::map<int, int> frequency;
        std::map<int, int> counts;
        std::vector<int> encodingTable;
        std::vector<int> start;
        std::vector<int> symbol;
        std::vector<int> nbBits;
        std::vector<int> nb;
        std::vector<int> k;
        std::vector<int> bitStream;
        int L, R, inSize;
        std::string INPUT;
};


int main(int argc, char *argv[])
{
    std::string IN = argv[1];
    std::cout << IN << std::endl;
    ANS a(IN);
    //a.Print();
    a.encodeString(IN);
}