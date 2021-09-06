#ifndef FMM_CONTAINERS_H
#define FMM_CONTAINERS_H

#ifdef DEBUG
#define IF_DEBUG(expr) expr
#include <iostream>
#else
#define IF_DEBUG(expr)
#endif

#include "file_structs.h"
#include <vector>
#include <map>
#include <mutex>

namespace binom {

typedef ui64 virtual_index;
typedef ui64 real_index;
typedef ui64 block_size;

struct NodePageInfo {
  NodePageDescriptor descriptor;
  real_index page_position;
};

struct HeapPageInfo {
  HeapPageDescriptor descriptor;
  real_index page_position;
};

struct NodeInfo {
  NodeDescriptor descriptor;
  virtual_index v_index;
};

struct NodeFullInfo {
  virtual_index v_index;
  NodeDescriptor descriptor;
  ByteArray data;
};


typedef std::vector<NodePageInfo> NodePageVector;
typedef std::vector<HeapPageInfo> HeapPageVector;
/// Vector of real memory blocks
typedef std::vector<RMemoryBlock> RMemoryBlockVector;


class HeapMap {

  struct HeapBlock {
    bool is_busy = false;
    block_size size = 0;
  };

  typedef std::map<virtual_index, HeapBlock> BlockMap;
  typedef BlockMap::iterator block_iterator;
  typedef std::multimap<block_size, block_iterator> FreeBlockMap;
  typedef FreeBlockMap::iterator free_block_iterator;

  BlockMap block_map;
  FreeBlockMap free_block_map;
  std::recursive_mutex mtx;

  inline free_block_iterator findFree(block_size size) {return free_block_map.lower_bound(size);}
  free_block_iterator findFree(block_iterator block_it) {
    auto range = free_block_map.equal_range(block_it->second.size);
    for(auto it = range.first; it != range.second; ++it)
      if(it->second == block_it) return it;
    return free_block_map.end();
  }

  inline block_iterator findBusy(virtual_index v_index) {return block_map.find(v_index);}

  bool updateFreeBlockSize(block_iterator block_it, block_size old_size) {
    auto range = free_block_map.equal_range(old_size);
    for(auto it = range.first; it != range.second; ++it)
      if(it->second == block_it) {
        auto node = free_block_map.extract(it);
        node.key() = block_it->second.size;
        free_block_map.insert(std::move(node));
        return true;
      }
    return false;
  }

  IF_DEBUG(friend void test();)

public:

  inline std::recursive_mutex& getHeapControlMutex() {return mtx;}

  void occupyBlock(virtual_index v_index, block_size size) {
    if(!size) return;
    block_iterator block_it = --block_map.upper_bound(v_index);
    if(block_it->second.is_busy)
      throw Exception(ErrCode::binomdb_memory_management_error, "Tryimg occupy busy block");

    // Split block tail
    if(block_it->first < v_index) {
      block_size old_size = block_it->second.size;
      block_it->second.size = v_index - block_it->first;
      updateFreeBlockSize(block_it, old_size);
      block_iterator new_block_it = block_map.emplace(v_index, HeapBlock{false, old_size - block_it->second.size}).first;
      free_block_map.emplace(new_block_it->second.size, new_block_it);
      block_it = new_block_it;
    }

    // Mark as busy
    block_it->second.is_busy = true;
    if(free_block_iterator free_block_it = findFree(block_it); free_block_it != free_block_map.cend())
      free_block_map.erase(free_block_it);

    if(block_it->second.size == size) return;

    // Block split
    block_iterator new_block_it = block_map.emplace(block_it->first + size, HeapBlock{false, block_it->second.size - size}).first;
    free_block_map.emplace(new_block_it->second.size, new_block_it);
    block_it->second.size = size;

  }

  inline VMemoryBlock find(virtual_index v_index) {
    block_iterator block_it = block_map.find(v_index);
    if(block_it == block_map.cend()) return VMemoryBlock{0,0};
    else return VMemoryBlock{block_it->first, block_it->second.size};
  }

