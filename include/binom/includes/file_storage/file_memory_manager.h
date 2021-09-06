#ifndef FILE_MEMORY_MANAGER_H
#define FILE_MEMORY_MANAGER_H

#define DEBUG

#include "binom/includes/utils/file.h"
#include "fmm_containers.h"
#include "../utils/rwguard.h"

namespace binom {

class FileMemoryManager {
  FileIO file;
  NodePageVector node_page_vector;
  HeapPageVector heap_page_vector;
  HeapMap heap_map;
  DBHeader db_header;
  std::shared_mutex node_page_mtx;
  std::shared_mutex heap_page_mtx;
  RWSyncMap sync_map;

  void init();

  void allocNodePage();
  void allocHeapPage();

  VMemoryBlock allocHeapBlock(block_size size);


  virtual_index allocNode();
  inline void freeHeapBlock(virtual_index v_index) {return heap_map.freeBlock(v_index);}

  real_index translateVNodeIndex(virtual_index v_index);
  real_index translateVHeapIndex(virtual_index v_index);
  RMemoryBlockVector translateVMemoryBlock(VMemoryBlock v_mem_block);

  void writeToVBlock(VMemoryBlock block, ByteArray data, real_index shift = 0);

public:
  FileMemoryManager(std::string_view file_path);

  inline NodeDescriptor getNodeDescriptor(virtual_index node_index) {return file.read<NodeDescriptor>(translateVNodeIndex(node_index));}
  inline ByteArray getNodeData(virtual_index node_index) {return getNodeData(getNodeDescriptor(node_index));}
  ByteArray getNodeData(NodeDescriptor descriptor);
  inline ByteArray getNodeDataPart(virtual_index node_index, real_index shift, block_size size) {return getNodeDataPart(getNodeDescriptor(node_index), shift, size);}
  ByteArray getNodeDataPart(NodeDescriptor descriptor, real_index shift, block_size size);
  NodeFullInfo getFullNodeInfo(virtual_index node_index);

  virtual_index createNode(VarType type, ByteArray data);
  void updateNode(virtual_index node_index, VarType type, ByteArray data, NodeDescriptor* descriptor_ptr = nullptr);
  inline void updateNode(virtual_index node_index, ByteArray data, NodeDescriptor* descriptor_ptr = nullptr) {return updateNode(node_index, VarType::end, std::move(data), descriptor_ptr);}
  void updateNodeDataPart(virtual_index node_index, real_index shift, ByteArray data, NodeDescriptor* descriptor_ptr = nullptr);
  void removeNode(virtual_index node_index);

  inline bool isEmpty() {return db_header.root_node.isFree();}

  inline RWSyncMap::RWGuard getRWGuard(f_virtual_index node_index) {return sync_map.get(node_index);}

IF_DEBUG(
  void checkHeap();
  void check();
)

};

}

#endif // FILE_MEMORY_MANAGER_H
