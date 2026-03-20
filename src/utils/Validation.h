#pragma once

#include <regex>
#include <string>
#include "models/ApiDtos.h"

namespace api::validation {

// Very simple skeleton validation.
// For production, use stronger IPv4/IPv6 validation.
inline bool isNonEmpty(const std::string& s) {
    return !s.empty();
}

inline bool looksLikeIp(const std::string& ip) {
    static const std::regex ipv4(R"(^(\d{1,3}\.){3}\d{1,3}$)");
    static const std::regex ipv6(R"(^[0-9A-Fa-f:]+$)");
    return std::regex_match(ip, ipv4) || std::regex_match(ip, ipv6);
}

inline void validate(const dto::IpPoolRequest& req) {
    if (req.ipAddresses.empty()) {
        throw std::invalid_argument("ipAddresses must not be empty");
    }
    for (const auto& ip : req.ipAddresses) {
        if (!looksLikeIp(ip.ip)) {
            throw std::invalid_argument("Invalid IP: " + ip.ip);
        }
        if (!ip.ipType.has_value()) {
            throw std::invalid_argument("ipType is required");
        }
    }
}

inline void validate(const dto::ReserveIpRequest& req) {
    if (!isNonEmpty(req.serviceId)) {
        throw std::invalid_argument("serviceId must not be empty");
    }
}

inline void validate(const dto::AssignTerminateRequest& req) {
    if (!isNonEmpty(req.serviceId)) {
        throw std::invalid_argument("serviceId must not be empty");
    }
    if (req.ipAddresses.empty()) {
        throw std::invalid_argument("ipAddresses must not be empty");
    }
    for (const auto& ip : req.ipAddresses) {
        if (!looksLikeIp(ip.ip)) {
            throw std::invalid_argument("Invalid IP: " + ip.ip);
        }
    }
}

inline void validate(const dto::ServiceIdChangeRequest& req) {
    if (!isNonEmpty(req.serviceIdOld) || !isNonEmpty(req.serviceId)) {
        throw std::invalid_argument("serviceIdOld and serviceId must not be empty");
    }
}

}  // namespace api::validation