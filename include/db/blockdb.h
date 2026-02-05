#pragma once

#ifndef BLOCKDB_H
#define BLOCKDB_H

#include <string>
#include "block.h" // Make sure this defines your Block class with serialize()/deserialize() and hash

class BlockDB {
public:
    BlockDB();
    ~BlockDB();

    // Open or create the LevelDB database at the given path
    bool open(const std::string& path);

    // Close the database
    void close();

    // Write a block to the database
    bool writeBlock(const Block& block);

    // Read a block from the database
    bool readBlock(const std::string& hash, Block& blockOut);

    // Check if a block exists
    bool hasBlock(const std::string& hash);

private:
    // Pointer to LevelDB database
    class leveldb::DB* db;
};

#endif // BLOCKDB_H
