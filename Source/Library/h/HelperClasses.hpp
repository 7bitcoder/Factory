#pragma once

#include <vector>

#include "Link.hpp"
#include "Worker.hpp"
#include "LoadingRamp.hpp"
#include "StoreHause.hpp"

namespace sd
{
    std::vector<std::string> splitStr(const std::string &str, char splitChat);
    
    LinkData parseLink(const std::vector<std::string> &input);
    WorkerData parseWorker(const std::vector<std::string> &input);
    LoadingRampData parseLoadingRamp(const std::vector<std::string> &input);
    StoreHauseData parseStoreHause(const std::vector<std::string> &input);
}