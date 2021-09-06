#ifndef BYTE_ARRAY_H
#define BYTE_ARRAY_H

#include "ctypes.h"
#include "mem.h"
#include <cstring>

namespace binom {

class ByteArray {
  ui64 _length = 0;
  byte* array = nullptr;

  friend class SharedByteArray;

public:

  typedef byte* iterator;
  typedef const byte* const_iterator;

  ByteArray() = default;
  ByteArray(const void* buffer, ui64 size);
  ByteArray(const ByteArray& other);
  ByteArray(ByteArray&& other);
  ByteArray(ui64 size);
  ByteArray(std::initializer_list<const ByteArray> arrays);

  ~ByteArray();

  bool isEmpty() const;
  bool isEqual(const ByteArray& other);

  template<typename Type>
  ui64 length() const {return _length/sizeof(Type);}
  ui64 length() const;

  void reset(ui64 new_length);
  void resize(ui64 new_length);
  iterator addSize(ui64 add);
  iterator addSizeFront(ui64 add);
  iterator addSizeTo(ui64 to, ui64 add);
  void subSize(ui64 sub);

  template<typename Type>
  ByteArray& pushBack(const Type& value) {return pushBack(&value, sizeof (Type));}
  ByteArray& pushBack(byte b);
  ByteArray& pushBack(const char* c_str);
  ByteArray& pushBack(const void* buffer, ui64 size);
  ByteArray& pushBack(const ByteArray& byte_array);
  ByteArray& pushBack(const ByteArray&& byte_array);

  template<typename Type>
  ByteArray& pushFront(const Type& value) {return pushFront(&value, sizeof (Type));}
  ByteArray& pushFront(byte b);
  ByteArray& pushFront(const char* c_str);
  ByteArray& pushFront(const void* buffer, ui64 size);
  ByteArray& pushFront(const ByteArray& byte_array);
  ByteArray& pushFront(const ByteArray&& byte_array);

  template<typename Type>
  ByteArray& insert(ui64 index, ui64 shift, Type type) { return insert(index*sizeof (Type) + shift, &type, sizeof(Type)); }
  ByteArray& insert(ui64 index, byte b);
  ByteArray& insert(ui64 index, const void* buffer, ui64 size);
  ByteArray& insert(ui64 index, const ByteArray& byte_array);
  ByteArray& insert(ui64 index, const ByteArray&& byte_array);

  template<typename Type>
  ByteArray& remove(ui64 index, ui64 shift, ui64 count = 1) { return remove(index*sizeof(Type)+shift, count*sizeof(Type)); }
  ByteArray& remove(ui64 index, ui64 size = 1);

  template<typename Type>
  Type& set(ui64 index, ui64 shift, Type value) { return get<Type>(index, shift) = value; }
  iterator set(ui64 index, ByteArray data);
  byte& set(ui64 index, byte value);

  template<typename Type>
  Type& get(ui64 index, ui64 shift = 0) {return reinterpret_cast<Type*>(array + shift)[index];}
  byte& get(ui64 index);

  template<typename Type>
  Type& first() {return *reinterpret_cast<Type*>(array);}
  byte& first();

  template<typename Type>
  Type& last() {return *reinterpret_cast<Type*>(array + _length - sizeof (Type));}
  byte& last();

  template<typename Type>
  Type takeBackAs() {
    void* ptr = takeBack(sizeof(Type)).unfree();
    return *reinterpret_cast<Type*>(ptr);
  }
   template<typename Type>
  ByteArray takeFrontAs(){
    void* ptr = takeFront(sizeof(Type)).unfree();
    return *reinterpret_cast<Type*>(ptr);
  }

  ByteArray takeBack(ui64 size);
  ByteArray takeFront(ui64 size);

  template<typename Type>
  ByteArray takeFrom(ui64 index, ui64 count) {return takeFrom(index * sizeof (Type), count * sizeof (Type));}
  ByteArray takeFrom(ui64 index, ui64 size);

  template<typename Type>
  Type takeBack() {
    if(sizeof (Type) > _length) throw Exception(ErrCode::any);
    Type _new;
    memcpy(&_new, end()-sizeof(Type), sizeof(Type));
    _length -= sizeof(Type);
    return _new;
  }

  template<typename Type>
  Type takeFront() {
    if(sizeof (Type) > _length) throw Exception(ErrCode::any);
    Type _new;
    memcpy(&_new, begin(), sizeof (Type));
    _length -= sizeof (Type);
    memmove(begin(), begin() + sizeof (Type), _length);
    array = tryRealloc<byte>(array, _length);
    return _new;
  }

  ui64 pointerToIndex(void* pos);

  template<typename Type>
  Type* begin(ui64 shift = 0) const {return reinterpret_cast<Type*>(array + shift);}
  iterator begin() const;
  const_iterator cbegin() const;

  template<typename Type>
  Type* end() const {return reinterpret_cast<Type*>(array) + length<Type>();}
  iterator end() const;
  const_iterator cend() const;

  void* unfree();

  void split(ui64 second_start, ByteArray& first, ByteArray& second);

  ByteArray& operator=(ByteArray other);

  byte& operator[](ui64 index);                             // byte& get(ui64) alias operator
  ByteArray& operator+=(byte b);                            // ByteArray& pushBack(byte) alias operator
  ByteArray& operator+=(const ByteArray& byte_array);       // ByteArray& pushBack(const ByteArray&) alias operator
  ByteArray& operator+=(const ByteArray&& byte_array);      // ByteArray& pushBack(const ByteArray&&) alias operator
  ByteArray& operator+=(const char* c_str);
  template<typename Type>
  ByteArray& operator+=(const Type& data) {return pushBack(data);}
};


}


#endif // BYTE_ARRAY_H
