#!/usr/bin/python

import os
import subprocess
import sys
import traceback

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
CONFIGS = ['Debug', 'Release', 'Gold', 'Platinum']

def do_main(argv):
  for config in CONFIGS:
    print 'GN: Generating Ninja Files (%s)...' % config
    sys.stdout.flush()
    subprocess.check_call(['gn',
                           'gen',
                           '//out/%s' % config,
                           '--args=config="%s"' % config.lower()],
                          shell=True,
                          cwd=SCRIPT_DIR)
  return 0

if __name__ == "__main__":
  try:
    sys.exit(do_main(sys.argv[1:]))
  except Exception as e:
    traceback.print_exc()
    sys.exit(1)
