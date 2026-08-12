#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <ostream>

std::vector<std::string> split(const std::string& str, char d);
uint32_t parse_ip(const std::string& ip_str);
void print_ip(uint32_t ip, std::ostream& out);