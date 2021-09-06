#ifndef PATH_H
#define PATH_H

#include "../variables/variable.h"

namespace binom {

enum class PathNodeType : ui8 {
  index,
  name
};

class Path {
public:

  class PathLiteral;
  class PathNode;
  class iterator;

private:
  ByteArray data;
  bool isValid();
  Path(ByteArray data);
public:
  Path(std::initializer_list<PathLiteral> path);
  Path();

  Path(const Path& other);
  Path(Path&& other);
  inline Path& operator=(const Path& other) {this->~Path(); return *new(this) Path(other);}
  inline Path& operator=(Path&& other) {this->~Path(); return *new(this) Path(std::move(other));}

  bool isEmpty() const;
  bool operator==(const Path& other) const;
  inline bool operator!=(const Path& other) const {return !(*this == other);}

  Path& pushBack(BufferArray name);
  Path& pushBack(ui64 index);
  inline Path& operator+=(BufferArray name) {return pushBack(std::move(name));}
  inline Path& operator+=(ui64 index) {return pushBack(index);}
  inline Path operator+(BufferArray name) {return Path(*this).pushBack(std::move(name));}
  inline Path operator+(ui64 index) {return Path(*this).pushBack(index);}

  iterator begin() const;
  iterator end() const;

  ByteArray toByteArray() const;
  std::string toString() const;
  static Path fromByteArray(ByteArray path);
  static Path fromString(std::string str);

};

class Path::PathLiteral{
  PathNodeType type;
  union PathLiteralValue {
    ui64 index;
    BufferArray string;
    PathLiteralValue(ui64 index) : index(index) {}
    PathLiteralValue(const BufferArray& string) : string(std::move(string)) {}
    PathLiteralValue(BufferArray&& string) : string(std::move(string)) {}
    ~PathLiteralValue() {}
  } value;
  friend class Path;
public:
  PathLiteral(ui64 index);
  PathLiteral(int index);
  PathLiteral(BufferArray string);
  PathLiteral(const char* string);
  PathLiteral(const PathLiteral& other) = delete;
  PathLiteral(PathLiteral&& other) = delete;
  ~PathLiteral();
};

class Path::PathNode  {
  PathNodeType* ptr;
public:
  PathNode(void* ptr);

  PathNodeType type() const;
  ui64 index() const;
  BufferArray name() const;
};

class Path::iterator {
  friend class Path;
  PathNodeType* ptr;
public:
  iterator(void* ptr);
  iterator(iterator& other);
  iterator(iterator&& other);

  PathNode& operator*();
  PathNode* operator->();

  iterator& operator++();

  iterator operator++(int);

  bool operator==(iterator other);
  bool operator!=(iterator other);
  bool operator>(iterator other);
  bool operator>=(iterator other);
  bool operator<(iterator other);
  bool operator<=(iterator other);
};


//Path operator""_pth(const char* str, size_t);

}

#endif // PATH_H
