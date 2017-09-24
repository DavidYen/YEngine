import sys

OS_NAMES = [ "Win" ]

OS_WIN, = OS_NAMES

OS_MAP = { OS_WIN : [ 'win32' ] }

OS_LOOKUP = dict([(plat_name, os_name) for os_name, plat_names in
                  OS_MAP.iteritems() for plat_name in plat_names])

def GetOSName(platform=None):
  "Get Standard OSName as used in Yengine"
  if platform is None:
    platform = sys.platform

  os_name = OS_LOOKUP.get(platform, None)
  assert os_name, "Error - invalid platform name: %s" % platform

  return os_name

