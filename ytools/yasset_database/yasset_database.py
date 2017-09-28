#!/usr/bin/env python

import json
import os
import sys

# This map must be kept up to date with module_binary_types.fbs.
BINARY_TYPE_SHADER, = xrange(1)

def do_main(argv):
  if len(argv) < 3:
    print "Outputs an asset database binary file registering listed assets."
    print "Usage - %s <PYTHON_PATH> <OUT_FILE> <ASSET1> [ASSET2]..."
    return 1

  sys.path.append(argv[0])
  import yengine_data.Shader

  data_class_map = {
    ".shdr": (BINARY_TYPE_SHADER, yengine_data.Shader.Shader.GetRootAsShader),
  }

  output_file = argv[1]
  asset_database = {}

  # Read each asset based on their file extension.
  for file_path in argv[2:]:
    base_name, file_ext = os.path.splitext(file_path)
    data_info = data_class_map.get(file_ext)
    assert data_info, "Unknown File Extension: %s" % file_path

    binary_type, data_class = data_info
    with open(file_path, "rb") as f:
      data_object = data_class(bytearray(f.read()), 0)

    data_name = data_object.Name()

    existing_asset = asset_database.get(data_name)
    if existing_asset:
      print "Error - Duplicate asset name detected (%s)" % data_name
      print "  ", file_path
      print "  ", existing_asset["file_path"]
      return 1

    asset_database[data_name] = {
      "binary_type": binary_type,
      "file_path": file_path,
    }

  # Write out asset database json file
  with open(output_file, "wb") as f:
    json.dump(asset_database, f, indent=2, sort_keys=True,
              separators=(',', ': '))

  return 0

if __name__ == "__main__":
  sys.exit(do_main(sys.argv[1:]))
