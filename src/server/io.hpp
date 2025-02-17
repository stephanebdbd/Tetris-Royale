#ifndef IO_HPP
#define IO_HPP

#include "../include.hpp"



bool ReadStreamMessage(int fd, char* buffer, uint32_t maxSize, uint32_t& size);

bool WriteStreamMessage(int fd, const char* buffer, uint32_t size);

#endif 