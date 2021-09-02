#pragma once

#include <string>
#include <variant>
#include <vector>
#include <optional>

namespace sd
{
    struct Configuration
    {
        std::optional<std::string> structureFile = std::nullopt;
        size_t maxIterations = 100;

        std::variant<size_t, std::vector<size_t>> stateRaportTimings = size_t{20};
        std::optional<std::string> raportFile = std::nullopt;
    };

}