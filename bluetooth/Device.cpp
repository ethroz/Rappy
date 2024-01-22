#include <array>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <bluetooth/rfcomm.h>

#include "utils/Logger.hpp"

#include "Device.hpp"

namespace bluetooth {

// Local function.
[[nodiscard]]
Socket createSocket(const bdaddr_t& addr) {
    // Create a socket.
    Socket socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

    // Set the conenction address.
    sockaddr_rc address{};
    address.rc_family = AF_BLUETOOTH;
    address.rc_channel = uint8_t(1); // Supposedly 1 is for SDP which is required.
    baswap(&address.rc_bdaddr, &addr);

    // Connect to the device.
    if (connect(socket.fd(), (sockaddr*)&address, sizeof(address)) != 0) {
        throw std::system_error(errno, std::generic_category(), "connect");
    }

    return socket;
}

Device Device::fromAddress(const std::string& address) {
    bdaddr_t addr{};
    if (str2ba(address.c_str(), &addr) < 0) {
        throw std::system_error(errno, std::generic_category(), "str2ba");
    }
    return Device(createSocket(addr));
}

Device Device::fromName(std::string_view name, int timeout) {
    const auto deviceId = hci_get_route(nullptr);
    if (deviceId < 0) {
        throw std::system_error(errno, std::generic_category(), "hci_get_route");
    }
    const Socket devSock = hci_open_dev(deviceId);
    if (deviceId < 0 || devSock < 0) {
        throw std::system_error(errno, std::generic_category(), "hci_open_dev");
    }

    constexpr long flags = IREQ_CACHE_FLUSH;
    std::array<inquiry_info, 256> devices{};
    inquiry_info* devicesPtr = devices.data();
    int deviceIndex = -1;
    
    while (deviceIndex < 0) {
        Log() << "Scanning for devices";
        const auto numDevices = hci_inquiry(deviceId, 1, devices.size(), nullptr, &devicesPtr, flags);
        if (numDevices < 0) {
            throw std::system_error(errno, std::generic_category(), "hci_inquiry");
        }
        
        for (int i = 0; i < numDevices; i++) {
            std::array<char, 248> charBuffer{};
            if (hci_read_remote_name(devSock, &devices[i].bdaddr, charBuffer.size(), charBuffer.data(), 0) >= 0) {
                Log() << "Found device: " << charBuffer.data();
                if (charBuffer.data() == name) {
                    deviceIndex = i;
                    break;
                }
            }
        }
    }

    const auto& addr = devices[deviceIndex].bdaddr;
    
    std::string addrStr;
    addrStr.resize(17);
    ba2str(&addr, addrStr.data());
    Log() << "Device address: " << addrStr;

    return Device(createSocket(addr));
}

} // namespace bluetooth

