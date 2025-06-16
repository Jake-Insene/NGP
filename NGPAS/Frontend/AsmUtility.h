#pragma once
#include "StringPool.h"


namespace AsmUtility
{

inline std::string path_relative_to(StringID rel, StringID path)
{
    std::string_view rel_str = StringPool::get(rel);

    std::string new_path{ StringPool::get(path) };
    std::string source_folder{ StringPool::get(rel) };

#if defined(_WIN32)
    u64 pos1 = rel_str.find_last_of('\\');
    u64 pos2 = rel_str.find_last_of('/');
    u64 pos = std::string::npos;
    if (pos1 != pos2)
    {
        if (pos1 > pos2 && pos1 != std::string::npos)
            pos = pos1;
        else
            pos = pos2;
    }

#else
    u64 pos = rel.find_last_of('/');
#endif
    if (pos != std::string::npos)
    {
        source_folder = source_folder.substr(0, pos);
        new_path = source_folder + "/" + new_path;
    }

    return new_path;
}

}
