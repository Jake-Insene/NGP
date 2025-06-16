/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#include "StringPool.h"


StringID StringPool::add(const std::string_view& new_str)
{
	StringID id = counter;
	counter++;

	entries.insert({ id, StringPoolEntry{.id = id, .str = std::string(new_str)} });
	entries_by_str.insert({ std::string(new_str), id });

	return id;
}

StringID StringPool::get_or_insert(const std::string_view& new_str)
{
	auto it = entries_by_str.find(std::string(new_str));
	if (it != entries_by_str.end())
		return it->second;

	return add(new_str);
}

std::string_view StringPool::get(StringID id)
{
	auto it = entries.find(id);
	if (it != entries.end())
		return it->second.str;

	return "";
}

bool StringPool::has(const std::string_view& new_str)
{
	return entries_by_str.find(std::string(new_str)) != entries_by_str.end();
}