  void expandMemory(block_size add) {
    if(!add) return;
    std::scoped_lock s_lock(mtx);
    if(block_map.empty()) {
      block_iterator it = block_map.emplace<virtual_index, HeapBlock>(0, HeapBlock{false, add}).first;
      free_block_map.emplace(it->second.size, it);
      return;
    }
    block_iterator it = --block_map.end();
    if(it->second.is_busy) {
      it = block_map.emplace<virtual_index, HeapBlock>(it->first + it->second.size, HeapBlock{false, add}).first;
      free_block_map.emplace(it->second.size, it);
      return;
    } else {
      block_size old_size = it->second.size;
      it->second.size += add;
      updateFreeBlockSize(it, old_size);
    }
  }

  VMemoryBlock allocBlock(block_size size) {
    if(!size) return VMemoryBlock{0, 0};
    std::scoped_lock s_lock(mtx);
    free_block_iterator free_block_it = findFree(size);
    if(free_block_it == free_block_map.cend()) return VMemoryBlock{0, 0};

    block_iterator block_it = free_block_it->second;
    free_block_map.erase(free_block_it);
    block_it->second.is_busy = true;
    if(block_it->second.size == size) return VMemoryBlock{block_it->first, block_it->second.size};

    // Block split
    block_iterator new_block_it = block_map.emplace(block_it->first + size, HeapBlock{false, block_it->second.size - size}).first;
    free_block_map.emplace(new_block_it->second.size, new_block_it);
    block_it->second.size = size;
    return VMemoryBlock{block_it->first, block_it->second.size};
  }

  void freeBlock(virtual_index v_index) {
    std::scoped_lock s_lock(mtx);
    block_iterator it = findBusy(v_index);
    if(it == block_map.cend())
      throw Exception(ErrCode::binomdb_memory_management_error, "Trying to free a non-existent block");
    it->second.is_busy = false;
    // Merge blocks
    block_iterator free_it = it;
    block_size old_size = free_it->second.size;

    while(true) {
      block_iterator free_last_it = free_it;
      if(free_it == block_map.begin()) break;
      --free_it;
      if(free_it->second.is_busy) {
        ++free_it;
        break;
      }
      old_size = free_it->second.size;
      free_it->second.size += free_last_it->second.size;
      if(free_block_iterator free_map_last_it = findFree(free_last_it); free_map_last_it != free_block_map.cend())
        free_block_map.erase(free_map_last_it);
      block_map.erase(free_last_it);
    }

    while (true) {
      block_iterator free_next_it = free_it;
      ++free_next_it;
      if(free_next_it == block_map.end() || free_next_it->second.is_busy) break;
      free_it->second.size += free_next_it->second.size;
      if(free_block_iterator free_map_next_it = findFree(free_next_it); free_map_next_it != free_block_map.cend())
        free_block_map.erase(free_map_next_it);
      block_map.erase(free_next_it);
    }

    if(!updateFreeBlockSize(free_it, old_size))
      free_block_map.emplace(free_it->second.size, free_it);
  }

  void clear() {block_map.clear();free_block_map.clear();}

  IF_DEBUG(
      void check() {
        ui64 allocated = 0;
        ui64 free_memory = 0;
        ui64 free_blocks = 0;
        ui64 busy_memory = 0;
        ui64 busy_blocks = 0;
        std::clog << "Block map:\n";
        for(auto block : block_map) {
          allocated += block.second.size;
          if(block.second.is_busy) {
            busy_memory += block.second.size;
            ++busy_blocks;
          } else {
            free_memory += block.second.size;
            ++free_blocks;
          }
          std::clog << "[ index: " << block.first << ", size: " << block.second.size << ", is busy: " << block.second.is_busy << " ]\n";
        }

        std::clog << "\n\nFree map:\n";
        for(auto block : free_block_map) {
          std::clog << "[ size: " << block.first << " index: " << block.second->first << " ]\n";
        }

        std::clog << "\n\nFree blocks: " << free_blocks << '\n';
        std::clog << "Busy blocks: " << busy_blocks << '\n';
        std::clog << "Free memory: "<< free_memory << '\n';
        std::clog << "Busy memory: "<< busy_memory << '\n';
        std::clog << "Allocated: " << allocated << '\n';
      }
  )

};


}

#endif // FMM_CONTAINERS_H
