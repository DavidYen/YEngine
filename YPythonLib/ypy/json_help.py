
import json

def ParseCommentedJson(filepath):
  """Parse a JSON file while stripping out # comments."""
  contents = ''
  with open( filepath, 'rt' ) as f:
    for line in f.readlines():
      comment_begin = line.find('#')
      if comment_begin != -1:
        contents += line[:comment_begin] + '\n'
      else:
        contents += line

  return json.loads(contents)
