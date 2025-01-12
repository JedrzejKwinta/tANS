#include <iostream>
#include <algorithm>
#include <numeric>
#include <vector>
#include <map>
#include <cmath>
#include <random>
#include <string>
#include <fstream>
#include <sstream>
#include <bitset>
#include <deque>

class ANS {
    public:
        ANS() {}

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

            for (int R = 8; R <= 8; ++R) {
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
            return 8;
        }

        void spread() {
            int step = std::floor((5.0 / 8.0) * L) + 3;

            // Upewnij się, że step i L są względnie pierwsze
            while (std::gcd(step, L) != 1) {
                ++step;
            }
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

        void prepareEncodingTable(std::string IN) {
            INPUT = IN;
            inSize = IN.size();
            mkSymbol(IN);
            mkNb();
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
            OUTPUT = std::bitset<32>(extractedBits).to_string().substr(32-nbBits);
            std::reverse(OUTPUT.begin(), OUTPUT.end());
            for(char c : OUTPUT)
            {
                outputDeque.push_front(c);
            }
            // Przykład: dodanie do strumienia bitowego (tu tylko symulacja)
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

        int encode(int s, int x, int r)
        { 
            int nbBits = (x + nb[s]) >> r;
            useBits(x, nbBits);
            x = encodingTable[start[s] + (x >> nbBits)];
            return x;
        }

        std::string encodeString(const std::string& input, std::string FILEN) {
            OUTPUT="";
            prepareEncodingTable(input);
            int x = L; // Stan początkowy, zwykle równy \( L \).
            //std::cout << "L: " << L <<std::endl;
            int i = 0;
            int r = R + 1;
            for (char c : input) {
                int s = c - '0';
                int dots = (i / 30000) % 3 + 1;

                std::cout << "\r\tKompresowanie";
                for (int j = 0; j < dots; ++j) {
                    std::cout << ".";
                }
                std::cout << "     ";
                std::cout << std::flush;
                i++;
                int newX = encode(s, x, r); // Kodowanie symbolu
                x = newX;
                
            }
            finalState = x;
            //std::cout << "\n\tFinal state: " << x << "\n\n" << std::endl;
            /*for(auto u: outputDeque)
                std::cout << u;
            std::cout << std::endl;*/
            std::string OUT(outputDeque.begin(), outputDeque.end());
            FILEN = getCompressedFileName(FILEN);
            saveDataCompressed(OUT, finalState, symbol, FILEN);
            return OUT;
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
            std::map<int, int> mapa;
            for (int c : sym) {
                mapa[c]++;
            }

            for (const auto& pair : mapa) {
                next[pair.first] = pair.second;
            }
            decodingTable.resize(L);

            std::map<int, int> counters;
            for(const auto& u: mapa)
            {
                counters[u.first] = 0;
            }

            for(int i = 0; i < L; i++)
            {
                DecodingEntry t;
                t.symbol = sym[i];
                t.L_s = mapa[t.symbol];
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

        int readBits(int bits, int& current) {
            if (bits > bitStream.size()) {
                throw std::out_of_range("Not enough bits in the stream!");
            }

            int result = 0;

            // Konwersja pierwszych `bits` bitów na liczbę dziesiętną
            std::vector<int> output;

            for(int i = 0; i < bits; i++)
            {
                output.push_back(bitStream[current]);
                current++;
            }

            for (int bit : output) {
                result = (result << 1) | bit;
            }
            return result;
        }

        int decode(int state, int& index) {
            int X = state;
            DecodingEntry t = decodingTable[X];
            useSymbol(t.symbol);
            int x = t.newX + readBits(t.nbBits, index);
            return x;
        }

        std::vector<int> decodeStream(std::string FILEN) {
            int inState;
            std::string pimpek;
            loadDataCompressed(pimpek, inState, sym, FILEN, L);
            R = std::log2(L);
            prepareDecodingTable();
            state = inState - L;
            bitStream.clear();

            for(auto u: pimpek)
            {
                bitStream.push_back(u - '0');
            }

            int current =0;
            int i =0;
            while (current < bitStream.size()) {
                try {
                    // Dekoduj pojedynczy krok
                    int newState = decode(state, current);
                    int dots = (i / 30000) % 3 + 1;

                    // Wypisz komunikat z odpowiednią liczbą kropek
                    std::cout << "\r\tDekompresowanie";
                    for (int j = 0; j < dots; ++j) {
                        std::cout << ".";
                    }
                    std::cout << "     ";
                    std::cout << std::flush;
                    i++;
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
            std::string toFile = "";
            for(int u: decoded)
            {
                toFile += std::to_string(u);
            }
            std::string OUTFILEN = getDecompressedFileName(FILEN);
            saveStringToFile(toFile, OUTFILEN);
            return decoded;
        }

        std::string getDecompressedFileName(const std::string& fileName) {
            std::string newFileName = fileName;

            // Znajdujemy ostatnią kropkę (jeśli istnieje)
            size_t dotPos = newFileName.find_last_of('.');
            if (dotPos != std::string::npos) {
                // Zamieniamy końcówkę
                newFileName.replace(dotPos, std::string::npos, "_decompressed.txt");
            } else {
                // Jeśli brak rozszerzenia, dodajemy końcówkę
                newFileName += "_decompressed.txt";
            }

            return newFileName;
        }

        std::string getCompressedFileName(const std::string& fileName) {
            std::string newFileName = fileName;

            // Znajdujemy ostatnią kropkę (jeśli istnieje)
            size_t dotPos = newFileName.find_last_of('.');
            if (dotPos != std::string::npos) {
                // Zamieniamy końcówkę
                newFileName.replace(dotPos, std::string::npos, "_compressed.bin");
            } else {
                // Jeśli brak rozszerzenia, dodajemy końcówkę
                newFileName += "_compressed.bin";
            }

            return newFileName;
        }

        void Print()
        {
            /*std::cerr << "\nCOUNTS:\n";
            for (const auto& pair : counts) 
                std::cerr << "\t" << pair.first << ": " << pair.second << std::endl;
            

            */std::cout << "\nSYMBOL:\n";
            for(auto u: symbol)
            {
                std::cout << u << " ";
            }
            std::cout << std::endl;/*

            std::cout << "\nFREQ:\n";
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

            */std::cerr << "\n\nDECODINGTABLE:\n";
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

        void saveDataCompressed(const std::string& bitString, int finalState, const std::vector<int>& symbol, const std::string& fileName) {
            std::ofstream outFile(fileName, std::ios::binary);
            if (!outFile) {
                std::cerr << "Nie można otworzyć pliku do zapisu." << std::endl;
                return;
            }

            // Zapis długości ciągu bitów
            size_t bitLength = bitString.size();
            outFile.write(reinterpret_cast<const char*>(&bitLength), sizeof(bitLength));

            // Obliczanie paddingu i konwersja bitów na bajty
            uint8_t padding = (bitLength % 8 == 0) ? 0 : (8 - (bitLength % 8));
            std::string paddedBitString = bitString + std::string(padding, '0');

            std::vector<uint8_t> bytes;
            for (size_t i = 0; i < paddedBitString.size(); i += 8) {
                std::bitset<8> byte(paddedBitString.substr(i, 8));
                bytes.push_back(static_cast<uint8_t>(byte.to_ulong()));
            }

            // Zapis bitów jako bajty
            outFile.write(reinterpret_cast<const char*>(bytes.data()), bytes.size());

            // Zapis finalState
            outFile.write(reinterpret_cast<const char*>(&finalState), sizeof(finalState));

            // Zapis rozmiaru symbol
            uint32_t symbolSize = symbol.size();
            outFile.write(reinterpret_cast<const char*>(&symbolSize), sizeof(symbolSize));

            // Zapis cyfr z symbol w formie skompresowanej
            std::vector<uint8_t> compressedSymbols;
            for (size_t i = 0; i < symbol.size(); i += 2) {
                uint8_t compressed = (symbol[i] << 4); // Przesunięcie pierwszej cyfry do wyższych 4 bitów
                if (i + 1 < symbol.size()) {
                    compressed |= symbol[i + 1]; // Dodanie drugiej cyfry do niższych 4 bitów
                }
                compressedSymbols.push_back(compressed);
            }
            outFile.write(reinterpret_cast<const char*>(compressedSymbols.data()), compressedSymbols.size());

            outFile.close();
            std::cout << "\n\tDane zostały zapisane do pliku: " << fileName << std::endl;
        }

        void loadDataCompressed(std::string& bitString, int& finalState, std::vector<int>& symbol, const std::string& fileName, int& len) {
            std::ifstream inFile(fileName, std::ios::binary);
            if (!inFile) {
                std::cerr << "Nie można otworzyć pliku do odczytu." << std::endl;
                return;
            }

            // Odczyt długości ciągu bitów
            size_t bitLength;
            inFile.read(reinterpret_cast<char*>(&bitLength), sizeof(bitLength));

            // Obliczenie liczby bajtów
            size_t bitBytesLength = (bitLength + 7) / 8;

            // Odczyt bajtów
            std::vector<uint8_t> bytes(bitBytesLength);
            inFile.read(reinterpret_cast<char*>(bytes.data()), bitBytesLength);

            // Rekonstrukcja ciągu bitów
            bitString.clear();
            for (uint8_t byte : bytes) {
                std::bitset<8> bits(byte);
                bitString += bits.to_string();
            }
            bitString.erase(bitString.begin() + bitLength, bitString.end());

            // Odczyt finalState
            inFile.read(reinterpret_cast<char*>(&finalState), sizeof(finalState));

            // Odczyt rozmiaru symbol
            uint32_t symbolSize;
            inFile.read(reinterpret_cast<char*>(&symbolSize), sizeof(symbolSize));
            len = symbolSize;

            // Odczyt cyfr z symbol w formie skompresowanej
            size_t compressedSize = (symbolSize + 1) / 2; // Liczba bajtów potrzebnych do przechowania cyfr
            std::vector<uint8_t> compressedSymbols(compressedSize);
            inFile.read(reinterpret_cast<char*>(compressedSymbols.data()), compressedSize);

            symbol.clear();
            for (uint8_t compressed : compressedSymbols) {
                // Wyodrębnianie pierwszej i drugiej cyfry
                symbol.push_back((compressed >> 4) & 0x0F); // Wyższe 4 bity
                if (symbol.size() < symbolSize) {
                    symbol.push_back(compressed & 0x0F); // Niższe 4 bity
                }
            }

            inFile.close();
            std::cout << "\n\n\tDane zostały odczytane z pliku: " << fileName << std::endl << std::endl;
        }

        void saveStringToFile(const std::string& data, const std::string& fileName) {
            std::ofstream outFile(fileName);
            if (!outFile) {
                std::cerr << "Nie można otworzyć pliku do zapisu." << std::endl;
                return;
            }

            outFile << data; // Zapisujemy dane do pliku
            outFile.close();

            std::cout << "\n\tDane zostały zapisane do pliku: " << fileName << std::endl << std::endl; 
        }

        std::string loadStringFromFile(std::string& data, const std::string& fileName) {
            std::ifstream inFile(fileName);
            if (!inFile) {
                std::cerr << "Nie można otworzyć pliku do odczytu." << std::endl;
                return NULL;
            }

            std::ostringstream oss;
            oss << inFile.rdbuf(); // Wczytanie całej zawartości pliku do strumienia
            data = oss.str();      // Przypisanie zawartości do stringa

            inFile.close();

            std::cout << "Dane zostały odczytane z pliku: " << fileName << std::endl;
            return oss.str();
        }

        void EN(std::string F)
        {
            std::string I;
            std::ifstream inFile;
            inFile.open(F);
            std::getline(inFile, I);
            std::cout << "\n\tDane odczytane z pliku: " << F << std::endl << std::endl;
            encodeString(I, F);
        }

        void DE(std::string F)
        {
            decodeStream(F);
        }
    
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
        std::vector<int> sym;
        std::vector<int> decoded;
        std::deque<char> outputDeque;
        int L, R, inSize, finalState, state;
        std::string INPUT;
        std::string OUTPUT;
};

int main(int argc, char *argv[])
{
    std::string MODE = argv[1];
    std::string INPUT = argv[2];
    ANS a;

    switch(MODE[0])
    {
        case 'e':
            a.EN(INPUT);
            break;
        case 'd':
            a.DE(INPUT);
            break;
        default:
            std::cout << "Niepoprawny parametr MODE" <<std::endl;
            break;
    }
    //a.Print();
}