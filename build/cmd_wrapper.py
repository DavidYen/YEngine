#!/usr/bin/python

import argparse
import os
import subprocess
import sys

if __name__ == "__main__":
  parser = argparse.ArgumentParser()
  parser.add_argument(
    "--cwd", default=None,
    help="Working directory.")
  parser.add_argument(
    "--env", default=[], action='append',
    help="Environment variable A=B")
  parser.add_argument(
    "--capture", default=None,
    help="Capture output into a file.")
  parser.add_argument(
    "--filter-capture", default=[], action='append',
    help="Filter captured output by replacing A:B")
  parser.add_argument(
    "app_args", nargs="+",
    help="Application arguments.")

  args = parser.parse_args()

  if len(sys.argv) == 1:
    print 'Usage: %s cmd cmdarg1 cmdarg2...' % sys.argv[0]
    sys.exit(1)

  if args.cwd and not os.path.isdir(args.cwd):
    os.makedirs(options.cwd)

  env = None
  if args.env:
    env = dict(value.split("=") for value in args.env)

  if args.capture:
    output = subprocess.check_output(args.app_args, cwd=args.cwd, env=env)
    with open(args.capture, "wb") as f:
      for filter_string in args.filter_capture:
        orig, replace = filter_string.split("=")
        output = output.replace(orig, replace)
      f.write(output)
  else:
    subprocess.check_call(args.app_args, cwd=args.cwd, env=env)
  sys.exit(0)
