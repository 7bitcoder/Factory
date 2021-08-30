#include <iostream>
#include <fstream>
#include <format>

#include "CLI11.hpp"
#include "Factory.hpp"

namespace sd
{
    namespace
    {
        // cli variable holders
        // general/store
        size_t id;
        // worker
        size_t processingTime;
        Net::QueueType queueType;
        // ramp
        size_t deliveryInterval;
        // link
        double probability;
        std::pair<size_t, NodeType> source;
        std::pair<size_t, NodeType> destination;

        bool runSimulation = false;

        std::vector<std::string> splitStr(const std::string &str, char splitChat)
        {
            std::vector<std::string> out;
            std::istringstream iss(str);
            std::string s;
            while (getline(iss, s, splitChat))
            {
                out.push_back(s);
            }
            return out;
        }
    }

    void Factory::run()
    {
        load(_config.structureFile);
        _net.validate();
        std::string line;
        while (!runSimulation && std::getline(std::cin, line))
        {
            try
            {
                _cli->parse(line);
            }
            catch (const CLI::ParseError &e)
            {
                _cli->exit(e);
            }
        }
        _net.run(_config.maxIterations, std::cout, {_config.stateRaportTimings});
    }

    Factory::Factory(Configuration &&config) : _config(config)
    {
        _cli = std::make_unique<CLI::App>("CLI");
        buildCli();
    }

    Factory::~Factory() {}

    void Factory::buildCli()
    {
        auto addWorker = _cli->add_subcommand("add_worker");
        addWorker->add_option("-i,--id", id)
            ->required();
        addWorker->add_option("-t,--processing-time", processingTime)
            ->required();
        addWorker->add_option("-q,--queue-type", queueType)
            ->required();

        addWorker->callback(
            [this]()
            {
                _net.addWorker(id, processingTime, queueType);
            });

        auto addRamp = _cli->add_subcommand("add_loading_ramp")->alias("add_ramp");
        addRamp->add_option("-i,--id", id)
            ->required();
        addRamp->add_option("-t,--delivery-interval", deliveryInterval)
            ->required();

        addRamp->callback(
            [this]()
            {
                _net.addLoadingRamp(id, deliveryInterval);
            });

        auto addStore = _cli->add_subcommand("add_storehause")->alias("add_store");
        addStore->add_option("-i,--id", id)
            ->required();

        addStore->callback(
            [this]()
            {
                _net.addStorehause(id);
            });

        auto addLink = _cli->add_subcommand("add_link");
        addLink->add_option("-i,--id", id)
            ->required();
        addLink->add_option("-s,--scr", source)
            ->required();
        addLink->add_option("-d,--dest", destination)
            ->required();
        addLink->add_option("-p,--probability", probability)
            ->required();

        addLink->callback(
            [this]()
            {
                Net::LinkBind scr{source.first, source.second};
                Net::LinkBind dest{destination.first, destination.second};
                _net.addLink(id, probability, scr, dest);
            });

        _cli->add_subcommand("run")->callback([]()
                                              { runSimulation = true; });
    }

    void Factory::loadNet(std::istream &stream)
    {
        size_t lineCnt = 0;
        try
        {
            for (std::string line; std::getline(stream, line); ++lineCnt)
            {
                if (line.empty() || line.front() == ';')
                {
                    continue;
                }

                auto splitted = splitStr(line, ' ');
                if (splitted.empty())
                {
                    continue;
                }
                if (splitted.front() == "WORKER")
                {
                    createWorker(splitted);
                }
                else if (splitted.front() == "LOADING_RAMP")
                {
                    createLoadingRamp(splitted);
                }
                else if (splitted.front() == "STOREHOUSE")
                {
                    createStoreHause(splitted);
                }
                else if (splitted.front() == "LINK")
                {
                    createLink(splitted);
                }
                else
                {
                    throw std::runtime_error("Expected word: WORKER | LOADING_RAMP | STOREHOUSE | LINK");
                }
            }
        }
        catch (std::exception &e)
        {
            std::cout << std::format("Error in line {}: {}", lineCnt, e.what());
        }
        catch (...)
        {
            std::cout << std::format("Unexpected error in line {}", lineCnt);
        }
    }

