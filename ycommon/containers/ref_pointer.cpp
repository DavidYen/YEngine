#include "ycommon/containers/ref_pointer.h"

#include "ycommon/headers/macros.h"

namespace ycommon { namespace containers {

ReadRefData RefPointer::GetReadRef() {
  return ReadRefData(this);
}

WriteRefData RefPointer::GetWriteRef() {
  return WriteRefData(this);
}

ReadWriteRefData RefPointer::GetReadWriteRef() {
  return ReadWriteRefData(this);
}

}} // namespace ycommon { namespace containers {
