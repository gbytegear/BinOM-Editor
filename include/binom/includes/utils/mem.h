#ifndef TRYMEM_H
#define TRYMEM_H

#include "exception.h"
#include <malloc.h>

namespace binom {


void tryFree(void* ptr);

template <typename Type>
Type* tryMalloc() {
  Type* ptr = reinterpret_cast<Type*>(malloc(sizeof(Type)));
  if(ptr == nullptr) throw binom::Exception(binom::ErrCode::memory_allocation_error, "Memory type allocation error!");
  else return ptr;
}

void* tryMalloc(size_t size);

template <typename Type>
Type* tryMalloc(size_t count) {
  if(!count) return nullptr;
  Type* ptr = reinterpret_cast<Type*>(malloc(sizeof (Type) * count));
  if(ptr == nullptr) throw binom::Exception(binom::ErrCode::memory_allocation_error, "Memory array allocation error!");
  else return ptr;
}

void* tryRealloc(void* ptr, size_t size);

template <typename Type>
Type* tryRealloc(Type* ptr, size_t count) {
  if(!count) {
    tryFree(ptr);
    return nullptr;
  }
  Type* new_ptr = reinterpret_cast<Type*>(realloc(ptr, sizeof (Type) * count));
  if(new_ptr == nullptr) throw binom::Exception(binom::ErrCode::memory_allocation_error, "Memory array allocation error!");
  else return new_ptr;
}

}

#endif // TRYMEM_H
