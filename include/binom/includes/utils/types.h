#ifndef BINOM_TYPES_H
#define BINOM_TYPES_H

#include "malloc.h"
#include <initializer_list>
#include <cstring>
#include <string>
#include "ctypes.h"
#include "mem.h"
#include "byte_array.h"


//Arch dependebce
#if __SIZEOF_POINTER__ == 2
#define PTR_SZ 2
#elif __SIZEOF_POINTER__ == 4
#define PTR_SZ 4
#elif __SIZEOF_POINTER__ == 8
#define PTR_SZ 8
#else
#error "Unsupported architecture!"
#endif


namespace binom {

  //! Variable type
  enum class VarType : byte {
    end                 = 0x00,
    byte                = 0x01,
    word                = 0x02,
    dword               = 0x03,
    qword               = 0x04,
    byte_array          = 0x05,
    word_array          = 0x06,
    dword_array         = 0x07,
    qword_array         = 0x08,
    array               = 0x09,
    object              = 0x0A,

    invalid_type         = 0xFF
  };

  //! Primitive value type
  enum class ValType : byte {
      byte                = 0x00,
      word                = 0x01,
      dword               = 0x02,
      qword               = 0x03,

      invalid_type         = 0xFF
  };

  //! Variable type class
  enum class VarTypeClass : byte {
    primitive           = 0x01,
    buffer_array        = 0x02,
    array               = 0x03,
    object              = 0x04,

    invalid_type        = 0xFF
  };

  //! Cast VarType => VarTypeClass
  inline VarTypeClass toTypeClass(VarType type) {
    switch(type) {
      case VarType::byte: case VarType::word: case VarType::dword: case VarType::qword:
        return VarTypeClass::primitive;
      case VarType::byte_array: case VarType::word_array: case VarType::dword_array: case VarType::qword_array:
        return VarTypeClass::buffer_array;
      case VarType::array:
        return VarTypeClass::array;
      case VarType::object:
        return VarTypeClass::object;
      default:
        return VarTypeClass::invalid_type;
    }
  }

  //! Cast VarType => ValType
  inline ValType toValueType(VarType type) {
      switch (type) {
          case VarType::byte: return ValType::byte;
          case VarType::word: return ValType::word;
          case VarType::dword: return ValType::dword;
          case VarType::qword: return ValType::qword;
          case VarType::byte_array: return ValType::byte;
          case VarType::word_array: return ValType::word;
          case VarType::dword_array: return ValType::dword;
          case VarType::qword_array: return ValType::qword;
        default: return ValType::invalid_type;
      }
  }

  //! Cast ValType => VarType
  inline VarType toVarType(ValType type) {
    switch (type) {
      case ValType::byte: return VarType::byte;
      case ValType::word: return VarType::word;
      case ValType::dword:return VarType::dword;
      case ValType::qword:return VarType::qword;
      default: return VarType::invalid_type;
    }
  }

  //! Cast ValType => VarType
  inline VarType toVarBufferType(ValType type) {
    switch (type) {
      case ValType::byte: return VarType::byte_array;
      case ValType::word: return VarType::word_array;
      case ValType::dword:return VarType::dword_array;
      case ValType::qword:return VarType::qword_array;
      default: return VarType::invalid_type;
    }
  }

  //! Cast ValType => Size of element
  constexpr ui8 toSize(ValType type) {
    switch (type) {
      case ValType::byte: return 1;
      case ValType::word: return 2;
      case ValType::dword: return 4;
      case ValType::qword: return 8;
      default: return 0;
    }
  }

  //! Cast VarType => Name of type
  inline const char* toTypeString(VarType type) {
    switch (type) {
    case binom::VarType::byte:  return "byte";
    case binom::VarType::word: return "word";
    case binom::VarType::dword: return "dword";
    case binom::VarType::qword:  return "qword";
    case binom::VarType::byte_array: return "byte_array";
    case binom::VarType::word_array: return "word_array";
    case binom::VarType::dword_array: return "dword_array";
    case binom::VarType::qword_array: return "qword_array";
    case binom::VarType::array: return "array";
    case binom::VarType::object:  return "object";
    case binom::VarType::end:
    default: return "invalid_type";
    }
  }

