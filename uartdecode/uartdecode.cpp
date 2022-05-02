// uartdecode.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <string>

#include <stdio.h>

#pragma warning(disable : 4996)

const char STX = '\2';


char Construct_BE_Byte(std::vector<uint8_t>& bitstream, int start) {
    uint8_t ret = 0;

    if (start + 9 >= bitstream.size()) {
        return 0xFF;
    }

    for (int i = 0; i < 8; i++) {
        ret |= bitstream.at(start + i) << i;
    }

    return (char)~ret;
}

int Validate_UART(std::vector<uint8_t>& bitstream) {
    int start = 0, end = 9;
    char data = 0;
    if (end >= bitstream.size()) {
        return -1;
    }

    while (end < bitstream.size()) {
        if (!(~bitstream.at(start) & bitstream.at(end))) {
            if (Construct_BE_Byte(bitstream, start + 1) == STX) {
                break;
            }
        }
        start++;
        end++;
    }
    if (end >= bitstream.size()) {
        return -1;
    }
    return start + 10;
}

void Print_UART(std::vector<uint8_t>& bitstream, int start) {
    char buffer = 0;
    for (int i = start + 1; i < bitstream.size(); i += 10) {
        buffer = Construct_BE_Byte(bitstream, i);
        if (buffer != '\r') {
            printf("%c", buffer);
        }
        
    }
    printf("\n\n");
    fflush(stdout);
}

// Frame alignment that uses organic machine learning to choose the correct frame.
int Align_Frame(std::vector<uint8_t>& bitstream) {
    int start = 0;
    for (int i = 0; i <= 9; i++) {
        printf("%d: ", i);
        
        Print_UART(bitstream, i);
    }
    printf("Choose the option that looks the best:\n> ");
    std::cin >> start;
    printf("Option %d has been selected\n\n", start);
    return start + 10;
}

int main(int argc, char* argv[])
{
    char* FILENAME = argv[1];
    FILE* fd;
    char buffer = 0x7F;
    std::vector<uint8_t> bitstream;
    int start = -2;



    //std::cout << "Hello World!\n";

    if (argc < 2) {
        std::cerr << "Usage: uartdecode \'file_to_decode\'" << std::endl;
        return -1;
    }

    fd = fopen(FILENAME, "r");

    if (!fd) {
        std::cerr << "ERROR: Bad filename: " << FILENAME << std::endl;
        return -2;
    }

    while (!feof(fd)) {
        buffer = fgetc(fd);
        if (!((buffer ^ 0x30) >> 1) || !(buffer >> 1)) {
            bitstream.push_back(buffer & 0x01);
        }
    }

    if (fclose(fd)) {
        std::cerr << "ERROR: Could not close file: " << FILENAME << std::endl;
        return -3;
    }

    for (auto& i : bitstream) {
        //printf("%d", i);
    }
    //printf("\n");

    start = Validate_UART(bitstream);
    if (start == -1) {
        //TODO: FUNCTION TO ALIGN THE FRAMES
        std::cerr << "ERROR: No start byte (START OF TEXT) found" << std::endl;
        return -4;
    }

    start = Align_Frame(bitstream);

    for (int i = start+1; i < bitstream.size(); i += 10) {
        buffer = Construct_BE_Byte(bitstream, i);
        printf("%c", buffer);
    }
    printf("\n");

    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
