#include "kvstore.h"
#include "utils.h"
#include <string>
#include <cmath>
#include <unordered_set>

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
	memset(levelCnt, 0, sizeof(levelCnt));
	for (std::string subDir : dirResult) {
		sscanf(subDir.c_str(), "level-%d", &levelNum);
		levelDir = dirP + '/' + subDir;
		if (!utils::dirExists(levelDir))
			continue;
		fileResult.clear();
		levelCnt[levelNum] = utils::scanDir(levelDir, fileResult);
		totLevel = totLevel < levelNum ? levelNum : totLevel;
		std::string sstFile;
		for (std::string filename : fileResult) {
			sstFiles.push_back({levelNum, filename});
			sstFile = dirP + "/level-" + std::to_string(levelNum) + "/" + filename;
			SSTable::SSTable_type *st = new SSTable::SSTable_type(sstFile, vLog);
			sscanf(filename.c_str(), "%lld.sst", &tm);
			SScnt = SScnt > tm ? SScnt : tm;
			tm = st->getTime();
			ssTimeStamp = ssTimeStamp > tm ? ssTimeStamp : tm;
			delete st;
		}
	}
	
	memTable = new skiplist::skiplist_type(possiblity);
	vLog = new VLog::VLog_type(vlog);
}

KVStore::~KVStore()
{
	for (auto &item : sstCache) {
		delete item.second;
	}
	SSTableGenerator();
	checkLevel();
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
	if (memTable->isFull()) {
		save();
	}

}

SSTable::SSTable_type* KVStore::cacheSST(const std::pair<int, std::string> &sstFile)
{
	SSTable::SSTable_type* now;
	if (sstCache.find(sstFile.second) == sstCache.end()) {
		std::string path = dirP + "/level-" + std::to_string(sstFile.first) + "/" + sstFile.second;
		now = new SSTable::SSTable_type(path, vLog);
		sstCache.insert({sstFile.second, now});
	}
	else
		now = sstCache[sstFile.second];
	return now;
}

// get the newest offset from SS_Table by key
unsigned long long KVStore::getOffsetFromSSTable(uint64_t key)
{
	std::string sstFile;
	unsigned long long timeNew = 0;
	unsigned long long offset = -1ull, offTmp;
	SSTable::SSTable_type *now;
	for (const auto &item : sstFiles) {
		now = cacheSST(item);
		SSTable::head h = now->getHead();
		if (h.dat.t > timeNew && key >= h.dat.mi && key <= h.dat.ma) {
			if (now->filterCheck(key)) {
				offTmp = now->getOffset(key);
				if (offTmp != -1ull) {
					offset = offTmp;
					timeNew = h.dat.t;
				}
			}
		}
	}
//	std::cout << key << " " << timeNew << " " << offset << std::endl;
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
//	std::cout << "FLAG:"  << flag << std::endl;
	if (flag > 0)
		return true;
	else if (flag == -1)
		return false;
	// find in SSTable
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
	for (auto &item : sstCache) {
		delete item.second;
	}
	sstCache.clear();
	memset(levelCnt, 0, sizeof(levelCnt));

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
	std::list<std::pair<uint64_t, std::string>> memLst;
	std::list<SSTable::entr> sstLst;
	memTable->scan(key1, key2, memLst);
	// TODO: scan the SST, insert the valid value
	std::vector<SSTable::SSTable_type*> candidates;
	SSTable::SSTable_type* now;
	SSTable::head h;
	for (auto &sstFile : sstFiles) {
		now = cacheSST(sstFile);
		h = now->getHead();
		if (!(h.dat.ma < key1 || h.dat.mi > key2))
			candidates.push_back(now);
	}
	
	bool flag;
	int totCand = candidates.size();
	SSTable::entr tmp;
	NewKV newKV;
	std::vector<unsigned long long> p(totCand);
	for (int i = 0; i < p.size(); i++)
		p[i] = 0;

	while (true) {
		flag = false;
		newKV.key = -1ull; newKV.time = 0;
		for (int i = 0; i < totCand; i++)
			if (p[i] < candidates[i]->getHead().dat.num) {
				flag = true;
				tmp = candidates[i]->getItem(p[i]);
				if (tmp.kov.key < newKV.key) {
					newKV.key = tmp.kov.key;
					newKV.offset = tmp.kov.offset;
					newKV.time = candidates[i]->getHead().dat.t;
					newKV.len = tmp.kov.len;
				}
				else if (tmp.kov.key == newKV.key && candidates[i]->getHead().dat.t > newKV.time) {
					newKV.offset = tmp.kov.offset;
					newKV.time = candidates[i]->getHead().dat.t;
					newKV.len = tmp.kov.len;
				}
			}
		if (!flag) break;
		for (int i = 0; i < totCand; i++) {
			tmp = candidates[i]->getItem(p[i]);
			if (tmp.kov.key == newKV.key)
				p[i]++;
		}
		tmp.kov.key = newKV.key;
		tmp.kov.len = newKV.len;
		tmp.kov.offset = newKV.offset;
		if (tmp.kov.key < key1)
			continue;
		if (tmp.kov.key > key2)
			break;
		if (tmp.kov.len > 0)
			sstLst.push_back(tmp);
	}
	std::pair<uint64_t, std::string> memItem;
	while (!memLst.empty() && !sstLst.empty()) {
		memItem = memLst.front();
		tmp = sstLst.front();
		if (memItem.first < tmp.kov.key) {
			list.push_back(memItem);
			memLst.pop_front();
		}
		else if (memItem.first == tmp.kov.key) {
			list.push_back(memItem);
			memLst.pop_front();
			sstLst.pop_front();
		}
		else {
			list.push_back({tmp.kov.key, vLog->getByOffset(tmp.kov.offset)});
			sstLst.pop_front();
		}
	}
	while (!memLst.empty()) {
		list.push_back(memLst.front());
		memLst.pop_front();
	}
	while (!sstLst.empty()) {
		tmp = sstLst.front();
		list.push_back({tmp.kov.key, vLog->getByOffset(tmp.kov.offset)});
		sstLst.pop_front();		
	}
	
}

