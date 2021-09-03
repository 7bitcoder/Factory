#pragma once

#include <algorithm>
#include <format>
#include <sstream>

#include "Link.hpp"
#include "Node.hpp"

namespace sd
{
    Link::Link(size_t id, double probability, SourceNode &source, DestinationNode &destination)
        : Identifiable(id), _baseProbability(std::clamp(probability, .0, 1.0)), _source(source), _destination(destination)
    {
        if (source.getNodeType() == NodeType::RAMP && destination.getNodeType() == NodeType::STORE)
        {
            throw std::runtime_error("Cannot bind Ramp and Store.");
        }
        if (probability <= 0)
        {
            throw std::runtime_error("Probability Cannot be set to 0.");
        }
        _probability = _baseProbability;
    }

    Link::Link(const LinkData &data, SourceNode &source, DestinationNode &destination)
        : Link(data.id, data.probability, source, destination) {}

    const LinkData Link::getLinkData() const
    {
        return {getId(), getBaseProbability(), {_source.getId(), _source.getNodeType()}, {_destination.getId(), _destination.getNodeType()}};
    }

    Link::~Link() {}

    double Link::getProbability() const { return _probability; }

    double Link::getBaseProbability() const { return _baseProbability; }

    void Link::setProbability(double newProbability) { _probability = newProbability; }

    std::string Link::getStructureRaport(size_t offset) const { return std::format("{}{} (p = {:.2f})", getOffset(offset), _destination.toString(), getProbability()); }

    void Link::unBindSource() { _source.unBindSourceLink(getId()); }

    void Link::unBindDestination() { _destination.unBindDestinationLink(getId()); }

    SourceNode &Link::getSource() { return _source; }

    DestinationNode &Link::getDestination() { return _destination; }
}