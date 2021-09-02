#include <string>
#include <sstream>
#include <format>

#include "Utils.hpp"
#include "Factory.hpp"

namespace sd
{
    namespace
    {
        const std::string linkPattern = "LINK id=<id> src=<type>-<id> dest=<type>-<id> p=<probability>, where id is unique indentificator, <type> is one of following: ramp/worker/store and <probability> is in range <0:1>";
        const std::string workPattern = "WORKER id=<worker-id> processing-time=<processing-time> queue-type=<queuetype>, where id is unique indentificator, <processing-time> number grather than zero describing time of processing the product by worker and <queue-type> 0 - LIFO, 1 - FIFO describind worker processing mode";
        const std::string rampPattern = "LOADING_RAMP id=<ramp-id> delivery-interval=<delivery-interval>, where id is unique indentificator, <delivery-interval> number grather than zero describing time of delivering the product by ramp";
        const std::string storePattern = "STOREHOUSE id=<storehouse-id>, where id is unique indentificator";

        void checkSize(size_t actual, size_t expected, const std::string &msg)
        {
            if (actual != expected)
            {
                throw std::runtime_error(msg);
            }
        }

        size_t getId(const std::string &word)
        {
            auto splitted = splitStr(word, '=');
            if (splitted.size() != 2)
            {
                throw std::runtime_error(std::format("Sentence: \"{}\", expected to fit this pattern: id=<id>, where <id> is idenfificator number grather than zero", word));
            }

            return std::stoull(splitted[1]);
        }

        LinkBind getLinkBind(const std::string &word, const ::std::string &errorMsg)
        {
            LinkBind result;
            auto splitted = splitStr(word, '-');
            if (splitted.size() != 2)
            {
                throw std::runtime_error(errorMsg);
            }

            if (splitted[0].ends_with("ramp"))
            {
                result.type = NodeType::RAMP;
            }
            else if (splitted[0].ends_with("worker"))
            {
                result.type = NodeType::WORKER;
            }
            else if (splitted[0].ends_with("store"))
            {
                result.type = NodeType::STORE;
            }
            else
            {
                throw std::runtime_error(errorMsg);
            }
            result.id = std::stoull(splitted[1]);
            return result;
        }

        void define(bool &def, const ::std::string &option)
        {
            if (def)
            {
                throw std::runtime_error(std::format("This option was already provided: {}", option));
            }
            def = true;
        }

        void checkDefine(bool &def, const ::std::string &option)
        {
            if (!def)
            {
                throw std::runtime_error(std::format("This option was not provided: {}", option));
            }
        }

        std::vector<std::string> popFront(std::vector<std::string> &vec)
        {
            vec.erase(vec.begin());
            return vec;
        }

        LinkData parseLink(const std::vector<std::string> &input)
        {
            checkSize(input.size(), 4, std::format("Expected this line to fit this pattern: {}", linkPattern));

            LinkData result;
            bool scrCheck = false, destCheck = false, idCheck = false, pCheck = false;
            for (auto &word : input)
            {
                if (word.starts_with("id="))
                {
                    result.id = getId(word);
                    define(idCheck, word);
                }
                else if (word.starts_with("src="))
                {
                    result.source = getLinkBind(word, std::format("Sentence: \"{}\", expected to fit this pattern: src=type-id, where type is one of ramp, worker and id is identifier", word));
                    define(scrCheck, word);
                }
                else if (word.starts_with("dest="))
                {
                    result.destination = getLinkBind(word, std::format("Sentence: \"{}\", expected to fit this pattern: dest=type-id, where type is one of store, worker and id is identifier", word));
                    define(destCheck, word);
                }
                else if (word.starts_with("p"))
                {
                    auto splitted = splitStr(word, '=');
                    result.probability = std::stod(splitted[1]);
                    define(pCheck, word);
                }
                else
                {
                    throw std::runtime_error(std::format("Expected this line to fit this pattern: {}", linkPattern));
                }
            }

            checkDefine(scrCheck, "source");
            checkDefine(idCheck, "id");
            checkDefine(destCheck, "destination");
            checkDefine(pCheck, "probability");
            return result;
        }

        WorkerData parseWorker(const std::vector<std::string> &input)
        {
            checkSize(input.size(), 3, std::format("Expected this line to fit this pattern: {}", workPattern));

            WorkerData result;

            bool processCheck = false, idCheck = false, typeCheck = false;
            for (auto &word : input)
            {
                if (word.starts_with("processing-time="))
                {
                    auto splitted = splitStr(word, '=');
                    if (splitted.size() != 2)
                    {
                        throw std::runtime_error(std::format("Sentence: \"{}\", expected to fit this pattern: processing-time=<processing-time>", word));
                    }

                    result.processingTime = std::stoull(splitted[1]);
                    define(processCheck, word);
                }
                else if (word.starts_with("id="))
                {
                    result.id = getId(word);
                    define(idCheck, word);
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
                    define(typeCheck, word);
                }
                else
                {
                    throw std::runtime_error(std::format("Expected this line to fit this pattern: {}", workPattern));
                }
            }
            checkDefine(processCheck, "processing-time");
            checkDefine(idCheck, "id");
            checkDefine(typeCheck, "queue-type");
            return result;
        }

