#pragma once
#ifndef LOAD_BMP_H
#define LOAD_BMP_H

#include <cstdint>
#include <vector>

bool loadBMP(
    const char * path,
    unsigned int & width,
    unsigned int & height,
    std::vector < std::uint8_t > & bitmap
);

#endif // LOAD_BMP_H
