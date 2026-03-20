#pragma once

#include <optional>
#include <string>
#include <vector>

namespace api::dto {

enum class IpType {
    IPv4,
    IPv6,
    Both
};

struct IpAddress {
    std::string ip;
    std::optional<IpType> ipType;
};

struct IpPoolRequest {
    std::vector<IpAddress> ipAddresses;
};

struct ReserveIpRequest {
    std::string serviceId;
    IpType ipType{IpType::Both};
};

struct AssignTerminateRequest {
    std::string serviceId;
    std::vector<IpAddress> ipAddresses;
};

struct ServiceIdChangeRequest {
    std::string serviceIdOld;
    std::string serviceId;
};

struct StatusResponse {
    std::string statusCode{"0"};
    std::string statusMessage{"Successful operation. OK"};
};

struct IpAddressesResponse {
    std::vector<IpAddress> ipAddresses;
};

}  // namespace api::dto