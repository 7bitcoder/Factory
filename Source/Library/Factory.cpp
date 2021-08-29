#include <iostream>
#include <fstream>
#include <format>

#include "Factory.hpp"

namespace sd
{
    namespace
    {
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

    void Factory::loadNet(std::istream &stream)
    {
        size_t lineCnt = 0;
        try
        {

            for (std::string line; std::getline(stream, line);)
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
        if (input.size() != 4)
        {
            throw std::runtime_error("Expected this line to fit this pattern: LINK src=ramp-1 dest=worker-1 p=1.0");
        }

        double probability = 1.0;
        Net::LinkBind source, sink;
        bool linkCheck = false, scrCheck = false, destCheck = false, pCheck = false;
        for (auto &word : input)
        {
            if (word == "LINK")
            {
                linkCheck = true;
                continue;
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
                    source.type = Net::NodeType::RAMP;
                }
                else if (splitted[0].ends_with("worker"))
                {
                    source.type = Net::NodeType::WORKER;
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
                    sink.type = Net::NodeType::STORE;
                }
                else if (splitted[0].ends_with("worker"))
                {
                    sink.type = Net::NodeType::WORKER;
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
        if (!destCheck)
        {
            throw std::runtime_error("destination not provided");
        }
        if (!pCheck)
        {
            throw std::runtime_error("probability not provided");
        }
        _net.addLink(probability, source, sink);
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
        if (!std::filesystem::exists(filePath))
        {
            throw std::runtime_error(std::format("File {}, does not exists", filePath.string()));
        }
        std::ifstream file(filePath);
        loadNet(file);
    }

    void Factory::build()
    {
        _net.build();
    }

    void Factory::run()
    {
        _net.validate();
        _net.run(100, std::cout, {2});
    }
}