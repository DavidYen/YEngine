#!/usr/bin/python

import optparse
import os
import shutil
import sys

if __name__ == "__main__":
  if len(sys.argv) != 3:
    print >> sys.stderr, 'Usage: %s src_file dst_file' % sys.argv[0]
    sys.exit(1)

  source_file, dest_file = sys.argv[1:]
  if not os.path.isfile(source_file):
    print >> sys.stderr, 'Invalid source file: %s' % sys.argv[1]
    sys.exit(1)

  dest_dir = os.path.dirname(dest_file)
  if dest_dir and not os.path.isdir(dest_dir):
    os.makedirs(dest_dir)

  if os.path.isfile(dest_file):
    os.unlink(dest_file)

  shutil.copyfile(source_file, dest_file)
  sys.exit(0)
