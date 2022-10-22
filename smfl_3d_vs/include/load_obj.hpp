#pragma once
#ifndef LOAD_OBJ_HPP
#define LOAD_OBJ_HPP

#include <vector>


struct XYZUVN_VERTEX
{
  float px;
  float py;
  float pz;
  float tx;
  float ty;
  float nx;
  float ny;
  float nz;
};

struct OBJ_VEC2
{
  float x;
  float y;
};

struct OBJ_VEC3
{
  float x;
  float y;
  float z;
};


bool loadOBJ(
    const char * path,
    std::vector < XYZUVN_VERTEX > & out_vertices,
    std::vector < unsigned int > & out_indexes
);

#endif // LOAD_OBJ_HPP
