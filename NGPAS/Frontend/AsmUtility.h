#pragma once
#include "Core/Header.h"
#include <string>


namespace AsmUtility
{
	
inline std::string path_relative_to(const std::string& rel, const std::string& path)
{
    std::string new_path = path;
    std::string source_folder = rel;

#if defined(_WIN32)
    u64 pos1 = rel.find_last_of('\\');
    u64 pos2 = rel.find_last_of('/');
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
