#!/usr/bin/python

import os
import subprocess
import sys
import traceback

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
CONFIGS = ['Debug', 'Release', 'Gold', 'Platinum']
ARG_ENVVAR = 'GN_DEFINES'

def ParseArgString(arg_string):
  """Parses an arg string of the format "A=B B=C" and returns a dictionary."""
  arg_dict = {}
  for pairs in arg_string.split(' '):
    if pairs:
      name, value = pairs.split('=', 1)
      arg_dict[name] = value

  return arg_dict

def do_main(argv):
  arg_dict = ParseArgString(os.getenv(ARG_ENVVAR, ''))
  for arg in argv:
    name, value = arg.split('=', 1)
    arg_dict[name] = value

  # See if user has chosen a configuration to generate.
  config = arg_dict.get('config', None)
  if config:
    configs = [config]
  else:
    # Generate all of them
    configs = CONFIGS

  # Generate configurations
  for config in configs:
    print 'GN: Generating Ninja Files (%s)...' % config
    sys.stdout.flush()

    arg_dict['config'] = config.lower()
    arg_strings = ['%s="%s"' % (key, value)
                   for key, value in arg_dict.iteritems()]
    subprocess.check_call(['gn',
                           'gen',
                           '//out/%s' % config,
                           '--args=' + ' '.join(arg_strings)],
                          shell=True,
                          cwd=SCRIPT_DIR)
  return 0

if __name__ == "__main__":
  try:
    sys.exit(do_main(sys.argv[1:]))
  except Exception as e:
    traceback.print_exc()
    sys.exit(1)