/**
 * This reclaims space from vLog by moving valid value and discarding invalid value.
 * chunk_size is the size in byte you should AT LEAST recycle.
 */
void KVStore::gc(uint64_t chunk_size)
{
	vLog->garbageCollect(chunk_size, this);
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
	st = new SSTable::SSTable_type(SScnt, memTable, vLog);
	st->save(dir);
	std::string filename = std::to_string(SScnt) + ".sst";
	sstFiles.push_back({0, filename});
	levelCnt[0]++;
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
	checkLevel();
	delete memTable;
	memTable = new skiplist::skiplist_type(possiblity);
}

// see if the level has too much sst
void KVStore::checkLevel()
{
	// puts("YES lev");
	for (int i = 0; i <= totLevel + 1; ++i) {
		if (levelCnt[i] > (2 << i)) {
			compaction(i);
		}
	}
}

void formSST(std::vector<SSTable::entr> &offsetList, unsigned long long _time, std::string &path)
{
	BloomFilter::BloomFilter_type bf;
	SSTable::head h;
	h.dat.t = _time;
	h.dat.num = offsetList.size();
	h.dat.mi = offsetList[0].kov.key;
	h.dat.ma = offsetList[h.dat.num - 1].kov.key;
	for (unsigned long long i = 0; i < h.dat.num; i++)
		bf.add(offsetList[i].kov.key);
	
	FILE* out = fopen(path.c_str(), "w");
	if (nullptr == out) {
		return;
	}
	fwrite(h.c, 32, 1, out);
	bf.save(out);

	// save KEY OFFSET LEN 
	for (auto item : offsetList) {
		fwrite(item.c, 20, 1, out);
	}

	fclose(out); out = nullptr;
}


void KVStore::getCandidates(int level, std::vector<SSTable::SSTable_type*> &candidates)
{
	unsigned long long keyMin = -1ull, keyMax = 0;
	SSTable::SSTable_type *now, *tmp;
	if (level == 0) {
		for (auto &sstFile : sstFiles) {
			if (sstFile.first != level) continue;
			now = cacheSST(sstFile);
			candidates.push_back(now);
			SSTable::head h = now->getHead();
			keyMin = h.dat.mi < keyMin ? h.dat.mi : keyMin;
			keyMax = h.dat.ma > keyMax ? h.dat.ma : keyMax;
		}
	}
	else {
		unsigned long long numL = levelCnt[level] - (2 << level);
		for (auto &sstFile : sstFiles) {
			if (sstFile.first != level) continue;
			now = cacheSST(sstFile);
			if (candidates.size() < numL)
				candidates.push_back(now);
			else {
				SSTable::head h = now->getHead();
				for (unsigned long long i = 0; i < numL; i++) {
					if (candidates[i]->getHead().dat.t > h.dat.t || 
					   (candidates[i]->getHead().dat.t == h.dat.t && candidates[i]->getHead().dat.mi > h.dat.mi) ) {
						tmp = candidates[i];
						candidates[i] = now;
						now = tmp;
						h = now->getHead();
					}
				}
			}
		}
		// get scale
		for (unsigned long long i = 0; i < candidates.size(); i++) {
			SSTable::head h = candidates[i]->getHead();			
			keyMin = h.dat.mi < keyMin ? h.dat.mi : keyMin;
			keyMax = h.dat.ma > keyMax ? h.dat.ma : keyMax;
		}
	}
	// find sst in (level + 1)
	for (auto &sstFile : sstFiles) {
		if (sstFile.first != level + 1) continue;
		now = cacheSST(sstFile);
		SSTable::head h = now->getHead();
		if (!(h.dat.ma < keyMin || h.dat.mi > keyMax))
			candidates.push_back(now);
	}
}

