#include "ytools/ymesh_compiler/mesh_data_inline.h"

#include <iostream>
#include <unordered_set>

#include <schemas/mesh_generated.h>

#include "ytools/report_utils/json_errors.h"

namespace {

yengine_data::VertexUsageType GetUsageType(const char* usage_type) {
  for (int i = static_cast<int>(yengine_data::VertexUsageType::MIN);
       i <= static_cast<int>(yengine_data::VertexUsageType::MAX);
       ++i) {
    if (strcmp(usage_type, yengine_data::EnumNamesVertexUsageType()[i]) == 0) {
      return static_cast<yengine_data::VertexUsageType>(i);
    }
  }
  return yengine_data::VertexUsageType::kInvalid;
}

yengine_data::VertexMemberType GetDataType(const char* member_type) {
  for (int i = static_cast<int>(yengine_data::VertexMemberType::MIN);
       i <= static_cast<int>(yengine_data::VertexMemberType::MAX);
       ++i) {
    if (strcmp(member_type,
               yengine_data::EnumNamesVertexMemberType()[i]) == 0) {
      return static_cast<yengine_data::VertexMemberType>(i);
    }
  }
  return yengine_data::VertexMemberType::kInvalid;
}

size_t GetDataTypeElementCount(yengine_data::VertexMemberType data_type) {
  static size_t kElementCount[] = {
    0,   // kInvalid
    1,   // kFloat,
    2,   // kFloat2
    3,   // kFloat3
  };
  static_assert(sizeof(kElementCount)/sizeof(int) ==
                static_cast<size_t>(yengine_data::VertexMemberType::MAX) + 1,
                "kElementCount must be kept up to date with VertexMemberType.");
  return kElementCount[static_cast<int>(data_type)];
}

yengine_data::DrawFormat GetDataFormat(const char* data_format) {
  for (int i = static_cast<int>(yengine_data::DrawFormat::MIN);
       i <= static_cast<int>(yengine_data::DrawFormat::MAX);
       ++i) {
    if (strcmp(data_format, yengine_data::EnumNamesDrawFormat()[i]) == 0) {
      return static_cast<yengine_data::DrawFormat>(i);
    }
  }
  return yengine_data::DrawFormat::kInvalid;
}

size_t GetFormatElementCount(yengine_data::DrawFormat data_format) {
  static size_t kFormatElementCount[] = {
    0, // kInvalid = 0
    3, // kTriangleList = 1
  };
  static_assert(sizeof(kFormatElementCount)/sizeof(int) ==
                static_cast<size_t>(yengine_data::DrawFormat::MAX) + 1,
                "kFormatElementCount must be kept up to date with DrawFormat.");
  return kFormatElementCount[static_cast<int>(data_format)];
}

} // namespace

