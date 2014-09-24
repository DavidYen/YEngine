#!/usr/bin/python

import os
import sys

SCRIPT_DIR = os.path.dirname(__file__)

ENVIRON32_FILE = 'environment.x86'
ENVIRON64_FILE = 'environment.x64'

def WriteWindowEnvFile(env_dict, env_file):
  with open(env_file, 'wb') as f:
    for key, value in env_dict.iteritems():
      f.write(key + '=' + value + '\0')
    f.write('\0')

def DoMain(argv):
  if len(argv) != 2 or '--h' in argv or '--help' in argv or '\?' in argv:
    print 'Usage: %s vs_name vs_path' % sys.argv[0]
    return 1

  vs_name, vs_path = argv
  if vs_name == 'SDK':
    path32 = [os.path.join(vs_path, 'bin', 'x86')]
    path64 = [os.path.join(vs_path, 'bin', 'x64')]
  elif vs_name.startswith('VS'):
    path32 = [os.path.join(vs_path, 'VC', 'bin')]
    path64 = [os.path.join(vs_path, 'VC', 'bin', 'x86_amd64')]

  env_dict32 = os.environ.copy()
  env_dict64 = os.environ.copy()

  env_dict32['PATH'] = ';'.join(env_dict32['PATH'].split(';') + path32)
  env_dict64['PATH'] = ';'.join(env_dict64['PATH'].split(';') + path64)

  WriteWindowEnvFile(env_dict32, ENVIRON32_FILE)
  WriteWindowEnvFile(env_dict64, ENVIRON64_FILE)
  return 0

if __name__ == "__main__":
  try:
    sys.exit(DoMain(sys.argv[1:]))
  except Exception as e:
    print str(e)
    print 1
