#ifndef CACHE_H
#define CACHE_H

#include <memory>
#include <unordered_map>

/*
 * Caches types of T, really just a hash table of name -> T
 */
template<typename T>
class Cache {
	std::unordered_map<std::string, std::unique_ptr<T>> cache;

public:
	using iter = typename std::unordered_map<std::string, std::unique_ptr<T>>::iterator;
	using citer = typename std::unordered_map<std::string, std::unique_ptr<T>>::const_iterator;
	/*
	 * Add some T to the cache
	 */
	T* add(const std::string &name, std::unique_ptr<T> t){
		auto it = cache.emplace(std::make_pair(name, std::move(t)));
		return it.first->second.get();
	}
	/*
	 * Get a non-owning pointer to some T in the cache
	 * nullptr is returned if the name isn't found
	 */
	const T* get(const std::string &name) const {
		auto fnd = cache.find(name);
		if (fnd != cache.end()){
			return fnd->second.get();
		}
		return nullptr;
	}
	T* get(const std::string &name){
		auto fnd = cache.find(name);
		if (fnd != cache.end()){
			return fnd->second.get();
		}
		return nullptr;
	}
	iter begin(){
		return cache.begin();
	}
	citer begin() const {
		return cache.begin();
	}
	iter end(){
		return cache.end();
	}
	citer end() const {
		return cache.cend();
	}
	size_t size() const {
		return cache.size();
	}
};

#endif

