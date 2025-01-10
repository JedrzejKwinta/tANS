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
            R = chooseOptimalR(input, 0.01); // maxError = 0.1
            L = std::pow(2, R);

            // Generowanie symboli na podstawie częstotliwości
            std::vector<int> temp;
            for (const auto& [symbol, freq] : frequency) {
                for (int i = 0; i < freq; ++i) {
                    temp.push_back(symbol);
                }
            }

            // Tasowanie
            /*std::random_device rd;
            std::mt19937 g(rd());
            std::shuffle(temp.begin(), temp.end(), g);

            symbol = std::move(temp);*/

            spread();

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
            //std::cerr << "witam " << std::gcd(step, L) << " stepujący step: " << step << std::endl;
            // Wstępne wypełnienie symbolami
            std::vector<int> tempSymbols(L, -1);
            std::vector<int> temp;
            int i = 0;
            for (const auto& [symbol, count] : frequency) {
                for (int j = 0; j < count; j++) {
                    if (tempSymbols[i] != -1) {
                        std::cerr << "ERROR: Overwriting symbol at position " << i << "\n";
                        //exit(EXIT_FAILURE); // Wyjdź, jeśli wykryjesz problem
                    }
                    tempSymbols[i] = symbol;
                    i = (i + step) % L;
                }
            }
            symbol = std::move(tempSymbols);  // Przeniesienie do właściwego wektora
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

            /*eTable.resize(L);
            std::map<int, int> counters;
            for(const auto& u: frequency)
            {
                counters[u.first] = 0;
            }
            for(int i = 0; i < L; i++)
            {
                EncodingEntry t;
                t.symbol = prepared[i];
                t.L_s = frequency[t.symbol];
                //std::cerr << "preparing decoding, symbol[" << i << "]: " << symbol[i] << std::endl;
                int x = next[t.symbol]++;
                t.xtmp = t.L_s + counters[t.symbol];
                counters[t.symbol]++;

                std::cerr << "tmp: " << t.xtmp << " lolxd: " << ((i+L) >> 1) << std::endl;
                t.nbBits = (x + nb[t.symbol]) >> r;
                eTable[i] = t;
            }*/

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
            //encodingTable = {18, 22, 25, 16, 17, 21, 24, 27, 29, 30, 31, 19, 20, 23, 26, 28};
        }

        void useBits(int x, int nbBits) {
            // Wyciągnij nbBits najmniej znaczących bitów
            int extractedBits = x & ((1 << nbBits) - 1);

            // Tutaj można wykorzystać extractedBits, np. zapisać do strumienia wyjściowego.
            //std::cout << "Extracted bits: " << std::bitset<32>(extractedBits).to_string().substr(32-nbBits) << " (" << extractedBits << ")\n";
            OUTPUT = std::bitset<32>(extractedBits).to_string().substr(32-nbBits) + OUTPUT;
            // Przykład: dodanie do strumienia bitowego (tu tylko symulacja)
            for (int i = nbBits - 1; i >= 0; --i) {
                bitStream.insert(bitStream.begin(), (extractedBits >> i) & 1);
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

        std::string encodeString(const std::string& input) {
            int x = L; // Stan początkowy, zwykle równy \( L \).

            for (char c : input) {
                int s = c - '0'; // Zamiana znaku na cyfrę
                //std::cout << "ENCODING! STATE: " << x << std::endl;
                int newX = encode(s, x); // Kodowanie symbolu
                x = newX;
                
            }

            // Na koniec wypisz zakodowane bity.
           /*std::cout << "\n\tEncoded string in bits:\t";
            for (int bit : bitStream) {
                std::cout << bit;
            }*/
            finalState = x;
            std::cout << "\n\tFinal state: " << x << "\n\n" << std::endl;
            return OUTPUT;
        }

        struct DecodingEntry {
            int symbol;
            int xtmp;
            int L_s;
            int nbBits;
            int newX;
        };

        void prepareDecodingTable()
        {
            std::vector<int> next(10, -1);

            for (const auto& pair : frequency) {
                next[pair.first] = pair.second;
            }
            decodingTable.resize(L);

            std::map<int, int> counters;
            for(const auto& u: frequency)
            {
                counters[u.first] = 0;
            }

            for(int i = 0; i < L; i++)
            {
                DecodingEntry t;
                t.symbol = symbol[i];
                t.L_s = frequency[t.symbol];
                //std::cerr << "preparing decoding, symbol[" << i << "]: " << symbol[i] << std::endl;
                int x = next[t.symbol]++;
                t.xtmp = t.L_s + counters[t.symbol]++;
                

                t.nbBits = R - std::floor(std::log2(x));
                t.newX = (x << t.nbBits) - L;
                decodingTable[i] = t;
            }
        }

        void useSymbol(int s)
        {
            decoded.push_back(s);
            //std::cerr << s;
        }

        int readBits(int bits) {
            if (bits > bitStream.size()) {
                throw std::out_of_range("Not enough bits in the stream!");
            }

            int result = 0;

            // Konwersja pierwszych `bits` bitów na liczbę dziesiętną
            std::vector<int> output;

            for(int i = 0; i < bits; i++)
            {
                output.push_back(bitStream[0]);
                bitStream.erase(bitStream.begin());
            }

            for (int bit : output) {
                result = (result << 1) | bit;
            }

            /*std::cout << "Decoded: ";
            for(auto u: output)
            {
                std::cout << u;
            }
            std::cout << ", equals to: " << result << "\nRemaining bitStream:\n";
            for(auto u: bitStream)
                std::cout << u;
            std::cout << std::endl;*/

            

            return result;
        }

        int decode(int state) {
            int X = state;
            DecodingEntry t = decodingTable[X];
            //std::cout << "DECODING! STATE: " << X+L << ", nbBits: " << t.nbBits << std::endl;
            useSymbol(t.symbol);

            int x = t.newX + readBits(t.nbBits);
            //std::cout << "New state: " << x << std::endl << std::endl;
            return x;
        }

        void decodeStream(int initialState) {
            prepareDecodingTable();

            state = initialState - L;

            bitStream.clear();

            for(auto u: OUTPUT)
            {
                bitStream.push_back(u - '0');
            }

            while (!bitStream.empty()) {
                try {
                    // Dekoduj pojedynczy krok
                    int newState = decode(state);
                    state = newState;

                    // Sprawdzanie poprawności zakresu stanu (opcjonalne)
                    if (state < 0 || state >= L) {
                        throw std::out_of_range("Invalid state encountered during decoding!");
                    }
                } catch (const std::out_of_range& e) {
                    std::cerr << "Decoding error: " << e.what() << std::endl;
                    break;
                }
            }
            std::reverse(decoded.begin(), decoded.end());
            // Dekodowanie zakończone
            std::cout << "\n\tDECODED: ";
            for (int symbol : decoded) {
                std::cout << symbol;
            }
            std::cout << "\n\n" << std::endl;
        }


        void Print()
        {
            /*std::cerr << "\nCOUNTS:\n";
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
            }*/

            std::cerr << "\n\nDECODINGTABLE:\n";
            int i = 0;
            for(const auto& triplet: decodingTable)
            {
                std::cerr << "\tx, symbol, x_tmp, nbBits, newX: "  << i 
                << ", " << triplet.symbol << ", {" << triplet.L_s << ", " << (2*triplet.L_s) - 1 << "}"
                << ", " << triplet.xtmp << ", " << triplet.nbBits 
                << ", "  << triplet.newX << std::endl;
                i++;
            }
            std::cout << "\nL = " << L << std::endl;
        }

        int getState() {return finalState;}
    
    private:
        std::map<int, int> frequency;
        std::map<int, int> counts;
        std::vector<int> encodingTable;
        std::vector<DecodingEntry> decodingTable;
        std::vector<int> start;
        std::vector<int> symbol;
        std::vector<int> nbBits;
        std::vector<int> nb;
        std::vector<int> k;
        std::vector<int> bitStream;
        std::vector<int> decodingStream;
        std::vector<int> decoded;
        int L, R, inSize, finalState, state;
        std::string INPUT;
        std::string OUTPUT;
};


int main(int argc, char *argv[])
{
    std::string IN = argv[1];
    std::cout << "\n\n\tInput: " << IN << std::endl;
    ANS a(IN);
    std::string encoded =  a.encodeString(IN);
    std::cout << "\n\tENCODED: " << encoded << std::endl;
    a.decodeStream(a.getState());
    //a.Print();
}