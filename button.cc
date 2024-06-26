#include <chrono>
#include <thread>
#include <fmt/format.h>

#include "utils/Other.hpp"
#include "wiring/Pin.hpp"

#include "program/Base.hpp"

using namespace program;
using namespace wiring;

class Prgm : public Base {
public:
    Prgm(std::string_view nm) : Base(nm) {
        parser.addPositional(pin, "pin", "The pin that is connected to the button");
        parser.addOptional(toggle, "toggle", "Each press toggles the trigger, otherwise the button must be held.");
        parser.addOptional(freq, "freq", "The frequency to poll the input.");

        examples.push_back(fmt::format("{} 6 --freq 100 --toggle", prgmName));
        examples.push_back(fmt::format("{} 6 --freq 10", prgmName));
    }

    void init() override {
        PinConfig config{pin, PinMode::IN, true};
        input = Pin::create(config);
        const auto fSec = std::chrono::duration<float>(1.0f / freq);
        sleepTime = std::chrono::round<std::chrono::nanoseconds>(fSec);        
        
        logger.info() << "Press the button to log a message...";
    }
    
    void loop() override {
        const auto pressed = input->get() == 1.0f;
   
        // Rising edge.
        if (pressed && !last && (!toggle || !on)) {
            on = true;
            logger.info() << "Button " << (toggle ? "toggled on" : "pressed");
        }
        // Falling edge.
        else if ((!toggle && !pressed && last) || (pressed && !last && on)) {
            on = false;
            logger.info() << "Button " << (toggle ? "toggled off" : "released");
        }

        last = pressed;

        std::this_thread::sleep_for(sleepTime);
    }

private:
    int pin;
    bool toggle = false;
    float freq = 100.0f;
    std::unique_ptr<Pin> input;
    std::chrono::nanoseconds sleepTime;
    bool on = false;
    bool last = false;
};

int main(int argc, char* argv[]) {
    return std::make_unique<Prgm>(argv[0])->run(argc, argv);
}

