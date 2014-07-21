
import os
import hashlib

def PathFilters():
  return { 'ypy_abspath': os.path.abspath,
           'ypy_basename': os.path.basename,
           'ypy_dirname': os.path.dirname,
           'ypy_fileext' : lambda x : os.path.splitext(x)[1],
           'ypy_join': os.path.join,
           'ypy_normpath': os.path.normpath,
           'ypy_relpath': os.path.relpath,
           'ypy_removeext': lambda x : os.path.splitext(x)[0],
         }

def DebugFilters():
  def __PrintFilter(string_item):
    print string_item
    return string_item

  return { 'ypy_print': __PrintFilter,
         }

def VSFilters():
  def __VSProjID(string_item):
    hash_value = hashlib.md5(string_item).hexdigest().upper()
    return '{%s-%s-%s-%s-%s}' % (hash_value[0:8],
                                 hash_value[8:12],
                                 hash_value[12:16],
                                 hash_value[16:20],
                                 hash_value[20:32])

  return { 'ypy_vsprojid' : __VSProjID,
         }
