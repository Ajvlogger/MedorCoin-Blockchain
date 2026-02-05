#include "db/blockdb.h"
...

Blockchain::Blockchain(const std::string& ownerAddr)
    : blockDB()
{
    ownerAddress = ownerAddr;
    medor = 0x1e00ffff;
    totalSupply = 0;
    maxSupply = 50000000;

    // open LevelDB
    if (!blockDB.open("data/medorcoin_blocks")) {
        std::cerr << "Error opening blockchain DB\n";
    }

    // load existing blocks from DB if any
    leveldb::Iterator* it = blockDB.db->NewIterator(leveldb::ReadOptions());
    for (it->SeekToFirst(); it->Valid(); it->Next()) {
        Block b;
        if (blockDB.readBlock(it->key().ToString(), b)) {
            chain.push_back(b);
        }
    }
    delete it;
}
