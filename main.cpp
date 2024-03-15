#include <iostream>
#include <fstream>

struct Arguments {
    std::ifstream file;
    char* path_to_save;
    long max_iterations;
    long frequency;
    int16_t table_lenght;
    int16_t table_width;
    int16_t quantity_to_zero_by_lenght;
    int16_t quantity_to_zero_by_wight;
};

Arguments ParseArguments(const int argc, char** argv) {
    Arguments args;
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-i") == 0) {
            ++i;
            args.file.open(argv[i]);
            if (args.file.fail()) {
                std::cerr << "Не удаётся открыть файл с именем " << argv[i];
                exit(EXIT_FAILURE);
            }
        } else if (strncmp(argv[i], "--input", 6) == 0) {
            args.file.open(argv[i] + 8);
            if (args.file.fail()) {
                std::cerr << "Не удаётся открыть файл с именем " << (argv[i] + 8);
                exit(EXIT_FAILURE);
            }
        } else if (strcmp(argv[i], "-o") == 0) {
            ++i;
            args.path_to_save = argv[i];
        } else if (strncmp(argv[i], "--output", 7) == 0) {
            args.path_to_save = argv[i] + 9;
        } else if (strcmp(argv[i], "-m") == 0) {
            ++i;
            args.max_iterations = std::stoull(argv[i]);
        } else if (strncmp(argv[i], "--max-iter", 9) == 0) {
            args.max_iterations = std::stoull(argv[i] + 11);
        } else if (strcmp(argv[i], "-f") == 0) {
            ++i;
            args.frequency = std::stoull(argv[i]);
        } else if (strncmp(argv[i], "--freq", 5) == 0) {
            args.frequency = std::stoull(argv[i] + 7);
        }
    }
    return args;
}

char* ConcatenateTwoCharArrays(char* first, char* second) {
    char* startFirst = first;
    int firstLenght = 0;
    while (*first++) {
        ++firstLenght;
    }
    first = startFirst;

    char* startSecond = second;
    int secondLenght = 0;
    while (*second++) {
        ++secondLenght;
    }
    second = startSecond;

    char* result = new char[firstLenght + secondLenght + 1];

    int index = 0;
    while (*first) {
        result[index++] = *first++;
    }
    while (*second) {
        result[index++] = *second++;
    }

    result[firstLenght + secondLenght] = '\0';

    return result;
}

char* AppendCharacterToCharArray(char* array, char character) {
    char* startArray = array;
    int arrayLenght = 0;
    while (*array++) {
        ++arrayLenght;
    }
    array = startArray;

    char* result = new char[arrayLenght + 2];

    int index = 0;
    while (*array) {
        result[index++] = *array++;
    }
    result[index++] = character;
    result[index] = '\0';
    return result;
}

void GetTableSize(Arguments& args) {
    int16_t max_width = -32768; // Минимальное значение, входящее в int16_t. От него будеи искать max_width
    int16_t min_width = 32767; // Максимальное значение, входящее в int16_t. От него будеи искать min_width
    int16_t max_length = -32768; // Минимальное значение, входящее в int16_t. От него будеи искать max_length
    int16_t min_length = 32767; // Максимальное значение, входящее в int16_t. От него будеи искать min_length

    // Сбрасываем позицию в файле и снова читаем
    args.file.clear();
    args.file.seekg(0);

    char character;
    int count_of_cell = 0;
    char* numstr = "";

    // Читаем файл посимвольно
    while (args.file.get(character)) {
        if ((character >= '0' and character <= '9') or (character == '-')) {
            if (count_of_cell % 3 == 0) {
                numstr = AppendCharacterToCharArray(numstr, character);
                if (!(args.file.peek() >= '0' and args.file.peek() <= '9')) {
                    int16_t num = std::stoi(numstr);
                    max_length = std::max(max_length, num);
                    min_length = std::min(min_length, num);
                    numstr = "";
                    ++count_of_cell;
                }
            } else if (count_of_cell % 3 == 1) {
                numstr = AppendCharacterToCharArray(numstr, character);
                if (!(args.file.peek() >= '0' and args.file.peek() <= '9')) {
                    int16_t num = std::stoi(numstr);
                    max_width = std::max(max_width, num);
                    min_width = std::min(min_width, num);
                    numstr = "";
                    ++count_of_cell;
                }
            } else {
                if (!(args.file.peek() >= '0' and args.file.peek() <= '9')) {
                    ++count_of_cell;
                }
            }
        }
    }

    args.quantity_to_zero_by_wight = 1 - min_width;
    args.table_width = max_width + args.quantity_to_zero_by_wight;

    args.quantity_to_zero_by_lenght = 1 - min_length;
    args.table_lenght = max_length + args.quantity_to_zero_by_lenght;
}