void KVStore::compaction(int level)
{
	std::vector<SSTable::SSTable_type*> candidates;
	// puts("g candi");
	getCandidates(level, candidates);
	// puts("got");
	int totCand = candidates.size();
	std::vector<unsigned long long> p(totCand);
	for (int i = 0; i < p.size(); i++)
		p[i] = 0;
	bool flag;
	
    std::vector<SSTable::entr> offsetList;
	SSTable::entr tmp;
	std::string d = dirP + "/level-" + std::to_string(level + 1);
	if (level + 1 > totLevel) totLevel = level + 1;
	if (!utils::dirExists(d)) {
		utils::mkdir(d);
	}

	unsigned long long totTime = 0;
	// puts("st fd");
	while (true) {
		flag = false;
		NewKV newKV;
		newKV.key = -1ull; newKV.time = 0;
		for (int i = 0; i < totCand; i++)
			if (p[i] < candidates[i]->getHead().dat.num) {
				flag = true;
				tmp = candidates[i]->getItem(p[i]);
				if (tmp.kov.key < newKV.key) {
					newKV.key = tmp.kov.key;
					newKV.offset = tmp.kov.offset;
					newKV.time = candidates[i]->getHead().dat.t;
					newKV.len = tmp.kov.len;
				}
				else if (tmp.kov.key == newKV.key && candidates[i]->getHead().dat.t > newKV.time) {
					newKV.offset = tmp.kov.offset;
					newKV.time = candidates[i]->getHead().dat.t;
					newKV.len = tmp.kov.len;
				}
			}
		if (!flag) break;
		tmp.kov.key = newKV.key;
		tmp.kov.len = newKV.len;
		tmp.kov.offset = newKV.offset;
		if (level + 1 == totLevel && tmp.kov.len == 0) {
			
		}
		else {
			offsetList.push_back(tmp);
			totTime = totTime > newKV.time ? totTime : newKV.time;
		}
		for (int i = 0; i < totCand; i++) {
			tmp = candidates[i]->getItem(p[i]);
			if (tmp.kov.key == newKV.key)
				p[i]++;
		}
		if (offsetList.size() == skiplist::MAX_NUM) {
			++SScnt; ++levelCnt[level + 1];
			std::string filename = std::to_string(SScnt) + ".sst";
			std::string pa = d + "/" + filename;
			sstFiles.push_back({level + 1, filename});
			formSST(offsetList, totTime, pa);
			offsetList.clear();
			totTime = 0;
		}
	}
	
	// puts("ed fd");
	if (offsetList.size() > 0) {
		++SScnt; ++levelCnt[level + 1];
		std::string filename = std::to_string(SScnt) + ".sst";
		std::string pa = d + "/" + filename;
		sstFiles.push_back({level + 1, filename});
		formSST(offsetList, totTime, pa);
		offsetList.clear();
		totTime = 0;
	}
	
	std::unordered_set<std::string> filesToRemove;
    for (auto it = sstCache.begin(); it != sstCache.end();) {
        bool erased = false;
        for (int i = 0; i < candidates.size(); i++) {
            if (it->second == candidates[i]) {
                filesToRemove.insert(it->first);
                delete candidates[i];
                it = sstCache.erase(it);
                erased = true;
                break;
            }
        }
        if (!erased) {
            ++it;
        }
    }

	std::string rp;
    for (const auto& file : filesToRemove) {
        for (auto it = sstFiles.begin(); it != sstFiles.end(); ++it) {
            if (it->second == file) {
                rp = dirP + "/level-" + std::to_string(it->first);
                if (utils::dirExists(rp)) {
                    --levelCnt[it->first];
                    rp += "/" + it->second;
                    utils::rmfile(rp);
                }
                sstFiles.erase(it);
                break;
            }
        }
    }
}

