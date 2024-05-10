#pragma once

#include <fstream>
#include "Logger.h"

namespace SoftGL
{
    class FileUtils
    {
    public:
        static bool exists(const std::string &path)
        {
            std::ifstream file(path);
            return file.good();
        }

        static std::vector<uint8_t> readBytes(const std::string &path)
        {
            std::vector<uint8_t> ret;
            std::ifstream file(path, std::ios::in | std::ios::binary | std::ios::ate);
            if (!file.is_open())
            {
                LOGE("failed to open file: %s", path.c_str());
                return ret;
            }
            size_t size = file.tellg();
            if (size <= 0)
            {
                LOGE("failed to read file, invalid size: %d", size);
                return ret;
            }
            ret.resize(size);
            file.seekg(0, std::ios::beg);
            file.read(reinterpret_cast<char *>(ret.data()), (std::streamsize)size);
            return ret;
        }
    };
}