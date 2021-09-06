#ifndef VARIABLE_H
#define VARIABLE_H

#include "../utils/types.h"

#include "primitive.h"
#include "buffer_array.h"
#include "array.h"
#include "object.h"

namespace binom {

class Variable {

  union types {
    void* ptr;
    VarType* type;
    byte* bytes;

    Primitive primitive;
    BufferArray buffer_array;
    Array array;
    Object object;

    types(void* ptr) : ptr(ptr) {}
    ~types() {}
  } data;

  void* clone() const;
  void destroy();

  inline void throwIfNot(VarTypeClass type_class) const {
    if(type_class != typeClass())
      throw Exception(ErrCode::binom_invalid_type, "Bad Variable-cast!");
  }

  friend class Primitive;
  friend class BufferArray;
  friend class Array;
  friend class Object;

  friend struct NamedVariable;
  friend class NodeVisitor;

public:

  Variable() : data(nullptr) {}
  Variable(decltype(nullptr)) : data(nullptr) {}

  Variable(VarType type);

  // Bool init
  Variable(bool value);

  // Primitive init

  Variable(ui8 value);
  Variable(ui16 value);
  Variable(ui32 value);
  Variable(ui64 value);

  Variable(i8 value);
  Variable(i16 value);
  Variable(i32 value);
  Variable(i64 value);
  Variable(f32 value);
  Variable(f64 value);

  // Buffer_array init
  Variable(const char* str);
  Variable(const std::string str);

  Variable(size_t size, ui8 value);
  Variable(size_t size, ui16 value);
  Variable(size_t size, ui32 value);
  Variable(size_t size, ui64 value);
  Variable(size_t size, i8 value);
  Variable(size_t size, i16 value);
  Variable(size_t size, i32 value);
  Variable(size_t size, i64 value);

  Variable(ui8* values, size_t size);
  Variable(ui16* values, size_t size);
  Variable(ui32* values, size_t size);
  Variable(ui64* values, size_t size);

  Variable(i8* values, size_t size);
  Variable(i16* values, size_t size);
  Variable(i32* values, size_t size);
  Variable(i64* values, size_t size);

  Variable(ui8arr array);
  Variable(ui16arr array);
  Variable(ui32arr array);
  Variable(ui64arr array);

  Variable(i8arr array);
  Variable(i16arr array);
  Variable(i32arr array);
  Variable(i64arr array);

  // Array
  Variable(varr array);

  // Object
  Variable(vobj object);

  Variable(Primitive primitive);
  Variable(BufferArray buffer_array);
  Variable(Array array);
  Variable(Object object);

  // By value reference
  Variable(ValueRef ref);

  Variable(Variable&& other);
  Variable(const Variable& other);

  ~Variable() {destroy();}

  static Variable create(VarType type);

  ByteArray serialize() const;
  static Variable deserialize(ByteArray::iterator& it);
  static inline Variable deserialize(ByteArray serialized) {ByteArray::iterator it = serialized.begin(); return deserialize(it);}

  inline void* getDataPointer() const {return data.ptr;}

  inline VarType type() const noexcept             {return (data.type == nullptr)? VarType::invalid_type :*data.type;}
  inline VarTypeClass typeClass() const noexcept   {return (data.type == nullptr)? VarTypeClass::invalid_type :toTypeClass(*data.type);}
  inline bool isNull() const noexcept              {return data.type == nullptr;}

  // Type class checks
  inline bool isPrimitive() const noexcept         {return typeClass() == VarTypeClass::primitive;}
  inline bool isBufferArray() const noexcept       {return typeClass() == VarTypeClass::buffer_array;}
  inline bool isArray() const noexcept             {return typeClass() == VarTypeClass::array;}
  inline bool isObject() const noexcept            {return typeClass() == VarTypeClass::object;}
  inline bool isByte() const noexcept              {return (isPrimitive() || isBufferArray())? toValueType(type()) == ValType::byte : false;}
  inline bool isWord() const noexcept              {return (isPrimitive() || isBufferArray())? toValueType(type()) == ValType::word : false;}
  inline bool isDword() const noexcept             {return (isPrimitive() || isBufferArray())? toValueType(type()) == ValType::dword : false;}
  inline bool isQword() const noexcept             {return (isPrimitive() || isBufferArray())? toValueType(type()) == ValType::qword : false;}

