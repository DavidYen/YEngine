
import os

def NormalizedJoin( *args ):
  "Normalizes and joins directory names"
  return os.path.normpath(os.path.join(*args))
