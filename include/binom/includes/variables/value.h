#ifndef VALUEPTR_H
#define VALUEPTR_H

#include "../utils/types.h"
#include <iostream>
#include <iomanip>

namespace binom {

class ValueRef {
  ValType type;
  union pointer {
    void* ptr;
    bool* boolptr;
    ui8* ui8ptr;
    ui16* ui16ptr;
    ui32* ui32ptr;
    ui64* ui64ptr;
    i8* i8ptr;
    i16* i16ptr;
    i32* i32ptr;
    i64* i64ptr;
    f32* f32ptr;
    f64* f64ptr;
    pointer(void* ptr) : ptr(ptr) {}
  } ptr;

  friend class ValueIterator;
public:
  ValueRef(Primitive& var)              : type(toValueType(**reinterpret_cast<VarType**>(&var))), ptr(*reinterpret_cast<byte**>(&var) + 1) {}
  ValueRef(ValType type, void* pointer) : type(type), ptr(pointer) {}
  ValueRef(VarType type, void* pointer) : type(toValueType(type)), ptr(pointer) {}
  ValueRef(const ValueRef& other) : type(other.type), ptr(other.ptr.ptr) {}
  ValueRef(const ValueRef&& other) : type(other.type), ptr(other.ptr.ptr) {}
  ValueRef(const ValueIterator& it);
  ValueRef(const ValueIterator&& it);

  ValType getType() const {return type;}
  inline ui8 getSize() const {return toSize(type);}

  bool asBool() const;
  ui64 asUnsigned() const;
  i64 asSigned() const;
  f64 asFloat() const;

  bool setBool(const bool value);
  ui64 setUnsigned(const ui64 value);
  i64 setSigned(const i64 value);
  f64 setFloat(const f64 value);

  inline ui8 asUi8() const {return asUnsigned();}
  inline ui16 asUi16() const {return asUnsigned();}
  inline ui32 asUi32() const {return asUnsigned();}
  inline ui64 asUi64() const {return asUnsigned();}

  inline i8 asI8() const {return asSigned();}
  inline i16 asI16() const {return asSigned();}
  inline i32 asI32() const {return asSigned();}
  inline i64 asI64() const {return asSigned();}

  inline f32 asF32() const {return asFloat();}
  inline f64 asF64() const {return asFloat();}

  inline operator ui8 () {return asUi8();}
  inline operator ui16 () {return asUi16();}
  inline operator ui32 () {return asUi32();}
  inline operator ui64 () {return asUi64();}

  inline operator i8 () {return asI8();}
  inline operator i16 () {return asI16();}
  inline operator i32 () {return asI32();}
  inline operator i64 () {return asI64();}

  inline operator f32 () {return asF32();}
  inline operator f64 () {return asF64();}

  inline ValueIterator& toIterator();

  inline bool operator=(const bool value) {return setBool(value);}
  inline ui64 operator=(const ui64 value) {return setUnsigned(value);}
  inline i64 operator=(const i64 value) {return setSigned(value);}
  inline f64 operator=(const f64 value) {return setFloat(value);}
  ValueRef& operator=(const ValueRef& other);
  ValueRef& operator<<(const ValueRef& other); //!< Set value from other to this

  inline bool operator==(ValueRef other) const {return asUi64() == other.asUi64();}
  inline bool operator!=(ValueRef other) const {return asUi64() != other.asUi64();}
  inline bool operator>(ValueRef other) const {return asUi64() > other.asUi64();}
  inline bool operator>=(ValueRef other) const {return asUi64() >= other.asUi64();}
  inline bool operator<(ValueRef other) const {return asUi64() < other.asUi64();}
  inline bool operator<=(ValueRef other) const {return asUi64() <= other.asUi64();}
};


class ValueIterator {
  ValueRef pointer;

  inline ui8 getShift() const {
    switch (pointer.type) {
      case ValType::byte: return 1;
      case ValType::word: return 2;
      case ValType::dword: return 4;
      case ValType::qword: return 8;
      default: throw Exception(ErrCode::binom_invalid_type);
    }
  }

public:
  ValueIterator(ValType type, void* pointer) : pointer(type, pointer) {}
  ValueIterator(VarType type, void* pointer) : pointer(type, pointer) {}
  ValueIterator(ValueRef& val_ptr) : pointer(val_ptr) {}
  ValueIterator(ValueRef&& val_ptr) : pointer(val_ptr) {}
  ValueIterator(ValueIterator& other) : pointer(other.pointer) {}
  ValueIterator(ValueIterator&& other) : pointer(other.pointer) {}

  inline ValueRef& operator*() const {return *const_cast<ValueRef*>(&pointer);}
  inline ValueRef* operator->() const {return const_cast<ValueRef*>(&pointer);}

  ValueRef operator[](i64 index) {
    return ValueRef(pointer.type, pointer.ptr.ui8ptr + getShift() * index);
  }

  ValueIterator& operator++() {
    pointer.ptr.ui8ptr += getShift();
    return *this;
  }

  ValueIterator& operator+=(i64 shift) {
    pointer.ptr.ui8ptr += getShift() * shift;
    return *this;
  }

  ValueIterator& operator--() {
    pointer.ptr.ui8ptr -= getShift();
    return *this;
  }

  ValueIterator& operator-=(i64 shift) {
    pointer.ptr.ui8ptr -= getShift() * shift;
    return *this;
  }

  ValueIterator& operator=(const ValueIterator& other) {
    pointer = other.pointer;
    return *this;
  }

  ValueIterator& operator=(const ValueRef& other) {
    pointer = other;
    return *this;
  }

  inline bool operator==(const ValueIterator& other) const {return pointer.ptr.ptr == other.pointer.ptr.ptr;}
  inline bool operator!=(const ValueIterator& other) const {return pointer.ptr.ptr != other.pointer.ptr.ptr;}
};

}

#endif // VALUEPTR_H
