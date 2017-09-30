#!/usr/bin/env python

import argparse
import os
import subprocess
import sys

if __name__ == "__main__":
  parser = argparse.ArgumentParser()
  parser.add_argument(
    "--depfile",
    help="Where flatbuffer depfile should be written to.")
  parser.add_argument(
    "flatc_args", nargs="+",
    help="Application arguments.")

  args = parser.parse_args()

  subprocess.check_call(args.flatc_args)
  if args.depfile:
    deps_output = subprocess.check_output([args.flatc_args[0], "-M"] +
                                          args.flatc_args[1:])
    dep_dir = os.path.dirname(args.depfile)
    if not os.path.isdir(dep_dir):
      os.makedirs(dep_dir)
    with open(args.depfile, "wb") as f:
      f.write(deps_output)
  sys.exit(0)