namespace ytools { namespace ymesh_compiler {

bool MeshDataInline::ProcessInlineData(const char* name,
                                       const rapidjson::Document& doc,
                                       std::vector<uint8_t>& output) {
  flatbuffers::FlatBufferBuilder fbb;

  const auto& inline_data_iter = doc.FindMember("inline_data");
  if (inline_data_iter == doc.MemberEnd() ||
      !inline_data_iter->value.IsObject()) {
    std::cerr << "Inline Mesh data expected \"inline_data\" object."
              << std::endl;
    return false;
  }
  const auto& inline_vertex_data = inline_data_iter->value.GetObject();

  // Parse the format.
  const auto& format_iter = inline_vertex_data.FindMember("format");
  if (format_iter == inline_vertex_data.MemberEnd() ||
      !format_iter->value.IsString()) {
    std::cerr << "Inline mesh data expected string \"format\" field."
              << std::endl;
    return false;
  }

  const yengine_data::DrawFormat draw_format =
      GetDataFormat(format_iter->value.GetString());
  if (draw_format == yengine_data::DrawFormat::kInvalid) {
    std::cerr << "Invalid mesh data format: "
              << format_iter->value.GetString()
              << std::endl;
    return false;
  }

  const size_t format_element_count = GetFormatElementCount(draw_format);

  // Parse the count.
  const auto& count_iter = inline_vertex_data.FindMember("count");
  if (count_iter == inline_vertex_data.MemberEnd() ||
      !count_iter->value.IsUint()) {
    std::cerr << "Inline mesh data expected unsigned integer \"count\" field."
              << std::endl;
    return false;
  }

  const uint32_t num_primitives = count_iter->value.GetUint();
  std::unordered_set<yengine_data::VertexUsageType> seen_usages;

  // Parse the vertex data lists
  const auto& vertex_datas_iter = inline_vertex_data.FindMember("vertex_datas");
  if (vertex_datas_iter == inline_vertex_data.MemberEnd() ||
      !vertex_datas_iter->value.IsArray()) {
    std::cerr << "Inline Mesh data expected \"vertex_datas\" array."
              << std::endl;
    return false;
  }

  std::vector<flatbuffers::Offset<yengine_data::VertexList> > vertex_datas;
  for (const auto& vertex_data_iter : vertex_datas_iter->value.GetArray()) {
    if (!vertex_data_iter.IsObject()) {
      std::cerr << "Unexpected vertex data item value, expected object."
                << std::endl;
      return false;
    }
    const auto& vertex_data = vertex_data_iter.GetObject();

    // Parse the usage.
    const auto& usage_iter = vertex_data.FindMember("usage");
    if (usage_iter == vertex_data.MemberEnd() ||
        !usage_iter->value.IsString()) {
      std::cerr << "Inline mesh data expected string \"usage\" field."
                << std::endl;
      return false;
    }

    const yengine_data::VertexUsageType usage_type =
        GetUsageType(usage_iter->value.GetString());
    if (usage_type == yengine_data::VertexUsageType::kInvalid) {
      std::cerr << "Invalid mesh data usage type: "
                << usage_iter->value.GetString()
                << std::endl;
      return false;
    }
    else if (seen_usages.count(usage_type)) {
      std::cerr << "Cannot specify same usage type multiple times: "
                << usage_iter->value.GetString()
                << std::endl;
      return false;
    }

    // Parse the data type.
    const auto& data_type_iter = vertex_data.FindMember("data_type");
    if (data_type_iter == vertex_data.MemberEnd() ||
        !data_type_iter->value.IsString()) {
      std::cerr << "Inline mesh data expected string \"data_type\" field."
                << std::endl;
      return false;
    }

    const yengine_data::VertexMemberType data_type =
        GetDataType(data_type_iter->value.GetString());
    if (data_type == yengine_data::VertexMemberType::kInvalid) {
      std::cerr << "Invalid mesh data type: "
                << data_type_iter->value.GetString()
                << std::endl;
      return false;
    }

    // Parse divisor.
    const auto& divisor_iter = vertex_data.FindMember("divisor");
    uint32_t divisor = 1;
    if (divisor_iter != vertex_data.MemberEnd()) {
      if (!divisor_iter->value.IsUint()) {
        std::cerr << "Inline mesh data has unexpected \"divisor\" value."
                  << std::endl;
        return false;
      }
      divisor = divisor_iter->value.GetUint();
    }

    // Parse the vertices.
    const auto& vertices_iter = vertex_data.FindMember("vertices");
    if (vertices_iter == vertex_data.MemberEnd() ||
        !vertices_iter->value.IsArray()) {
      std::cerr << "Vertex data list expected \"vertices\" list field."
                << std::endl;
      return false;
    }

    std::vector<float> vertices;
    const size_t num_elements = GetDataTypeElementCount(data_type);
    for (const auto& vertex_elements_iter : vertices_iter->value.GetArray()) {
      if (!vertex_elements_iter.IsArray()) {
        std::cerr << "Vertex Type (" << data_type_iter->value.GetString() << ")"
                  << " expected list of elements." << std::endl;
        return false;
      } else if (vertex_elements_iter.GetArray().Size() != num_elements) {
        std::cerr << "Vertex Type (" << data_type_iter->value.GetString() << ")"
                  << " expected " << num_elements << " elements. Received: "
                  << vertex_elements_iter.GetArray().Size() << std::endl;
        return false;
      }

      for (const auto& elem_iter : vertex_elements_iter.GetArray()) {
        if (!elem_iter.IsFloat() && !elem_iter.IsInt()) {
          std::cerr << "Vertex Element must be of type float, received "
                    << report_utils::JsonErrors::StringifyValueType(elem_iter)
                    << std::endl;
          return false;
        }

        vertices.push_back(elem_iter.GetFloat());
      }
    }

    // Is Indexed?
    std::vector<uint32_t> indexes;
    const auto& indexed_iter = vertex_data.FindMember("indexed");
    if (indexed_iter == vertex_data.end() ||
        !indexed_iter->value.IsBool() ||
        !indexed_iter->value.GetBool()) {
      // Not indexed.
      const size_t expected_vertices = num_primitives * format_element_count;
      const size_t num_vertices = vertices.size() * divisor;
      if (expected_vertices != num_vertices) {
        std::cerr << "Expected " << expected_vertices << "vertices, received: "
                  << vertices.size() << " * " << divisor << "." << std::endl;
        return false;
      }
    } else {
      // Is indexed.
      const auto& indices_iter = vertex_data.FindMember("indices");
      if (indices_iter == vertex_data.MemberEnd() ||
          !indices_iter->value.IsArray()) {
        std::cerr << "Vertex indexed data expected \"indices\" list field."
                  << std::endl;
        return false;
      }

      for (const auto& index_element_iter : indices_iter->value.GetArray()) {
        if (!index_element_iter.IsUint()) {
          std::cerr << "Index Element must be of type unsigned integer."
                    << std::endl;
          return false;
        }

        indexes.push_back(index_element_iter.GetUint());
      }

      const size_t expected_indices = num_primitives * format_element_count;
      const size_t num_indices = indexes.size() * divisor;
      if (expected_indices != num_indices) {
        std::cerr << "Usage (" << usage_iter->value.GetString() << ")"
                  << " Expected " << expected_indices << " indices, received: "
                  << indexes.size() << " * " << divisor << "." << std::endl;
        return false;
      }
    }

    vertex_datas.push_back(yengine_data::CreateVertexList(
        fbb,
        usage_type,
        data_type,
        !indexes.empty(),
        fbb.CreateVector(vertices),
        fbb.CreateVector(indexes)
    ));
  }

  yengine_data::FinishMeshBuffer(fbb, yengine_data::CreateMesh(
      fbb,
      fbb.CreateString(name),
      draw_format,
      fbb.CreateVector(vertex_datas)
  ));

  output.clear();
  output.insert(
      output.begin(),
      fbb.GetBufferPointer(),
      fbb.GetBufferPointer() + fbb.GetSize()
  );
  return true;
}

}} // namespace ytools { namespace ymesh_compiler {
