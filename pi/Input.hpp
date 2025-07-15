#pragma once

#include <functional>
#include <memory>
#include <string_view>

#include <boost/json.hpp>

namespace pi {

using Producer = std::function<float()>;

class Input {
public:
    virtual ~Input() {}

    static std::unique_ptr<Input> create(const boost::json::object& cfg);

    virtual void poll() = 0;

    virtual Producer getProducer(std::string_view key) const = 0;

    virtual float read(std::string_view key) const { return getProducer(key)(); };

    std::string_view type() const { return m_type; }

protected:
    Input(std::string_view type) : m_type(type) {}

private:
    std::string m_type;
};

} // namespace pi
