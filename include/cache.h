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
	 * Add some geometry to the cache
	 */
	void add(const std::string &name, std::unique_ptr<T> t){
		cache[name] = std::move(t);
	}
	/*
	 * Get a non-owning pointer to some geometry in the cache
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
	citer cbegin() const {
		return cache.cbegin();
	}
	iter end(){
		return cache.end();
	}
	citer cend(){
		return cache.cend();
	}
};

#endif

