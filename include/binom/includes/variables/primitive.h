#ifndef PRIMITIVE_H
#define PRIMITIVE_H

#include "../utils/types.h"
#include "value.h"

namespace binom {

class Primitive {

  union types {
      void* ptr;
      VarType* type;
      byte* bytes;
      types(void* ptr) : ptr(ptr) {}
  } data;

  inline ui64 msize() const {
    switch (*data.type) {
      case VarType::byte: return 2;
      case VarType::word: return 3;
      case VarType::dword: return 5;
      case VarType::qword: return 9;
      default: throw Exception(ErrCode::binom_invalid_type);
    }
  }

  void destroy();
  void* clone() const;

  friend class Variable;
  friend class ValueRef;

  Primitive(void* buffer) : data(buffer) {}

public:
  Primitive(VarType type);
  Primitive() : data(nullptr) {}
  // Bool init
  Primitive(bool value);

  // Primitive init
  Primitive(ui8 value);
  Primitive(ui16 value);
  Primitive(ui32 value);
  Primitive(ui64 value);

  Primitive(i8 value);
  Primitive(i16 value);
  Primitive(i32 value);
  Primitive(i64 value);
  Primitive(f32 value);
  Primitive(f64 value);

  Primitive(ValType type, void* value_ptr);

  Primitive(const Primitive& other);
  Primitive(Primitive&& other);
  Primitive(ValueRef value);

  ~Primitive();

  VarType getType() const {return *data.type;}
  ValType getValType() const {return toValueType(*data.type);}

  ByteArray serialize() const;
  static Primitive deserialize(ByteArray::iterator& it);

  inline ui8 getMemberSize() const {
    switch (*data.type) {
      case VarType::byte: return 1;
      case VarType::word: return 2;
      case VarType::dword: return 4;
      case VarType::qword: return 8;
      default: throw Exception(ErrCode::binom_invalid_type);
    }
  }

  static inline ui8 getMemberSize(VarType type) {
    switch (type) {
      case VarType::byte: return 1;
      case VarType::word: return 2;
      case VarType::dword: return 4;
      case VarType::qword: return 8;
      default: throw Exception(ErrCode::binom_invalid_type);
    }
  }


  inline ValueRef getValue() const {return ValueRef(*data.type, data.bytes + 1);}

  inline ValueRef operator*() const {return getValue();}

  inline bool operator=(const bool value) {return (**this).setBool(value);}
  inline ui64 operator=(const ui64 value) {return (**this).setUnsigned(value);}
  inline i64 operator=(const i64 value) {return (**this).setSigned(value);}
  inline f64 operator=(const f64 value) {return (**this).setFloat(value);}

  Primitive& operator=(Primitive other);

  bool operator==(Primitive other) const;
  bool operator!=(Primitive other) const;
  bool operator>(Primitive other) const;
  bool operator>=(Primitive other) const;
  bool operator<(Primitive other) const;
  bool operator<=(Primitive other) const;

  i8 getCompare(Primitive other) const;

  Variable& asVar() {return *reinterpret_cast<Variable*>(this);}

  BufferArray toBufferArray();

  void* getDataPtr() {return data.bytes + 1;}
};

}

#endif
