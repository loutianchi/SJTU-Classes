#pragma once

#include "kvstore_api.h"
#include "skiplist.h"
#include "vlog.h"
#include "sstable.h"

#include <map>

class KVStore : public KVStoreAPI
{
	// You can add your implementation here
private:
	skiplist::skiplist_type *memTable;
	VLog::VLog_type *vLog;
	const double possiblity = 0.36787944;
	unsigned long long SScnt, ssTimeStamp;
	std::list< std::pair<int, std::string> > sstFiles;
	std::map< std::string, SSTable::SSTable_type* > sstCache;

	const std::string dirP, vlogP;

	SSTable::SSTable_type* cacheSST(const std::pair<int, std::string> &sstFile);
	int levelCnt[30], totLevel;
	struct NewKV
	{
		uint64_t key;
		unsigned long long offset, time;
		unsigned int len;
	};
	
	void SSTableGenerator();
	void checkLevel();
	void getCandidates(int level, std::vector<SSTable::SSTable_type*> &candidates);
	void compaction(int level);
public:
	KVStore(const std::string &dir, const std::string &vlog);

	~KVStore();

	void put(uint64_t key, const std::string &s) override;

	std::string get(uint64_t key) override;

	bool del(uint64_t key) override;

	void reset() override;

	void scan(uint64_t key1, uint64_t key2, std::list<std::pair<uint64_t, std::string>> &list) override;

	void gc(uint64_t chunk_size) override;

	bool checkOffset(uint64_t key, unsigned long long currentOffset);
	
	unsigned long long getOffsetFromSSTable(uint64_t key);

	void save();
};
