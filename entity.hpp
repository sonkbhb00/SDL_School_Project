
#pragma once
#include <iostream>
#include <algorithm>
#include <memory>
#include <vector>
#include <bitset>
#include <array>

class Component;
class Entity;

using ComponentID = std::size_t;

inline ComponentID getComponentID(){
    static ComponentID lastID = 0;
    return lastID++;
}

template <typename T> inline ComponentID getComponentID() noexcept{
    static ComponentID typeID = getComponentID();
    return typeID();
}
