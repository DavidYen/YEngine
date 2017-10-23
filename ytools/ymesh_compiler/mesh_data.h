#ifndef YTOOLS_YMESH_COMPILER_MESH_DATA_H
#define YTOOLS_YMESH_COMPILER_MESH_DATA_H

#include <string>
#include <vector>

namespace ytools { namespace file_utils {
  class FileEnv;
}} // namespace ytools { namespace file_utils {

namespace ytools { namespace ymesh_compiler {

class MeshData {
 public:
  MeshData(ytools::file_utils::FileEnv* file_env,
            const std::string& file_path);
  ~MeshData() = default;

  bool IsValid() { return mValid; }
  bool WriteModelBinaryFile(ytools::file_utils::FileEnv* file_env,
                            const std::string& file_path);

 private:
  bool ProcessMeshData(ytools::file_utils::FileEnv* file_env,
                       const std::string& file_path);

  bool mValid = false;
  std::vector<uint8_t> mMeshData;
};

}} // namespace ytools { namespace ymesh_compiler {

#endif // YTOOLS_YMESH_COMPILER_MESH_DATA_H
