#!/usr/bin/python

import os
import sys

import subprocess
import traceback

SCRIPT_DIR = os.path.dirname(__file__)

ENVIRON32_FILE = 'environment.x86'
ENVIRON64_FILE = 'environment.x64'

VC_VARS_SCRIPTS = [
  "vcvarsall.bat",
  os.path.join("VC", "vcvarsall.bat"),
  os.path.join("Community", "Common7", "Tools", "VsDevCmd.bat"),
]

def GetEnvDictFromSet(set_output):
  env_dict = {}
  for line in set_output.splitlines():
    line = line.strip()
    if line and "=" in line:
      key, value = line.split('=', 1)
      env_dict[key] = value

  return env_dict

def GetVCVarsEnv(vc_vars_path):
  """Returns both the 32 and 64 bit environment dictionary as a tuple."""
  env32 = subprocess.check_output([vc_vars_path, '&&', 'set'],
                                  shell=True)
  env32_dict = GetEnvDictFromSet(env32)

  env64 = subprocess.check_output([vc_vars_path, '&&', 'set'],
                                  shell=True)
  env64_dict = GetEnvDictFromSet(env64)

  return (env32_dict, env64_dict)

def WriteWindowEnvFile(env_dict, env_file):
  with open(env_file, 'wb') as f:
    for key, value in env_dict.iteritems():
      f.write(key + '=' + value + '\0')
    f.write('\0')
  with open(env_file + '.txt', 'wt') as f:
    for key, value in env_dict.iteritems():
      f.write(key + '=' + value + '\n')

def DoMain(argv):
  if len(argv) != 1 or '--h' in argv or '--help' in argv or '\?' in argv:
    print 'Usage: %s vs_path' % sys.argv[0]
    return 1

  vs_path, = argv
  for vc_vars_script in VC_VARS_SCRIPTS:
    script_file = os.path.join(vs_path, vc_vars_script)
    if os.path.isfile(script_file):
      vc_vars_path = script_file
      break
  else:
    assert False, "Could not determine VC Vars script path."
    vc_vars_path = os.path.join(vs_path, 'VC', 'vcvarsall.bat')

  env_dict32, env_dict64 = GetVCVarsEnv(vc_vars_path)

  WriteWindowEnvFile(env_dict32, ENVIRON32_FILE)
  WriteWindowEnvFile(env_dict64, ENVIRON64_FILE)
  return 0

if __name__ == "__main__":
  try:
    sys.exit(DoMain(sys.argv[1:]))
  except Exception as e:
    traceback.print_exc()
    sys.exit(1)
