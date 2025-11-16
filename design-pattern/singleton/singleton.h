#ifndef SINGLETON_H
#define SINGLETON_H

// template<class T>
// class Singleton {
// public:
//     static T& instance() {
//         static T obj = T();
//         return obj;
//     }
// protected:
//     Singleton() = default;
//     virtual ~Singleton() = default;
// private:
//     Singleton(const Singleton& other) = delete;
//     Singleton(Singleton&& other) = delete;
//     Singleton operator=(const Singleton& other) = delete;
//     Singleton operator=(Singleton&& other) = delete;
// };

#include <memory>

template<typename T, bool LongLifeTime = false> 
class Singleton;

template<typename T>
class Singleton<T, false>
{
public:

	template<typename... Args>
	static T& instance(Args&&... args) {
		static T obj(std::forward<Args>(args)...);
		return obj;
	}

protected:
	Singleton() = default;
	virtual ~Singleton() {}

private:
	Singleton(const Singleton&) = delete;
	Singleton& operator=(const Singleton&) = delete;
	Singleton(Singleton&&) = delete;
	Singleton& operator=(Singleton&&) = delete;
};



#endif