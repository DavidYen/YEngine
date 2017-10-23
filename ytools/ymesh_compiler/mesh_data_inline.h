#ifndef YTOOLS_YMESH_COMPILER_MESH_DATA_INLINE_H
#define YTOOLS_YMESH_COMPILER_MESH_DATA_INLINE_H

#include <vector>

#include <rapidjson/document.h>

namespace ytools { namespace ymesh_compiler {

namespace MeshDataInline {
  bool ProcessInlineData(const char* name,
                         const rapidjson::Document& doc,
                         std::vector<uint8_t>& output);
}

}} // namespace ytools { namespace ymesh_compiler {

#endif // YTOOLS_YMESH_COMPILER_MESH_DATA_INLINE_H
