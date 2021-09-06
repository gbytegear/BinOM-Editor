#ifndef ARRAY_H
#define ARRAY_H

#include "../utils/types.h"
#include "../utils/var_mem_mngr.h"

namespace binom {

class Array : VarMemoryManager<Array> {
  friend class VarMemoryManager<Array>;
  friend class Variable;

  union types {
      void* ptr;
      VarType* type;
      byte* bytes;
      types(void* ptr) : ptr(ptr) {}
  } data;

  inline ui64& length() const {return *reinterpret_cast<ui64*>(data.bytes + 1);}

  inline byte*& ptrImpl() {return data.bytes;}
  inline ui64 msizeImpl() const {return 9 + length() * PTR_SZ;}
  void* cloneImpl() const;
  void destroyImpl();

public:
  Array();
  Array(varr array);
  Array(const Array& other);
  Array(Array&& other);
  ~Array() {destroy ();}

  ByteArray serialize() const;
  static Array deserialize(ByteArray::iterator& it);

  inline bool isEmpty() const {return !length();}
  inline ui64 getMemberCount() const {return *reinterpret_cast<ui64*>(data.bytes + 1);}
  inline bool inRange(ui64 index) const {return length() > index;}

  Variable& getVariable(ui64 index) const;

  Variable& insert(ui64 index, Variable var);
  Variable& pushBack(Variable var);
  Variable& pushFront(Variable var);

  void remove(ui64 index, ui64 n = 1);
  void popBack(ui64 n = 1);
  void popFront(ui64 n = 1);

  void clear();

  inline Variable& operator[](ui64 index) const {return getVariable(index);}
  Array& operator+=(Variable var);
  Array& operator=(Array other);

  bool operator==(Array other) const;
  bool operator!=(Array other) const;
  bool operator<(Array other) const;
  bool operator<=(Array other) const;
  bool operator>(Array other) const;
  bool operator>=(Array other) const;

  ui8 getCompare(Array other) const;

  ArrayIterator begin() const {return reinterpret_cast<ArrayIterator>(data.bytes + 9);}
  ArrayIterator end() const {return reinterpret_cast<ArrayIterator>(data.bytes + msize());}

  Variable& asVar() {return *reinterpret_cast<Variable*>(this);}
};
}


#endif
