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
        }

        void mkSymbol(std::string input) {
            // Wybór optymalnego R
            R = chooseOptimalR(input, 0.01);  // maxError = 0.01
            L = std::pow(2, R);
            std::cerr << L << " halo\n";
            // Skalowanie częstotliwości
            // Korekta liczności
            std::cerr << "before spread\n";
            spread();  // Rozmieszczenie symboli
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
            for (int R = 2; R <= 16; ++R) {
                L = std::pow(2, R);

                if (L < numSymbols) continue; // Upewnij się, że L >= liczba symboli
                double scalingFactor = static_cast<double>(L) / input.size();
                
                sumFrequency = 0;

                for (const auto& [symbol, count] : counts) {
                    int f = std::round(count * scalingFactor);
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


        int useBits(int x, int bit)
        {
            return 1;
        }

        void mkNb()
        {
            std::vector<int> k;
            for(int i=0; i<10; i++)
            {
                k.push_back(0);
                nb.push_back(0);
            }
            for(const auto& s: frequency)
            {
                //k[s.first] = 
            }
        }

        int encode(int s, int x, int r, int start[])
        {
            int nbBits = (x + nb[s]) >> r;
            useBits(x, nbBits);
            x = encodingTable[start[s] + (x >> nbBits)];
            return 0;
        }

        void Print()
        {
            for(auto u: symbol)
            {
                std::cout << u << " ";
            }
            std::cout << std::endl;
        }
    private:
        std::map<int, int> frequency;
        std::vector<int> encodingTable;
        std::vector<int> start;
        std::vector<int> symbol;
        std::vector<int> nbBits;
        std::vector<int> nb;
        int L, R, inSize;
};


int main(int argc, char *argv[])
{
    std::string IN = argv[1];
    std::cout << IN << std::endl;
    ANS a(IN);
    a.Print();
}