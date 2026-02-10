#include "bytecode.h"
#include <fstream>

std::vector<uint8_t> loadContractBytecode(const std::string& path)
{
    std::ifstream file(path, std::ios::binary);
    return std::vector<uint8_t>((std::istreambuf_iterator<char>(file)),
                                std::istreambuf_iterator<char>());
}