    void Factory::createLink(const std::vector<std::string> &input)
    {
        if (input.size() != 5)
        {
            throw std::runtime_error("Expected this line to fit this pattern: LINK src=ramp-1 dest=worker-1 p=1.0");
        }

        size_t id = 0;
        double probability = 1.0;
        Net::LinkBind source, sink;
        bool linkCheck = false, scrCheck = false, destCheck = false, idCheck = false, pCheck = false;
        for (auto &word : input)
        {
            if (word == "LINK")
            {
                linkCheck = true;
                continue;
            }
            else if (word.starts_with("id="))
            {
                auto splitted = splitStr(word, '=');
                if (splitted.size() != 2)
                {
                    throw std::runtime_error(std::format("Sentence: \"{}\", expected to fit this pattern: id=<worker-id>", word));
                }

                id = std::stoi(splitted[1]);
                idCheck = true;
            }
            else if (word.starts_with("src="))
            {
                auto splitted = splitStr(word, '-');
                if (splitted.size() != 2)
                {
                    throw std::runtime_error(std::format("Sentence: \"{}\", expected to fit this pattern: src=type-id, where type is one of ramp, worker and id is identifier", word));
                }

                if (splitted[0].ends_with("ramp"))
                {
                    source.type = NodeType::RAMP;
                }
                else if (splitted[0].ends_with("worker"))
                {
                    source.type = NodeType::WORKER;
                }
                else
                {
                    throw std::runtime_error(std::format("Sentence: \"{}\", expected to fit this pattern: src=type-id, where type is one of ramp, worker and id is identifier", word));
                }
                source.id = std::stoi(splitted[1]);
                scrCheck = true;
            }
            else if (word.starts_with("dest="))
            {
                auto splitted = splitStr(word, '-');
                if (splitted.size() != 2)
                {
                    throw std::runtime_error(std::format("Sentence: \"{}\", expected to fit this pattern: dest=type-id, where type is one of store, worker and id is identifier", word));
                }

                else if (splitted[0].ends_with("store"))
                {
                    sink.type = NodeType::STORE;
                }
                else if (splitted[0].ends_with("worker"))
                {
                    sink.type = NodeType::WORKER;
                }
                else
                {
                    throw std::runtime_error(std::format("Sentence: \"{}\", expected to fit this pattern: dest=type-id, where type is one of store, worker and id is identifier", word));
                }
                sink.id = std::stoi(splitted[1]);
                destCheck = true;
            }
            else if (word.starts_with("p"))
            {
                auto splitted = splitStr(word, '=');
                probability = std::stod(splitted[1]);
                pCheck = true;
            }
            else
            {
                throw std::runtime_error("Expected this line to fit this pattern: LINK src=ramp-1 dest=worker-1 p=1.0");
            }
        }
        if (!linkCheck)
        {
            throw std::runtime_error("LINK word not provided");
        }
        if (!scrCheck)
        {
            throw std::runtime_error("source not provided");
        }
        if (!idCheck)
        {
            throw std::runtime_error("id not provided");
        }
        if (!destCheck)
        {
            throw std::runtime_error("destination not provided");
        }
        if (!pCheck)
        {
            throw std::runtime_error("probability not provided");
        }
        _net.addLink(id, probability, source, sink);
    }

    void Factory::createWorker(const std::vector<std::string> &input)
    {
        if (input.size() != 4)
        {
            throw std::runtime_error("Expected this line to fit this pattern: LINK src=ramp-1 dest=worker-1 p=1.0");
        }

        size_t processingTime = 0;
        size_t id = 0;
        Net::QueueType type;

        bool workerCheck = false, processCheck = false, idCheck = false, typeCheck = false;
        for (auto &word : input)
        {
            if (word == "WORKER")
            {
                workerCheck = true;
                continue;
            }
            else if (word.starts_with("processing-time="))
            {
                auto splitted = splitStr(word, '=');
                if (splitted.size() != 2)
                {
                    throw std::runtime_error(std::format("Sentence: \"{}\", expected to fit this pattern: processing-time=<processing-time>", word));
                }

                processingTime = std::stoi(splitted[1]);
                processCheck = true;
            }
            else if (word.starts_with("id="))
            {
                auto splitted = splitStr(word, '=');
                if (splitted.size() != 2)
                {
                    throw std::runtime_error(std::format("Sentence: \"{}\", expected to fit this pattern: id=<worker-id>", word));
                }

                id = std::stoi(splitted[1]);
                idCheck = true;
            }
            else if (word.starts_with("queue-type="))
            {
                auto splitted = splitStr(word, '=');
                if (splitted.size() != 2)
                {
                    throw std::runtime_error(std::format("Sentence: \"{}\", expected to fit this pattern: queue-type=<queuetype>", word));
                }

                if (splitted[1] == "FIFO")
                {
                    type = Net::QueueType::FIFO;
                }
                else if (splitted[1] == "LIFO")
                {
                    type = Net::QueueType::LIFO;
                }
                else
                {
                    throw std::runtime_error(std::format("Sentence: \"{}\", expected to fit this pattern: queue-type=<queuetype>", word));
                }
                typeCheck = true;
            }
            else
            {
                throw std::runtime_error("Expected this line to fit this pattern: WORKER id=<worker-id> processing-time=<processing-time> queue-type=<queuetype>");
            }
        }
        if (!workerCheck)
        {
            throw std::runtime_error("Worker word not provided");
        }
        if (!processCheck)
        {
            throw std::runtime_error("processing-time not provided");
        }
        if (!idCheck)
        {
            throw std::runtime_error("id not provided");
        }
        if (!typeCheck)
        {
            throw std::runtime_error("queue-type not provided");
        }
        _net.addWorker(id, processingTime, type);
    }

