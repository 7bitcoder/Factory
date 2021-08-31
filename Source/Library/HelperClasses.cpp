#include <string>
#include <sstream>
#include <format>

#include "HelperClasses.hpp"

namespace sd
{
    std::vector<std::string> splitStr(const std::string &str, char splitChat)
    {
        std::vector<std::string> out;
        std::istringstream iss(str);
        std::string s;
        while (std::getline(iss, s, splitChat))
        {
            out.push_back(s);
        }
        return out;
    }

    LinkData parseLink(const std::vector<std::string> &input)
    {
        if (input.size() != 5)
        {
            throw std::runtime_error("Expected this line to fit this pattern: LINK src=ramp-1 dest=worker-1 p=1.0");
        }

        LinkData result;
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

                result.id = std::stoi(splitted[1]);
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
                    result.source.type = NodeType::RAMP;
                }
                else if (splitted[0].ends_with("worker"))
                {
                    result.source.type = NodeType::WORKER;
                }
                else
                {
                    throw std::runtime_error(std::format("Sentence: \"{}\", expected to fit this pattern: src=type-id, where type is one of ramp, worker and id is identifier", word));
                }
                result.source.id = std::stoi(splitted[1]);
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
                    result.sink.type = NodeType::STORE;
                }
                else if (splitted[0].ends_with("worker"))
                {
                    result.sink.type = NodeType::WORKER;
                }
                else
                {
                    throw std::runtime_error(std::format("Sentence: \"{}\", expected to fit this pattern: dest=type-id, where type is one of store, worker and id is identifier", word));
                }
                result.sink.id = std::stoi(splitted[1]);
                destCheck = true;
            }
            else if (word.starts_with("p"))
            {
                auto splitted = splitStr(word, '=');
                result.probability = std::stod(splitted[1]);
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
        return result;
    }

    WorkerData parseWorker(const std::vector<std::string> &input)
    {
        if (input.size() != 4)
        {
            throw std::runtime_error("Expected this line to fit this pattern: LINK src=ramp-1 dest=worker-1 p=1.0");
        }

        WorkerData result;

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

                result.processingTime = std::stoi(splitted[1]);
                processCheck = true;
            }
            else if (word.starts_with("id="))
            {
                auto splitted = splitStr(word, '=');
                if (splitted.size() != 2)
                {
                    throw std::runtime_error(std::format("Sentence: \"{}\", expected to fit this pattern: id=<worker-id>", word));
                }

                result.id = std::stoi(splitted[1]);
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
                    result.type = WorkerType::FIFO;
                }
                else if (splitted[1] == "LIFO")
                {
                    result.type = WorkerType::LIFO;
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
        return result;
    }

    LoadingRampData parseLoadingRamp(const std::vector<std::string> &input)
    {
        if (input.size() != 3)
        {
            throw std::runtime_error("Expected this line to fit this pattern: LOADING_RAMP id=<ramp-id> delivery-interval=<delivery-interval>");
        }

        LoadingRampData result;

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

                result.deliveryInterval = std::stoi(splitted[1]);
                deliveryCheck = true;
            }
            else if (word.starts_with("id="))
            {
                auto splitted = splitStr(word, '=');
                if (splitted.size() != 2)
                {
                    throw std::runtime_error(std::format("Sentence: \"{}\", expected to fit this pattern: id=<worker-id>", word));
                }

                result.id = std::stoi(splitted[1]);
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
        return result;
    }

    StoreHauseData parseStoreHause(const std::vector<std::string> &input)
    {
        if (input.size() != 2)
        {
            throw std::runtime_error("Expected this line to fit this pattern: LOADING_RAMP id=<ramp-id> delivery-interval=<delivery-interval>");
        }

        StoreHauseData result;

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

                result.id = std::stoi(splitted[1]);
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
        return result;
    }
}