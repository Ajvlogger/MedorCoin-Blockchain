#include "db/blockdb.h"

#include <leveldb/write_batch.h>
#include <iostream>

/*
  Constructor
*/
BlockDB::BlockDB() : db(nullptr) {}

/*
  Destructor
*/
BlockDB::~BlockDB() {
    close();
}

/*
  Open or create the LevelDB database
*/
bool BlockDB::open(const std::string& path) {
    leveldb::Options options;
    options.create_if_missing = true;

    leveldb::Status status = leveldb::DB::Open(options, path, &db);

    if (!status.ok()) {
        std::cerr << "BlockDB open failed: " << status.ToString() << std::endl;
        return false;
    }

    return true;
}

/*
  Close database
*/
void BlockDB::close() {
    if (db) {
        delete db;
        db = nullptr;
    }
}

/*
  Serialize and write a block
*/
bool BlockDB::writeBlock(const Block& block) {
    if (!db) return false;

    std::string key = block.hash;
    std::string value = block.serialize();

    leveldb::Status status = db->Put(
        leveldb::WriteOptions(),
        key,
        value
    );

    return status.ok();
}

/*
  Read and deserialize a block
*/
bool BlockDB::readBlock(const std::string& hash, Block& blockOut) {
    if (!db) return false;

    std::string value;
    leveldb::Status status = db->Get(
        leveldb::ReadOptions(),
        hash,
        &value
    );

    if (!status.ok()) return false;

    blockOut.deserialize(value);
    return true;
}

/*
  Check if block exists
*/
bool BlockDB::hasBlock(const std::string& hash) {
    if (!db) return false;

    std::string value;
    leveldb::Status status = db->Get(
        leveldb::ReadOptions(),
        hash,
        &value
    );

    return status.ok();
}
