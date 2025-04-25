#pragma once

#include "CoreTypes.h"
#include "Allocator.h"
#include <vector>
#include <list>
#include <queue>
#include <stack>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <array>
using namespace std;


template<typename Type, uint32 Size>
using Array = array<Type, Size>;

template<typename Type>
using Vector = vector<Type, core::memory::StlAllocator<Type>>;

template<typename Type>
using List = list<Type, core::memory::StlAllocator<Type>>;

template<typename Key, typename Type, typename Pred = less<Key>>
using Map = map<Key, Type, Pred, core::memory::StlAllocator<pair<const Key, Type>>>;

template<typename Key, typename Pred = less<Key>>
using Set = set<Key, Pred, core::memory::StlAllocator<Key>>;

template<typename Type>
using Deque = deque<Type, core::memory::StlAllocator<Type>>;

template<typename Type, typename Container = Deque<Type>>
using Queue = queue<Type, Container>;

template<typename Type, typename Container = Deque<Type>>
using Stack = stack<Type, Container>;

template<typename Type, typename Container = Vector<Type>, typename Pred = less<typename Container::value_type>>
using PriorityQueue = priority_queue<Type, Container, Pred>;

using String = basic_string<char, char_traits<char>, core::memory::StlAllocator<char>>;

using WString = basic_string<wchar_t, char_traits<wchar_t>, core::memory::StlAllocator<wchar_t>>;

template<typename Key, typename Type, typename Hasher = hash<Key>, typename KeyEq = equal_to<Key>>
using HashMap = unordered_map<Key, Type, Hasher, KeyEq, core::memory::StlAllocator<pair<const Key, Type>>>;

template<typename Key, typename Hasher = hash<Key>, typename KeyEq = equal_to<Key>>
using HashSet = unordered_set<Key, Hasher, KeyEq, core::memory::StlAllocator<Key>>;