#ifndef SKIPLIST_H
#define SKIPLIST_H

#include <cstdint>
// #include <optional>
// #include <vector>
#include <string>
#include <list>

namespace skiplist {

const int MAX_NUM = 408;
const int MAX_LEVEL = 17;

using key_type = uint64_t;
// using value_type = std::vector<char>;
using value_type = std::string;

class skiplist_type
{
	struct skiplistNode_type{
		skiplistNode_type *forward[MAX_LEVEL];
		key_type key;
		value_type value;
		skiplistNode_type(key_type &keyI, const value_type &valueI) {
			this->key = keyI;
			this->value = valueI;
		}
	};

	double possibility;
	unsigned long long total;
	skiplistNode_type *first, *last, *update[MAX_LEVEL];
	int max_level;
	
	int randomLevel();
public:
	explicit skiplist_type(double p = 0.5);
    ~skiplist_type();
	void put(key_type key, const value_type &val);
	//std::optional<value_type> get(key_type key) const;
	std::string get(key_type key) const;
    int del(key_type key);
    void scan(key_type st, key_type ed, std::list<std::pair<key_type, value_type> > &res);
    void getAll(std::list<std::pair<key_type, value_type> > &res);
    bool isEmpty();
	bool isFull();
};

} // namespace skiplist

#endif // SKIPLIST_H
