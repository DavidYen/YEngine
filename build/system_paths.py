#!/usr/bin/python

import argparse
import json
import os
import sys
import traceback

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
DEFAULT_CONFIG_FILE = os.path.join(SCRIPT_DIR, 'SystemPaths.json')
DEFAULT_CONFIG_FILENAME = os.path.basename(DEFAULT_CONFIG_FILE)

def GetConfigDict(args):
  config_json = {}
  if os.path.isfile(args.config_file):
    try:
      with open(args.config_file, 'rt') as f:
        config_json = json.load(f)
    except Exception as e:
      print 'Could not load file:', args.config_file
      raise e

  return config_json

def SaveConfigDict(args, config_dict):
  with open(args.config_file, 'wt') as f:
    json.dump(config_dict, f, sort_keys=True, indent=2)

def do_config(args):
  config_dict = GetConfigDict(args)
  config_dict[args.name] = args.path

  SaveConfigDict(args, config_dict)
  return 0

def do_list(args):
  config_json = GetConfigDict(args)
  if not config_json:
    print 'No system paths found'
    return 1

  for name, path in config_json.iter_items():
    print '%s: %s' % (name, path)

  return 0

def do_output(args):
  name = args.output_name
  config_dict = GetConfigDict(args)
  if name not in config_dict:
    print 'System Path is not set:', name
    print 'Configure Usage: %s config %s PATH' % (sys.argv[0], name)
    return 1

  print config_dict[name]
  return 0

def do_main(argv):
  parser = argparse.ArgumentParser(prog=sys.argv[0])

  command_parser = parser.add_subparsers(title='command', dest='command')
  parser.add_argument(
      '--config-file', dest='config_file', action='store',
      default=DEFAULT_CONFIG_FILE,
      help='Configuration file. (default: %s)' % DEFAULT_CONFIG_FILENAME)

  # Configure Command
  config_parser = command_parser.add_parser('config')
  config_parser.add_argument(
      'name',
      help='System Name: IE. VS20XX, SDK, VS20XXe...etc.')
  config_parser.add_argument(
      'path',
      help='Path for the System path')

  # List Command
  config_parser = command_parser.add_parser('list')

  # Output System Command
  output_parser = command_parser.add_parser('output')
  output_parser.add_argument(
      '--name', dest='output_name', action='store',
      help='Output the path for a given system name.')

  args = parser.parse_args(argv)

  if (args.command == 'config'):
    return do_config(args)
  elif (args.command == 'list'):
    return do_list(args)
  elif (args.command == 'output'):
    return do_output(args)

  print 'Invalid Command:', args.command
  return 1

if __name__ == "__main__":
  try:
    sys.exit(do_main(sys.argv[1:]))
  except Exception as e:
    traceback.print_exc()
    sys.exit(1)
