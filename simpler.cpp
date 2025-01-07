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
            inSize = IN.size();
            mkSymbol(IN);
            mkNb();
            prepareEncodingTable();
        }

        void mkSymbol(std::string input) {
            std::map<int, int> counts;
            for (char c : input) {
                int digit = c - '0';
                counts[digit]++;
            }
            std::cerr << "\nCOUNTS:\n";
            for (const auto& pair : counts) 
                std::cerr << pair.first << ": " << pair.second << std::endl;

            // Wybór R i obliczenie L
            R = 4; // chooseOptimalR(input, 0.1); // maxError = 0.01
            L = std::pow(2, R);

            // Skalowanie częstotliwości
            int totalFrequency = 0;
            for (const auto& pair : counts) {
                int f = std::round(pair.second * L / input.size());
                if (f == 0) f = 1;  // Upewnij się, że częstotliwość nie jest zerowa
                frequency[pair.first] = f;
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

            // Generowanie symboli
            std::vector<int> temp;
            for (const auto& pair : frequency) {
                for (int k = 0; k < pair.second; k++) {
                    temp.push_back(pair.first);
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
            int numSymbols = frequency.size();
            std::map<int, int> counts;

            // Zliczanie wystąpień symboli
            for (char c : input) {
                int digit = c - '0';
                counts[digit]++;
            }
            int sumFrequency = 0;
            for (int R = 4; R <= 16; ++R) {
                L = std::pow(2, R);

                //if (L < numSymbols) continue; // Upewnij się, że L >= liczba symboli
                
                sumFrequency = 0;

                for (const auto& [symbol, count] : counts) {
                    int f = std::round(count * L)/input.size();
                    if (f == 0) f = 1;  // Upewnij się, że częstotliwość nie jest zerowa
                    frequency[symbol] = f;
                    sumFrequency += f;
                }

                //std::cerr << "before correction\n";

                // Oblicz błąd odwzorowania
                double error = 0.0;
                for (auto const& pair: frequency) {
                    double Ps = static_cast<double>(counts[pair.first]) / input.size();
                    double approxPs = static_cast<double>(frequency[pair.first]) / L;
                    error += std::abs(Ps - approxPs);
                }

                if (error <= maxError) {
                    while (sumFrequency < L) {
                        auto maxElement = std::max_element(frequency.begin(), frequency.end(),
                                                        [](const auto& a, const auto& b) { return a.second < b.second; });
                        maxElement->second++;
                        sumFrequency++;
                    }

                    while (sumFrequency > L) {
                        auto minElement = std::min_element(frequency.begin(), frequency.end(),
                                                        [](const auto& a, const auto& b) { return a.second < b.second; });
                        if (minElement->second > 1) {
                            minElement->second--;
                            sumFrequency--;
                        }
                    }
                    sumFrequency = 0;
                    for (const auto& [symbol, f] : frequency) {
                        sumFrequency += f;
                    }
                    if (sumFrequency != L) {
                        std::cerr << "Error: sumFrequency = " << sumFrequency << " but L = " << L << "\n";
                        exit(EXIT_FAILURE);
                    }
                    std::cerr << "counts, len, f_s, L for 2" << counts[2] << ", " << input.size() << ", " << frequency[2]  << ", " << L << std::endl;
                    return R;
                }

                
            }
                while (sumFrequency < L) {
                    auto maxElement = std::max_element(frequency.begin(), frequency.end(),
                                                    [](const auto& a, const auto& b) { return a.second < b.second; });
                    maxElement->second++;
                    sumFrequency++;
                }

                while (sumFrequency > L) {
                    auto minElement = std::min_element(frequency.begin(), frequency.end(),
                                                    [](const auto& a, const auto& b) { return a.second < b.second; });
                    if (minElement->second > 1) {
                        minElement->second--;
                        sumFrequency--;
                    }
                }
            sumFrequency = 0;
            for (const auto& [symbol, f] : frequency) {
                sumFrequency += f;
            }
            if (sumFrequency != L) {
                std::cerr << "Error: sumFrequency = " << sumFrequency << " but L = " << L << "\n";
                exit(EXIT_FAILURE);
            }
            std::cerr << "counts, len, f_s, L for 2" << counts[2] << ", " << input.size() << ", " << frequency[2]  << ", " << L << std::endl;
            return 16; // Domyślnie największe R
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

            std::cerr << "\nSTART:\n";
            for(const auto& pair : start) std::cerr << pair << std::endl;
            std::cerr << "\npreparing encoding table:\n" << std::endl;
            for(int x = L; x < 2*L; x++)
            {
                //std::cerr << "x = " << x <<  ", L = " << L << std::endl;
                //std::cerr << symbol[x - L] << std::endl;
                int s = symbol[x-L];
                int index = start[s] + (next[s])++;
                std::cerr << "index: " << index << std::endl;
                encodingTable[index] = x;
            }
            std::cerr << "\nENCODINGTABLE:\n";
            for(auto u : encodingTable)
            {
                std::cerr << u << std::endl;
            }

        }

        int useBits(int x, int bit)
        {
            return 1;
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
            return 0;
        }

        void Print()
        {
            std::cout << "\nSYMBOL:\n";
            for(auto u: symbol)
            {
                std::cout << u << " ";
            }
            std::cout << "\n\nFREQ:\n";
            for(const auto& pair: frequency)
            {
                std::cout << pair.first << ": " << pair.second << std::endl;
            }
            std::cout << symbol[symbol.size()-2] << std::endl;
            std::cout << L << std::endl;
        }

    private:
        std::map<int, int> frequency;
        std::vector<int> encodingTable;
        std::vector<int> start;
        std::vector<int> symbol;
        std::vector<int> nbBits;
        std::vector<int> nb;
        std::vector<int> k;
        int L, R, inSize;
};


int main(int argc, char *argv[])
{
    std::string IN = argv[1];
    std::cout << IN << std::endl;
    ANS a(IN);
    a.Print();
}