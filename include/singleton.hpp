#pragma once
#include <memory>

template <typename T>
class Singleton
{
public:
	Singleton(const Singleton&) = delete;
	Singleton& operator=(const Singleton&) = delete;

	static T& getInstance()
	{
		static std::unique_ptr<T> instance{new T{}};
		return *instance;
	}

protected:
	Singleton() {};
};