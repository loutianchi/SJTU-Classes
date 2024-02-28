#ifndef SKIPLIST_H
#define SKIPLIST_H

#include <cstdint>
// #include <optional>
// #include <vector>
#include <string>

namespace skiplist {
using key_type = uint64_t;
// using value_type = std::vector<char>;
using value_type = std::string;

class skiplist_type
{
	// add something here
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
