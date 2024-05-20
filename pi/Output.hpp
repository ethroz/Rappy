#pragma once

#include <memory>
#include <string_view>

#include <boost/json.hpp>

using namespace boost;

namespace pi {

using Consumer = std::function<void(float)>;

class Output {
public:
    virtual ~Output() {}

    static std::unique_ptr<Output> create(const json::object& cfg);

    virtual Consumer getConsumer(std::string_view key) = 0;

    virtual void step() = 0;

    std::string_view type() const { return m_type; }

protected:
    Output(std::string_view type) : m_type(type) {}

private:
    std::string m_type;
};

} // namespace pi
