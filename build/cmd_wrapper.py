#!/usr/bin/python

import optparse
import os
import subprocess
import sys

if __name__ == "__main__":
  parser = optparse.OptionParser()
  parser.add_option(
    "--cwd",
    default=None,
    help="Working directory.")

  options, args = parser.parse_args()

  if len(sys.argv) == 1:
    print 'Usage: %s cmd cmdarg1 cmdarg2...' % sys.argv[0]
    sys.exit(1)

  if options.cwd and not os.path.isdir(options.cwd):
    os.makedirs(options.cwd)

  subprocess.check_call(args, cwd=options.cwd)
  sys.exit(0)