  //! Cast VarTypeClass => Name of type class
  inline const char* toTypeString(VarTypeClass type_class) {
    switch (type_class) {
    case binom::VarTypeClass::primitive: return "primitive";
    case binom::VarTypeClass::buffer_array: return "buffer_array";
    case binom::VarTypeClass::array: return "array";
    case binom::VarTypeClass::object: return "object";
    default:
    case binom::VarTypeClass::invalid_type: return "invalid_type";
    }
  }

  //! Cast VarTypeClass => Name of value type
  inline const char* toTypeString(ValType val_type) {
    switch (val_type) {
    case binom::ValType::byte: return "byte";
    case binom::ValType::word: return "word";
    case binom::ValType::dword: return "dword";
    case binom::ValType::qword: return "qword";
    default: return "invalid_type";
    }
  }

  inline bool isstreq(const char* s1, const char* s2) {
    while(true)
      if(*s1 == *s2) {
        if(!*s1) return true;
        ++s1;
        ++s2;
        continue;
      } else return false;
  }

  inline ValType toValueType(const char* str) {
    if(isstreq(str, "byte"))
      return ValType::byte;
    elif(isstreq(str, "word"))
        return ValType::word;
    elif(isstreq(str, "dword"))
        return ValType::dword;
    elif(isstreq(str, "qword"))
        return ValType::qword;
    else return ValType::invalid_type;
  }

  inline VarTypeClass toVarTypeClass(const char* str) {
    if(isstreq(str, "primitive"))
      return VarTypeClass::primitive;
    elif(isstreq(str, "buffer_array"))
        return VarTypeClass::buffer_array;
    elif(isstreq(str, "array"))
        return VarTypeClass::array;
    elif(isstreq(str, "object"))
        return VarTypeClass::object;
    else return VarTypeClass::invalid_type;
  }

  inline VarType toVarType(const char* str) {
    if(isstreq(str, "byte"))
      return VarType::byte;
    elif(isstreq(str, "word"))
        return VarType::word;
    elif(isstreq(str, "dword"))
        return VarType::dword;
    elif(isstreq(str, "qword"))
        return VarType::qword;
    elif(isstreq(str, "byte_array"))
        return VarType::byte_array;
    elif(isstreq(str, "word_array"))
        return VarType::word_array;
    elif(isstreq(str, "dword_array"))
        return VarType::dword_array;
    elif(isstreq(str, "qword_array"))
        return VarType::qword_array;
    elif(isstreq(str, "array"))
        return VarType::array;
    elif(isstreq(str, "object"))
        return VarType::object;
    else return VarType::invalid_type;
  }

  //! Type class generic (may have any BinOM type)
  class Variable;

  // Basic type class
  //! BinOM primitive data type class (may have type: byte/word/dword/qword)
  class Primitive;
  //! BinOM buffer array data type class (may have type: byte_array/word_array/dword_array/qword_array)
  class BufferArray;
  //! BinOM array data type
  class Array;
  //! BinOM object data type
  class Object;

  // Helpers
  //! Reference to generic value in primitver or buffer array (may have value type: byte/word/dword/qword)
  class ValueRef;
  //! Pair of name(BufferArray) and Variable stored in Object
  struct NamedVariable;

  // Iterators
  //! BufferArray iterator
  class ValueIterator;
  //! Array iterator
  typedef Variable* ArrayIterator;
  //! Object iterator
  typedef NamedVariable* ObjectIterator;

  //! Node Visitor Base
  class NodeVisitorBase;

  // Node Visitors
  //! RAM Node Visitor
  class NodeVisitor;
  //! File Node Visitor
  class FileNodeVisitor;

  // Initilizers
  typedef std::initializer_list<const ui8> ui8arr;
  typedef std::initializer_list<const i8> i8arr;
  typedef std::initializer_list<const ui16> ui16arr;
  typedef std::initializer_list<const i16> i16arr;
  typedef std::initializer_list<const ui32> ui32arr;
  typedef std::initializer_list<const i32> i32arr;
  typedef std::initializer_list<const ui64> ui64arr;
  typedef std::initializer_list<const i64> i64arr;
  typedef std::initializer_list<const Variable> varr;
  typedef std::initializer_list<const NamedVariable> vobj;
}

#endif
