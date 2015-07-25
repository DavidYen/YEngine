#include <YCommon/Headers/stdincludes.h>
#include "RefPointer.h"

#include <YCommon/Headers/Macros.h>

namespace YCommon { namespace YContainers {

ReadRefData RefPointer::GetReadRef() {
  return ReadRefData(this);
}

WriteRefData RefPointer::GetWriteRef() {
  return WriteRefData(this);
}

ReadWriteRefData RefPointer::GetReadWriteRef() {
  return ReadWriteRefData(this);
}

}} // namespace YCommon { namespace YContainers {
