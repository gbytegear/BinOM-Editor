#ifndef VAR_MEM_MNGR_H
#define VAR_MEM_MNGR_H

#include "types.h"

namespace binom {

template<class T>
class VarMemoryManager {
  friend T;
  friend class Variable;
  friend struct NamedVariable;

  size_t msize() const {return static_cast<const T*>(this)->msizeImpl();}
  byte*& ptr() {return static_cast<T*>(this)->ptrImpl();}
  void* clone() const {return static_cast<const T*>(this)->cloneImpl();}
  void destroy() {static_cast<T*>(this)->destroyImpl();}

  void mch(size_t new_size) { ptr() = tryRealloc(ptr(), new_size); }

  void* madd(size_t add_size) {
    size_t shift = msize();
    mch(shift + add_size);
    return ptr() + shift;
  }

  void msub(size_t sub_size) {
    mch(msize() - sub_size);
  }

  void* maddto(void* to, size_t size) {
    size_t pos = reinterpret_cast<byte*>(to) - ptr();
    size_t old_size = msize();
    madd(size);
    memmove(ptr() + pos + size, ptr() + pos, old_size - pos);
    return ptr() + pos;
  }

  void msubfrom(void* from, size_t size) {
    if(from < ptr()) throw Exception(ErrCode::binom_out_of_range);
    size_t old_size = msize();
    memmove(from, reinterpret_cast<byte*>(from) + size,
            old_size - (reinterpret_cast<byte*>(from) - ptr()) - size);
    mch(old_size - size);
  }
};

}

#endif // VAR_MEM_MNGR_H