  // Type casts
  inline Primitive& toPrimitive() const         {throwIfNot(VarTypeClass::primitive); return const_cast<Primitive&>(data.primitive);}
  inline BufferArray& toBufferArray() const     {throwIfNot(VarTypeClass::buffer_array); return const_cast<BufferArray&>(data.buffer_array);}
  inline Array& toArray() const                 {throwIfNot(VarTypeClass::array); return const_cast<Array&>(data.array);}
  inline Object& toObject() const               {throwIfNot(VarTypeClass::object); return const_cast<Object&>(data.object);}

  // Member access
  inline Variable& getVariable(ui64 index) const         {throwIfNot(VarTypeClass::array); return toArray().getVariable(index);}
  inline Variable& getVariable(BufferArray name) const   {throwIfNot(VarTypeClass::object); return toObject().getVariable(name);}
  inline ValueRef getValue() const                       {throwIfNot(VarTypeClass::primitive); return toPrimitive().getValue();}
  inline ValueRef getValue(ui64 index) const             {throwIfNot(VarTypeClass::buffer_array); return toBufferArray().getValue(index);}

  inline Variable& operator[](ui64 index) const          {return getVariable(index);}
  inline Variable& operator[](BufferArray name) const    {return getVariable(std::move(name));}

  Variable& operator=(Variable other);

  inline ui64 length() {
    return isNull()
        ? 0
        : (isBufferArray() || isArray() || isObject())
        ? *reinterpret_cast<ui64*>(data.bytes + 1)
        : 1;
  }

  bool operator==(Variable other) const;
  bool operator!=(Variable other) const;
  bool operator<(Variable other) const;
  bool operator<=(Variable other) const;
  bool operator>(Variable other) const;
  bool operator>=(Variable other) const;

  i8 getCompare(Variable other) const;

  inline bool contains(BufferArray name) const { if(!isObject()) return false; else return toObject().contains(std::move(name)); }
  inline bool inRange(ui64 index) const {
      switch (typeClass()) {
          case VarTypeClass::array: return toArray().inRange(index);
          case VarTypeClass::buffer_array: return toBufferArray().inRange(index);
          default: return false;
      }
  }

  NodeVisitor getNode();
  operator NodeVisitor();

};








// Initers or other

struct NamedVariable {
  BufferArray name;
  Variable variable;

  NamedVariable& operator=(NamedVariable& other) {
    name.data.ptr = other.name.clone();
    variable.data.ptr = other.variable.clone();
    return *this;
  }
};

struct OutputManip {

  static inline bool print_type = false;

  static inline enum class Primitive : ui8 {
    HEX,
    SIGNED,
    UNSIGNED
  } primitive = Primitive::HEX;

  static inline enum class BufferArray : ui8 {
    PRIMITIVE,
    STRING
  } buffer_array = BufferArray::STRING;

  OutputManip() = delete;
  OutputManip(const OutputManip&) = delete;
  OutputManip(OutputManip&&) = delete;
};

}

std::ostream& operator<<(std::ostream& os, const binom::ValueRef val);
std::ostream& operator<<(std::ostream& os, binom::Primitive& primitive);
std::ostream& operator<<(std::ostream& os, const binom::BufferArray& buffer);
std::ostream& operator<<(std::ostream& os, const binom::Array& array);
std::ostream& operator<<(std::ostream& os, const binom::Object& object);
std::ostream& operator<<(std::ostream& os, const binom::Variable& var);

#endif
