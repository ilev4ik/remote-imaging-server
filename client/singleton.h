#pragma once
#ifndef SINGLETON_H
#define SINGLETON_H

template <class T>
struct singleton
{
	static T* instance() {
		static T ins;
		return &ins;
	}

	singleton() = delete;
	singleton(const singleton&) = delete;
};

#endif // SINGLETON_H