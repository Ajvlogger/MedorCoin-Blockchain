#pragma once

#include <string>
#include <leveldb/db.h>
#include "block.h"

/*
  BlockDB is the persistent database
  that stores blocks on disk using LevelDB.
  Each block is stored by its hash.
*/
class BlockDB {
private:
    leveldb::DB* db;  // pointer to LevelDB instance

public:
    // Constructor
    BlockDB();

    // Destructor
    ~BlockDB();

    // Open or create the database at path
    bool open(const std::string& path);

    // Close the database cleanly
    void close();

    // Write a block to the DB
    bool writeBlock(const Block& block);

    // Read a block from the DB by hash
    bool readBlock(const std::string& hash, Block& blockOut);

    // Check if the DB has a block by hash
    bool hasBlock(const std::string& hash);
};
