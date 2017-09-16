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

  subprocess.check_call(args.app_args, cwd=args.cwd, env=env)
  sys.exit(0)