    void Factory::createLoadingRamp(const std::vector<std::string> &input)
    {
        if (input.size() != 3)
        {
            throw std::runtime_error("Expected this line to fit this pattern: LOADING_RAMP id=<ramp-id> delivery-interval=<delivery-interval>");
        }

        size_t deliveryInterval = 0;
        size_t id = 0;

        bool loadingCheck = false, deliveryCheck = false, idCheck = false;
        for (auto &word : input)
        {
            if (word == "LOADING_RAMP")
            {
                loadingCheck = true;
                continue;
            }
            else if (word.starts_with("delivery-interval="))
            {
                auto splitted = splitStr(word, '=');
                if (splitted.size() != 2)
                {
                    throw std::runtime_error(std::format("Sentence: \"{}\", expected to fit this pattern: processing-time=<processing-time>", word));
                }

                deliveryInterval = std::stoi(splitted[1]);
                deliveryCheck = true;
            }
            else if (word.starts_with("id="))
            {
                auto splitted = splitStr(word, '=');
                if (splitted.size() != 2)
                {
                    throw std::runtime_error(std::format("Sentence: \"{}\", expected to fit this pattern: id=<worker-id>", word));
                }

                id = std::stoi(splitted[1]);
                idCheck = true;
            }
            else
            {
                throw std::runtime_error("Expected this line to fit this pattern: WORKER id=<worker-id> processing-time=<processing-time> queue-type=<queuetype>");
            }
        }
        if (!loadingCheck)
        {
            throw std::runtime_error("Loading Ramp word not provided");
        }
        if (!deliveryCheck)
        {
            throw std::runtime_error("delivery-interval= not provided");
        }
        if (!idCheck)
        {
            throw std::runtime_error("id not provided");
        }
        _net.addLoadingRamp(id, deliveryInterval);
    }

    void Factory::createStoreHause(const std::vector<std::string> &input)
    {
        if (input.size() != 2)
        {
            throw std::runtime_error("Expected this line to fit this pattern: LOADING_RAMP id=<ramp-id> delivery-interval=<delivery-interval>");
        }

        size_t id = 0;

        bool storeCheck = false, idCheck = false;
        for (auto &word : input)
        {
            if (word == "STOREHOUSE")
            {
                storeCheck = true;
                continue;
            }
            else if (word.starts_with("id="))
            {
                auto splitted = splitStr(word, '=');
                if (splitted.size() != 2)
                {
                    throw std::runtime_error(std::format("Sentence: \"{}\", expected to fit this pattern: id=<worker-id>", word));
                }

                id = std::stoi(splitted[1]);
                idCheck = true;
            }
            else
            {
                throw std::runtime_error("Expected this line to fit this pattern: WORKER id=<worker-id> processing-time=<processing-time> queue-type=<queuetype>");
            }
        }
        if (!storeCheck)
        {
            throw std::runtime_error("Loading Ramp word not provided");
        }
        if (!idCheck)
        {
            throw std::runtime_error("id not provided");
        }
        _net.addStorehause(id);
    }

    void Factory::load(const std::filesystem::path &filePath)
    {
        auto curr = std::filesystem::current_path().string();
        if (!std::filesystem::exists(filePath))
        {
            throw std::runtime_error(std::format("File {}, does not exists", filePath.string()));
        }
        std::ifstream file(filePath);
        loadNet(file);
        std::cout << _net.getStructure();
    }

    void Factory::build()
    {
        _net.build();
    }
}