        LoadingRampData parseLoadingRamp(const std::vector<std::string> &input)
        {
            checkSize(input.size(), 2, std::format("Expected this line to fit this pattern: {}", rampPattern));

            LoadingRampData result;

            bool deliveryCheck = false, idCheck = false;
            for (auto &word : input)
            {
                if (word.starts_with("delivery-interval="))
                {
                    auto splitted = splitStr(word, '=');
                    if (splitted.size() != 2)
                    {
                        throw std::runtime_error(std::format("Sentence: \"{}\", expected to fit this pattern: delivery-interval=<delivery-interval>", word));
                    }

                    result.deliveryInterval = std::stoi(splitted[1]);
                    define(deliveryCheck, word);
                }
                else if (word.starts_with("id="))
                {
                    result.id = getId(word);
                    define(idCheck, word);
                }
                else
                {
                    throw std::runtime_error(std::format("Expected this line to fit this pattern: {}", rampPattern));
                }
            }
            checkDefine(idCheck, "id");
            checkDefine(deliveryCheck, "delivery-interval");
            return result;
        }

        StoreHouseData parseStoreHouse(const std::vector<std::string> &input)
        {
            checkSize(input.size(), 1, std::format("Expected this line to fit this pattern: {}", storePattern));

            StoreHouseData result;

            bool idCheck = false;
            for (auto &word : input)
            {
                if (word.starts_with("id="))
                {
                    result.id = getId(word);
                    define(idCheck, word);
                }
                else
                {
                    throw std::runtime_error(std::format("Expected this line to fit this pattern: {}", storePattern));
                }
            }
            checkDefine(idCheck, "id");
            return result;
        }
    }

    std::vector<std::string> splitStr(const std::string &str, char splitChar)
    {
        std::vector<std::string> out;
        std::istringstream iss(str);
        std::string s;
        while (std::getline(iss, s, splitChar))
        {
            out.push_back(s);
        }
        return out;
    }

    std::istream &operator>>(std::istream &stream, Factory &factory)
    {
        size_t lineCnt = 0;
        try
        {
            auto ptr = std::make_unique<Factory>();
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
                    factory.addWorker(parseWorker(popFront(splitted)));
                }
                else if (splitted.front() == "LOADING_RAMP")
                {
                    factory.addLoadingRamp(parseLoadingRamp(popFront(splitted)));
                }
                else if (splitted.front() == "STOREHOUSE")
                {
                    factory.addStorehouse(parseStoreHouse(popFront(splitted)));
                }
                else if (splitted.front() == "LINK")
                {
                    factory.addLink(parseLink(popFront(splitted)));
                }
                else
                {
                    throw std::runtime_error("Expected word: WORKER | LOADING_RAMP | STOREHOUSE | LINK");
                }
            }
        }
        catch (std::exception &e)
        {
            throw std::runtime_error(std::format("Error in line {}: {}", lineCnt, e.what()));
        }
        catch (...)
        {
            throw std::runtime_error(std::format("Unexpected error in line {}", lineCnt));
        }
        return stream;
    }

    std::ostream &operator<<(std::ostream &stream, const Factory &factory)
    {
        stream << "; == LOADING RAMPS ==" << std::endl
               << std::endl;
        for (auto &data : factory.getLoadingRampsData())
        {
            stream << std::format("LOADING_RAMP id={} delivery-interval={}", data.id, data.deliveryInterval) << std::endl;
        }
        stream << std::endl
               << "; == WORKERS ==" << std::endl
               << std::endl;
        for (auto &data : factory.getWorkersData())
        {
            stream << std::format("WORKER id={} processing-time={} queue-type={}", data.id, data.processingTime, toString(data.type)) << std::endl;
        }
        stream << std::endl
               << "; == STOREHOUSES ==" << std::endl
               << std::endl;
        for (auto &data : factory.getStorehousesData())
        {
            stream << std::format("STOREHOUSE id={}", data.id) << std::endl;
        }

        stream << std::endl
               << "; == LINKS ==" << std::endl
               << std::endl;
        for (auto &data : factory.getLinksData())
        {
            stream << std::format("LINK id={} src={}-{} dest={}-{} p={}", data.id, toString(data.source.type), data.source.id, toString(data.destination.type), data.destination.id, data.probability) << std::endl;
        }
        return stream;
    }

    std::string getOffset(size_t offset) { return std::string(offset, '\t'); }

    std::string toString(NodeType type)
    {
        switch (type)
        {
        case NodeType::WORKER:
            return "worker";
        case NodeType::RAMP:
            return "ramp";
        case NodeType::STORE:
            return "store";
        default:
            return "";
        }
    }

    std::string toString(WorkerType type)
    {
        return type == WorkerType::FIFO ? "FIFO" : "LIFO";
    }
}