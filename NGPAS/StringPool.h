/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#pragma once
#include "Core/Header.h"

#include <unordered_map>
#include <string>
#include <string_view>

using StringID = usize;

static constexpr StringID InvalidStringID = StringID(0);

struct StringPool
{
    struct StringPoolEntry
    {
        StringID id;
        std::string str;
    };

    static inline std::unordered_map<StringID, StringPoolEntry> entries;
    static inline std::unordered_map<std::string, StringID> entries_by_str;
    static inline StringID counter = 1; // 0 Invalid

    static StringID add(const std::string_view& new_str);
    static StringID get_or_insert(const std::string_view& new_str);
    static std::string_view get(StringID id);

    static bool has(const std::string_view& new_str);
};