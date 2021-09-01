#include <iostream>
#include <fstream>
#include <format>

#include "CLI11.hpp"
#include "Controller.hpp"
#include "FactoryUtils.hpp"

namespace sd
{
    namespace
    {
        // cli variable holders
        // general/store
        size_t id;
        // worker
        size_t processingTime;
        WorkerType queueType;
        // ramp
        size_t deliveryInterval;
        // link
        double probability;
        std::pair<size_t, NodeType> source;
        std::pair<size_t, NodeType> destination;

        bool breakFromCliMode = false;

        Factory::Ptr createFactory(const std::optional<std::string> &filePathOptional)
        {
            auto factory = std::make_unique<Factory>();
            if (filePathOptional)
            {
                auto filePath = *filePathOptional;
                if (!std::filesystem::exists(filePath))
                {
                    throw std::runtime_error(std::format("File {}, does not exists", filePath));
                }
                std::ifstream file(filePath);
                file >> *factory;
            }
            return std::move(factory);
        }

        void saveFactoryToFile(const Factory &factory, const std::filesystem::path &filePath)
        {
            std::ofstream file(filePath);
            file << factory;
        }
    }

    Controller::Controller(const Configuration &config, std::ostream &out, std::ostream &err, std::istream &in)
        : _config(config),
          _out(out),
          _in(in),
          _err(err)
    {
        buildCommandLineInterface();
    }

    Controller::~Controller() {}

    void Controller::buildCommandLineInterface()
    {
        _cli = std::make_unique<CLI::App>("CLI");

        auto addCommands = _cli->add_subcommand("add");

        auto addWorker = addCommands->add_subcommand("worker", "Adds new worker to factory");
        addWorker->add_option("-i,--id", id, "Worker Id, must be unique for all workers in factory")
            ->required();
        addWorker->add_option("-t,--processing-time", processingTime, "Processing time, describes how long this worker will process product")
            ->required();
        addWorker->add_option("-q,--queue-type", queueType, "Worker type, can be one of following: 0 - FIFO, 1 - LIFO")
            ->required();

        addWorker->callback(
            [this]()
            { _factory->addWorker({id, processingTime, queueType}); });

        auto addRamp = addCommands->add_subcommand("loading_ramp", "Adds new loading ramp to factory")->alias("ramp");
        addRamp->add_option("-i,--id", id, "Loading ramp Id, must be unique for all loading ramps in factory")
            ->required();
        addRamp->add_option("-t,--delivery-interval", deliveryInterval, "Delivery Interval, describes how often this loading ramp will deliver new product")
            ->required();

        addRamp->callback(
            [this]()
            { _factory->addLoadingRamp({id, deliveryInterval}); });

        auto addStore = addCommands->add_subcommand("storehause", "Adds new storehause to factory")->alias("store");
        addStore->add_option("-i,--id", id, "Storehause Id, must be unique for all storehauses in factory")
            ->required();

        addStore->callback(
            [this]()
            { _factory->addStorehause({id}); });

        auto addLink = addCommands->add_subcommand("link", "Adds new link to factory");
        addLink->add_option("-i,--id", id, "Link Id, must be unique for all links in factory")
            ->required();
        addLink->add_option("-s,--src", source, "Source, is a pair of two values: id of source object (worker/ramp) and type of this object: 0 - ramp, 1 - worker")
            ->required();
        addLink->add_option("-d,--dest", destination, "Destination, is a pair of two values: id of destination object (worker/store) and type of this object: 2 - store, 1 - worker")
            ->required();
        addLink->add_option("-p,--probability", probability, "Probability, is a probability of using this link to pass product further in factory")
            ->required();

        addLink->callback(
            [this]()
            {
                LinkBind scr{source.first, source.second};
                LinkBind dest{destination.first, destination.second};
                _factory->addLink({id, probability, scr, dest});
            });

        _cli->add_subcommand("run", "Runs the simulation")
            ->callback([this]()
                       {
                           _factory->validate();
                           breakFromCliMode = true;
                       });

        _cli->add_subcommand("save", "Saves factory structure to file")
            ->callback([this]()
                       { saveFactoryToFile(*_factory, *_config.structureFile); });

        _cli->add_subcommand("print", "Prints factory structure")
            ->callback([this]()
                       { getOut() << _factory->generateStructureRaport(); });
    }

    void Controller::run()
    {
        getOut() << " ============================ FACTORY SIMULATOR v 0.1 ============================ " << std::endl;
        _factory = createFactory(_config.structureFile);
        if (_factory->initialized())
        {
            getOut() << ">> factory structure is not initialized please add new elelemts to factory using cli, type -h for help <<" << std::endl;
        }
        _factory->validate();
        for (std::string line; !breakFromCliMode;)
        {
            try
            {
                getOut() << ">> ";
                if (!std::getline(getIn(), line))
                {
                    return;
                }
                _cli->parse(line);
            }
            catch (const CLI::Error &e)
            {
                _cli->exit(e, getOut(), getErr());
            }
            catch (const std::runtime_error &e)
            {
                getErr() << e.what() << std::endl;
            }
        }
        getOut() << " ============================== STARTING SIMULATION ============================== " << std::endl;
        runSimulation(_config.raportFile, _config.maxIterations, {_config.stateRaportTimings});
    }

    void Controller::runSimulation(const std::optional<std::string> &raportfilePath, size_t maxIterations, const Factory::RaportGuard &raportGuard)
    {
        if (raportfilePath)
        {
            std::ofstream file(*raportfilePath);
            _factory->run(maxIterations, file, raportGuard);
        }
        else
        {
            _factory->run(maxIterations, getOut(), raportGuard);
        }
    }

    std::ostream &Controller::getOut() { return _out; }

    std::ostream &Controller::getErr() { return _err; }

    std::istream &Controller::getIn() { return _in; }

}