uint64_t** CreateSandpileArray(Arguments& args) {
    // Выделяем память для массива и заполняем его нулями
    uint64_t** result = new uint64_t*[args.table_width];
    for (int16_t line = 0; line < args.table_width; ++line) {
        result[line] = new uint64_t[args.table_lenght];
        for (int16_t column = 0; column < args.table_lenght; ++column) {
            result[line][column] = 0;
        }
    }

    // Сбрасываем позицию в файле и снова читаем
    args.file.clear();
    args.file.seekg(0);

    char character;
    int count_of_cell = 0;
    char* numstr = "";
    int16_t x;
    int16_t y;

    // Читаем файл посимвольно
    while (args.file.get(character)) {
        if ((character >= '0' and character <= '9') or (character == '-')) {
            if (count_of_cell % 3 == 0) {
                numstr = AppendCharacterToCharArray(numstr, character);
                if (!(args.file.peek() >= '0' and args.file.peek() <= '9')) {
                    x = std::stoi(numstr) + args.quantity_to_zero_by_lenght;
                    numstr = "";
                    ++count_of_cell;
                }
            } else if (count_of_cell % 3 == 1) {
                numstr = AppendCharacterToCharArray(numstr, character);
                if (!(args.file.peek() >= '0' and args.file.peek() <= '9')) {
                    y = std::stoi(numstr) + args.quantity_to_zero_by_wight;
                    numstr = "";
                    ++count_of_cell;
                }
            } else if (count_of_cell % 3 == 2) {
                numstr = AppendCharacterToCharArray(numstr, character);
                if (!(args.file.peek() >= '0' and args.file.peek() <= '9')) {
                    int64_t value = std::stoull(numstr);
                    result[y - 1][x - 1] = value;
                    numstr = "";
                    ++count_of_cell;
                }
            }
        }
    }

    return result;
}

char* CreateOutputFileName(Arguments& args, long iteration) {
    if (iteration == -1) {
        return ConcatenateTwoCharArrays(args.path_to_save, "/result.bmp");
    } else {
        int iterationLength = 0;
        int tempNum = iteration;
        do {
            iterationLength++;
            tempNum /= 10;
        } while (tempNum > 0);

        char *numStr = new char[iterationLength + 1];
        numStr[iterationLength] = '\0';

        tempNum = iteration;
        for (int i = iterationLength - 1; i >= 0; i--) {
            numStr[i] = '0' + (tempNum % 10);
            tempNum /= 10;
        }

        char* filename = ConcatenateTwoCharArrays("/iteration", numStr);
        delete[] numStr;
        filename = ConcatenateTwoCharArrays(filename, ".bmp");
        return ConcatenateTwoCharArrays(args.path_to_save, filename);
    }
}

void CreateBMP(Arguments& args, uint64_t**& sandpile, long iteration) {

#pragma pack(push, 1)

    struct BmpHeader {
        char bitmapSignatureBytes[2] = {'B', 'M'};
        uint32_t sizeOfBitmapFile;
        uint16_t reserved1 = 0;
        uint16_t reserved2 = 0;
        uint32_t pixelDataOffset = 54 + 4*5; // 54 заголовок + (5 цветов * 4 бита на пиксель)
    };

    struct BmpInfoHeader {
        uint32_t size = 40;
        int32_t width;
        int32_t height;
        uint16_t planes = 1;
        uint16_t bitsPerPixel = 4;
        uint32_t compression = 0;
        uint32_t sizeOfBitmap;
        int32_t horzResolution = 2835;
        int32_t vertResolution = 2835;
        uint32_t colorsUsed = 5;
        uint32_t colorsImportant = 5;
    };

#pragma pack(pop)

    const int paddingSize = (4 - ((args.table_lenght + 1) / 2) % 4) % 4;

    BmpHeader bmpHeader;
    bmpHeader.sizeOfBitmapFile = sizeof(BmpHeader) + sizeof(BmpInfoHeader) + 4*5 + ((args.table_lenght * args.table_width / 2) + paddingSize * args.table_width);

    BmpInfoHeader bmpInfoHeader;
    bmpInfoHeader.width = args.table_lenght;
    bmpInfoHeader.height = args.table_width;
    bmpInfoHeader.sizeOfBitmap = ((args.table_lenght * args.table_width) / 2) + paddingSize * args.table_width;

    char pixels[args.table_lenght * args.table_width]; // Массив для хранения данных о цвете каждого пикселя
    for (int i = 0; i < args.table_width; ++i) {
        for (int j = 0; j < args.table_lenght; ++j) {
            long value = sandpile[i][j];
            pixels[i * args.table_lenght + j] = value >= 0 and value < 4 ? value : 4;
        }
    }

    // Создаём output файл
    char* filename = CreateOutputFileName(args, iteration);
    std::ofstream file(filename, std::ios::binary);
    delete[] filename;

    // Записываем згаловки
    file.write((char*)&bmpHeader, sizeof(BmpHeader));
    file.write((char*)&bmpInfoHeader, sizeof(BmpInfoHeader));

    // Записываем палитру цветов
    file.put(255);   file.put(255);   file.put(255);   file.put(0);    // Белый
    file.put(0);     file.put(255);   file.put(0);     file.put(0);    // Зелёный
    file.put(0);     file.put(255);   file.put(255);   file.put(0);    // Жёлтый
    file.put(128);   file.put(0);     file.put(128);   file.put(0);    // Фиолетовый
    file.put(0);     file.put(0);     file.put(0);     file.put(0);    // Чёрный

    // Записывем изображение (по 2 пикселя в 1 байт)
    for (int i = args.table_width - 1; i >= 0; --i) { // Перебираем строки
        for (int j = 0; j < (args.table_lenght + 1) / 2; ++j) { // Перебираем столбцы с шагом 2, так как в теле делаем проверку на след. столбец
            char byte = (pixels[i * args.table_lenght + j * 2] << 4) | (j * 2 + 1 < args.table_lenght ? pixels[i * args.table_lenght + j * 2 + 1] : 0);
            file.write(&byte, 1);
        }
        for(int j = 0; j < paddingSize; ++j)
            file.put(0);
    }

    file.close();
}

