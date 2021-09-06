#ifndef FILE_H
#define FILE_H

#if __has_include(<filesystem>)
  #include <filesystem>
  namespace fs = std::filesystem;
#elif __has_include(<experimental/filesystem>)
  #include <experimental/filesystem>
  namespace fs = std::experimental::filesystem;
#endif

#include <fstream>
#include <shared_mutex>

#include "byte_array.h"

namespace binom {

class FileIO {
public:
  typedef std::basic_ifstream<char> Reader;
  typedef std::basic_fstream<char> Writer;
private:
  fs::path file_path;
  std::shared_mutex mtx;
  inline Reader getReader() {return Reader(file_path.string(), std::ios::binary);}
  inline Writer getWriter() {return Writer(file_path.string(), std::ios::binary | std::ios::out | std::ios::in);}
public:
  static inline bool isExist(std::string_view file_path) {return fs::exists(file_path);}
  static inline bool isRegularFile(std::string_view file_path) {return fs::is_regular_file(file_path);}
  FileIO(std::string_view str) : file_path(str) {if(!isExist())std::ofstream(file_path.string(), std::ios::binary | std::ios::out);}
  FileIO(const FileIO& other) : file_path(other.file_path) {}
  FileIO(FileIO&& other) : file_path(std::move(other.file_path)) {}

  inline bool isExist() const {return fs::exists(file_path);}
  inline bool isEmpty() const {return fs::is_empty(file_path);}
  inline ui64 getSize() const {return fs::file_size(file_path);}
  inline fs::path getPath() const {return file_path;}

  inline void resize(ui64 new_size) {fs::resize_file(file_path, new_size);}
  inline ui64 addSize(ui64 add_size) {
    ui64 pos = getSize();
    fs::resize_file(file_path, pos + add_size);
    return pos;
  }
  inline void subSize(ui64 sub_size) {return fs::resize_file(file_path, getSize() - sub_size);}

  bool readBuffer(void* buffer, ui64 pos, ui64 size) {
    Reader reader = getReader();
    if(!reader.is_open()) return false;
    mtx.lock_shared();
    reader.seekg(pos, std::ios::beg);
    reader.read(static_cast<char*>(buffer), size);
    mtx.unlock_shared();
    return true;
  }

  template<typename T>
  T read(ui64 from) {
    T to;
    readBuffer(&to, from, sizeof(to));
    return to;
  }

  template<typename T>
  bool read(T& to, ui64 from) { return readBuffer(&to, from, sizeof(to)); }

  bool read(ByteArray& to, ui64 from) { return readBuffer(to.begin(), from, to.length()); }

  ByteArray read(ui64 from, ui64 size) {
    ByteArray data(size);
    readBuffer(data.begin(), from, size);
    return data;
  }

  bool writeBuffer(void* buffer, ui64 pos, ui64 size) {
    Writer writer = getWriter();
    if(!writer.is_open()) return false;
    mtx.lock();
    writer.seekp(pos, std::ios::beg);
    writer.write(static_cast<char*>(buffer), size);
    writer.flush();
    mtx.unlock();
    return true;
  }

  template<typename T>
  bool write(ui64 to, T& from) { return writeBuffer(&from, to, sizeof(from)); }

  bool write(ui64 to, const ByteArray from) { return writeBuffer(from.begin(), to, from.length()); }

};


}

#endif // FILE_H
