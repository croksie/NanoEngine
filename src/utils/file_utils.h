#pragma once
#include "utils/log.h"

#include <string>
#include <fstream>
#include <sstream>

namespace midgard::utils::file {

    inline std::string readTextFile(const std::string& path) {
        std::ifstream file(path);
        if (!file.is_open()) {
            ENGINE_LOG_ERROR("Failed to open file: {0}", path);
            return "";
        }
        std::stringstream buffer;
        buffer << file.rdbuf();
        file.close();
        return buffer.str();
    }

} // namespace midgard::utils::file