#ifndef SKIPLIST_H
#define SKIPLIST_H

#include <cstdint>
// #include <optional>
// #include <vector>
#include <string>

namespace skiplist {

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
	skiplistNode_type *first, *last, *update[MAX_LEVEL];
	int max_level;
	
	int randomLevel();
public:
	explicit skiplist_type(double p = 0.5);
	void put(key_type key, const value_type &val);
	//std::optional<value_type> get(key_type key) const;
	std::string get(key_type key) const;

	// for hw1 only
	int query_distance(key_type key) const;
};

} // namespace skiplist

#endif // SKIPLIST_H
