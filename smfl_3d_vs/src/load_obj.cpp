#include "load_obj.hpp"
#include <cstdio>
#include <cstring>


bool loadOBJ(
    const char * path,
    std::vector < XYZUVN_VERTEX > & out_vertices,
    std::vector < unsigned int > & out_indexes
)
{
  std::vector< OBJ_VEC2 > temp_uvs;
  std::vector< OBJ_VEC3 > temp_normals;

  FILE * file = nullptr;
  fopen_s(&file, path, "r");
  if( file == nullptr )
  {
    //printf("Impossible to open the file !\n");
    return false;
  }

  while ( file != nullptr )
  {
    char lineBuffer[80];
    char const *lineHeader = fgets(lineBuffer, sizeof(lineBuffer), file);

    if (lineHeader == nullptr)
    {
      break; // EOF = End Of File. Quit the loop.
    }

    if ( memcmp( lineHeader, "v ", 2 ) == 0 )
    {
      XYZUVN_VERTEX vertex = {0};
      sscanf_s(&lineHeader[2], "%f %f %f\n", &vertex.px, &vertex.py, &vertex.pz );
      out_vertices.push_back(vertex);
    }
    else
    if ( memcmp( lineHeader, "vt ", 3 ) == 0 )
    {
      OBJ_VEC2 uv;
      sscanf_s(&lineHeader[3], "%f %f\n", &uv.x, &uv.y );
      temp_uvs.push_back(uv);
    }
    else
    if ( memcmp( lineHeader, "vn ", 3 ) == 0 )
    {
      OBJ_VEC3 normal;
      sscanf_s(&lineHeader[3], "%f %f %f\n", &normal.x, &normal.y, &normal.z );
      temp_normals.push_back(normal);
    }
    else
    if ( memcmp( lineHeader, "f ", 2 ) == 0 )
    {
      unsigned int vertexIndex[3] = {0};
      unsigned int uvIndex[3] = {0};
      unsigned int normalIndex[3] = {0};
      int matches = sscanf_s(&lineHeader[2], "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2] );
      if (matches != 9)
      {
          //printf("File can't be read by our simple parser : ( Try exporting with other options\n");
          return false;
      }

      for (auto i = 0; i < 3; ++i)
      {
        if (vertexIndex[i] > 0)
        {
          unsigned int indexI = vertexIndex[i] - 1;
          out_indexes.push_back(indexI);

          XYZUVN_VERTEX &out_vertex = out_vertices[indexI];

          if (uvIndex[i] > 0)
          {
            unsigned int indexT = uvIndex[i] - 1;
            out_vertex.tx = temp_uvs[indexT].x;
            out_vertex.ty = temp_uvs[indexT].y;
          }

          if (normalIndex[i] > 0)
          {
            unsigned int indexN = normalIndex[i] - 1;
            out_vertex.nx = temp_normals[indexN].x;
            out_vertex.ny = temp_normals[indexN].y;
            out_vertex.nz = temp_normals[indexN].z;
          }
        }
      }
    }
  }

  if (out_indexes.size() == 0)
  {
    for (unsigned int i = 0; i < out_vertices.size(); ++i)
    {
      out_indexes.push_back(i);

      XYZUVN_VERTEX &out_vertex = out_vertices[i];

      if (i < temp_uvs.size())
      {
        out_vertex.tx = temp_uvs[i].x;
        out_vertex.ty = temp_uvs[i].y;
      }

      if (i < temp_normals.size())
      {
        out_vertex.nx = temp_normals[i].x;
        out_vertex.ny = temp_normals[i].y;
        out_vertex.nz = temp_normals[i].z;
      }
    }
  }

  return (true);
}