uint64_t** Expansion(Arguments& args, uint64_t**& sandpile, bool zoom_up, bool zoom_down, bool zoom_left, bool zoom_right) {
    uint64_t** expansioned_sandpile = new uint64_t*[args.table_width + zoom_up + zoom_down];

    for (int16_t line = 0; line < args.table_width + zoom_up + zoom_down; ++line) {
        expansioned_sandpile[line] = new uint64_t[args.table_lenght + zoom_left + zoom_right]();

        if (line >= zoom_up && line < args.table_width + zoom_up) {
            for (int16_t column = zoom_left; column < args.table_lenght + zoom_left; ++column) {
                expansioned_sandpile[line][column] = sandpile[line - zoom_up][column - zoom_left];
            }
        }
    }

    for (int16_t i = 0; i < args.table_width; ++i) {
        delete sandpile[i];
    }
    delete[] sandpile;

    args.table_width += (zoom_up + zoom_down);
    args.table_lenght += (zoom_left + zoom_right);

    return expansioned_sandpile;
}

void CrumbleSandpile(Arguments& args, uint64_t**& referenceToSandpile) {
    long iteration = 0;
    int16_t line = 0;
    int16_t column = 0;
    bool is_iteration_done;
    while (true) {
        is_iteration_done = false;
        while (true) {
            if (referenceToSandpile[line][column] > 3) {
                bool zoom_up = (line == 0);
                bool zoom_down = (line == args.table_width - 1);
                bool zoom_left = (column == 0);
                bool zoom_right = (column == args.table_lenght - 1);
                if (zoom_up or zoom_down or zoom_left or zoom_right) {
                    uint64_t** sandpile = Expansion(args, referenceToSandpile, zoom_up, zoom_down, zoom_left, zoom_right);
                    referenceToSandpile = sandpile;
                }

                referenceToSandpile[line + zoom_up][column + zoom_left] -= 4;
                ++referenceToSandpile[line - 1 + zoom_up][column + zoom_left];
                ++referenceToSandpile[line + 1 + zoom_up][column + zoom_left];
                ++referenceToSandpile[line + zoom_up][column - 1 + zoom_left];
                ++referenceToSandpile[line + zoom_up][column + 1 + zoom_left];

                ++iteration;

                if (iteration == args.max_iterations) {
                    CreateBMP(args, referenceToSandpile, -1);
                    exit(0);
                }

                if (args.frequency != 0 and iteration % args.frequency == 0) {
                    CreateBMP(args, referenceToSandpile, iteration);
                }

                is_iteration_done = true;

                line = line - 1 + zoom_up;
                column = column + zoom_left;
                break;
            }
            ++column;
            if (column >= args.table_lenght) {
                break;
            }
        }
        if (!is_iteration_done) {
            ++line;
            column = 0;
        }
        if (line >= args.table_width) {
            break;
        }
    }

    CreateBMP(args, referenceToSandpile, -1);
}

int main(const int argc, char* argv[]) {
    Arguments args = ParseArguments(argc, argv);
    GetTableSize(args);
    uint64_t** sandpile = CreateSandpileArray(args);
    uint64_t**& referenceToSandpile = sandpile;
    CrumbleSandpile(args, referenceToSandpile);
    return 0;
}
