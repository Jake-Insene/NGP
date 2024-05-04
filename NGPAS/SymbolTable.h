#pragma once
#include <Header.h>
#include <unordered_map>
#include <vector>
#include <string_view>

template<typename T>
struct SymbolTable {
    using Map = std::unordered_map<std::string_view, u32>;
    using Vec = std::vector<T>;

    Vec data;
    Map map;

    [[nodiscard]] constexpr Vec::iterator begin() { return data.begin(); }
    [[nodiscard]] constexpr Vec::const_iterator begin() const { return data.begin(); }
    [[nodiscard]] constexpr Vec::iterator end() { return data.end(); }
    [[nodiscard]] constexpr Vec::const_iterator end() const { return data.end(); }

    [[nodiscard]] T& create(const std::string_view& label) {
        auto& value = data.emplace_back();
        map.emplace(label, u32(data.size() - 1));
        return value;
    }

    [[nodiscard]] T& get(u32 index) {
        return data[label];
    }

    [[nodiscard]] Map::iterator find(const std::string_view& label) {
        return map.find(label);
    }

};
