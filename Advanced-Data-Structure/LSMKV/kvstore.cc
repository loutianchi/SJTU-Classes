#include "kvstore.h"
#include "utils.h"
#include <string>

#include <iostream>

KVStore::KVStore(const std::string &dir, const std::string &vlog)
	: KVStoreAPI(dir, vlog), dirP(dir), vlogP(vlog)
{
	std::vector<std::string> dirResult;
	std::vector<std::string> fileResult;
	utils::scanDir(dirP, dirResult);
	std::string levelDir;
	SScnt = ssTimeStamp = 0;
	int levelNum;
	unsigned long long tm;
	for (std::string subDir : dirResult) {
		sscanf(subDir.c_str(), "level-%d", &levelNum);
		levelDir = dirP + '/' + subDir;
		if (!utils::dirExists(levelDir))
			continue;
		utils::scanDir(levelDir, fileResult);
		for (std::string filename : fileResult) {
			++SScnt;
			sstFiles.push_back({levelNum, filename});
			sscanf(filename.c_str(), "%lld.sst", &tm);
			ssTimeStamp = ssTimeStamp > tm ? ssTimeStamp : tm;
		}
	}

	memTable = new skiplist::skiplist_type(possiblity);
	vLog = new VLog::VLog_type(vlog);
}

KVStore::~KVStore()
{
	SSTableGenerator();
	delete memTable;
	delete vLog;
}

/**
 * Insert/Update the key-value pair.
 * No return values for simplicity.
 */
void KVStore::put(uint64_t key, const std::string &s)
{
	memTable->put(key, s);
	// When too large, save to SSTable;
//	if (memTable->isFull()) {
//		save();
//	}

}

// get the newest offset from SS_Table by key
unsigned long long KVStore::getOffsetFromSSTable(uint64_t key)
{
	std::string sstFile;
	unsigned long long timeNew = 0;
	unsigned long long offset = -1ull, offTmp;
	for (auto item : sstFiles) {
		sstFile = dirP + "/level-" + std::to_string(item.first) + "/" + item.second;
		FILE* in = fopen(sstFile.c_str(), "r");
		if (nullptr == in) {
			std::cerr << "wrong sstFile:" << sstFile << std::endl;
			return -1ull;
		}
		SSTable::head h;
		fread(h.c, 32, 1, in);
		if (h.dat.t > timeNew && key >= h.dat.mi && key <= h.dat.ma) {
			SSTable::SSTable_type st(sstFile, vLog);
			if (st.filterCheck(key)) {
				offTmp = st.getOffset(key);
				if (offTmp != -1ull) {
					offset = offTmp;
					timeNew = h.dat.t;
				}
			}
		}
		fclose(in);
	}
	return offset;
}

/**
 * Returns the (string) value of the given key.
 * An empty string indicates not found.
 */
std::string KVStore::get(uint64_t key)
{
	std::string s = memTable->get(key);
	if (s == "~DELETED~") {
		return "";
	}
	if (s == "") {
		unsigned long long offset = getOffsetFromSSTable(key);
		if (offset != -1ull) {
			s = vLog->getByOffset(offset);
		}
	}
	return s;
}
/**
 * Delete the given key-value pair if it exists.
 * Returns false iff the key is not found.
 */
bool KVStore::del(uint64_t key)
{
	int flag = memTable->del(key);
	if (flag > 0)
		return true;
	else if (flag == -1)
		return false;
	// TODO: find in SSTable
	unsigned long long offset = getOffsetFromSSTable(key);
	if (offset != -1ull) {
		std::string s = vLog->getByOffset(offset);
		if (s == "")
			return false;
		else {
			put(key, "~DELETED~");
			return true;
		}
	}
	else
		return false;
}

/**
 * This resets the kvstore. All key-value pairs should be removed,
 * including memtable and all sstables files.
 */
void KVStore::reset()
{
	delete vLog;
	delete memTable;

	utils::rmfile(vlogP);
	std::vector<std::string> dirResult;
	std::vector<std::string> fileResult;
	utils::scanDir(dirP, dirResult);
	std::string levelDir;
	for (std::string subDir : dirResult) {
		levelDir = dirP + '/' + subDir;
		utils::scanDir(levelDir, fileResult);
		for (std::string filename : fileResult)
			utils::rmfile(levelDir + '/' + filename);
		utils::rmdir(levelDir);
	}
	utils::rmdir(dirP);
	sstFiles.clear();

	utils::mkdir(dirP);
	ssTimeStamp = SScnt = 0;
	memTable = new skiplist::skiplist_type(possiblity);
	vLog = new VLog::VLog_type(vlogP);
}

/**
 * Return a list including all the key-value pair between key1 and key2.
 * keys in the list should be in an ascending order.
 * An empty string indicates not found.
 */
void KVStore::scan(uint64_t key1, uint64_t key2, std::list<std::pair<uint64_t, std::string>> &list)
{
	memTable->scan(key1, key2, list);
}

/**
 * This reclaims space from vLog by moving valid value and discarding invalid value.
 * chunk_size is the size in byte you should AT LEAST recycle.
 */
void KVStore::gc(uint64_t chunk_size)
{
	vLog->garbageCollect(chunk_size, this);
}

int getLevel(int x){
	int k = 0; 
	while (x > 1) x/=2, ++k;
	return k;
}

// add a new sstable
void KVStore::SSTableGenerator()
{
	if (memTable->isEmpty())
		return;
	++ssTimeStamp; ++SScnt;
	std::string dir = dirP + "/level-0";
	if (!utils::dirExists(dir)) {
		utils::mkdir(dir);
	}
	SSTable::SSTable_type *st;
	st = new SSTable::SSTable_type(ssTimeStamp, memTable, vLog);
	st->save(dir);
	std::string filename = std::to_string(ssTimeStamp) + ".sst";
	sstFiles.push_back({0, filename});
	delete st;
}

// receive a offset from vlog, check if it is a newest offset
bool KVStore::checkOffset(uint64_t key, unsigned long long currentOffset)
{
	std::string s = memTable->get(key);
	// Can find in mem_table, do not insert again
	if (s != "") {
		return false;
	}
	return (getOffsetFromSSTable(key) == currentOffset);
}
void KVStore::save()
{
	SSTableGenerator();
	delete memTable;
	memTable = new skiplist::skiplist_type(possiblity);
}