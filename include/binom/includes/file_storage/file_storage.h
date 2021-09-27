#ifndef FILE_STORAGE_H
#define FILE_STORAGE_H

#include "file_node_visitor.h"

namespace binom {

class FileStorage {
  FileMemoryManager fmm;
public:

  FileStorage(std::string_view file_name)
    : fmm(file_name) {}

  FileStorage(std::string_view file_name, Variable init_var, bool force_init = false)
    : fmm(file_name, force_init) {
    if(force_init || fmm.isEmpty())
      getRoot().setVariable(init_var);
  }


  FileNodeVisitor getRoot() {return fmm;}
  inline operator FileNodeVisitor () {return fmm;}
  FileNodeVisitor getNode(virtual_index index, real_index element_index = 0xFFFFFFFFFFFFFFFF) {
    return FileNodeVisitor(fmm, index, element_index);
  }

#ifdef DEBUG
  void checkHeap() {fmm.checkHeap();}
  void check() {fmm.check();}
#endif
};

}

#endif // FILE_STORAGE_H
