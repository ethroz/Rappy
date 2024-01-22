#include <memory>
#include <stdexcept>
#include <string>
#include <fmt/core.h>

#include "bluetooth/Device.hpp"

#include "program/Base.hpp"

using namespace program;
using namespace bluetooth;

class Prgm : public Base {
public:
    Prgm(std::string_view nm) : Base(nm) {
        parser.addOptional(name, "name", "The name of the bluetooth device");
        parser.addOptional(address, "address", "The address of the bluetooth device");
        parser.addOptional(timeout, "timeout", "The number of seconds for each scan for devices");

        examples.push_back(fmt::format("{} --name \"Wireless Controller\"", prgmName));
        examples.push_back(fmt::format("{} --address 12:34:56:78:90:AB", prgmName));
        examples.push_back(fmt::format("{} --name \"Wireless Controller\" --timeout 5", prgmName));
    }

    void init() override {
        if (name.empty() && address.empty()) {
            throw std::invalid_argument("Must specify a name or an address");
        }
        if (!name.empty() && !address.empty()) {
            throw std::invalid_argument("Cannot specify a name AND an address");
        }
        
        if (address.empty()) {
            // Timeout calculations.
            timeout *= 100;
            timeout %= 128;
            timeout = std::max(timeout, 1);

            device = std::make_unique<Device>(Device::fromName(name, timeout));
        }
        else {
            device = std::make_unique<Device>(Device::fromAddress(address));
        }
        running = false;
    }
    
    void loop() override {}

private:
    std::string name;
    std::string address;
    int timeout = 1;
    std::unique_ptr<Device> device;
};

int main(int argc, char* argv[]) {
    return std::make_unique<Prgm>(argv[0])->run(argc, argv);
}

