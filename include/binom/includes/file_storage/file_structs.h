#ifndef FILE_NODE_STRUCTS_H
#define FILE_NODE_STRUCTS_H
#include "../utils/types.h"


namespace binom {

class BitIterator;

//! Real index in file
typedef ui64 real_index;
//! Virtual index for translation to real index
typedef ui64 virtual_index;
//! Size of data block in file
typedef ui64 block_size;
//! Element count in the file
typedef ui64 element_cnt;

struct VMemoryBlock {
  virtual_index v_index = 0;
  block_size size = 0;
  inline bool isEmpty() const {return !v_index && !size;}
};

struct RMemoryBlock {
  real_index r_index;
  block_size size;
};

#pragma pack(push, 1)


class BitMap {

  union {
    ui64 number = 0;
    struct {
      bool _0:1;
      bool _1:1;
      bool _2:1;
      bool _3:1;
      bool _4:1;
      bool _5:1;
      bool _6:1;
      bool _7:1;
      bool _8:1;
      bool _9:1;
      bool _10:1;
      bool _11:1;
      bool _12:1;
      bool _13:1;
      bool _14:1;
      bool _15:1;
      bool _16:1;
      bool _17:1;
      bool _18:1;
      bool _19:1;
      bool _20:1;
      bool _21:1;
      bool _22:1;
      bool _23:1;
      bool _24:1;
      bool _25:1;
      bool _26:1;
      bool _27:1;
      bool _28:1;
      bool _29:1;
      bool _30:1;
      bool _31:1;
      bool _32:1;
      bool _33:1;
      bool _34:1;
      bool _35:1;
      bool _36:1;
      bool _37:1;
      bool _38:1;
      bool _39:1;
      bool _40:1;
      bool _41:1;
      bool _42:1;
      bool _43:1;
      bool _44:1;
      bool _45:1;
      bool _46:1;
      bool _47:1;
      bool _48:1;
      bool _49:1;
      bool _50:1;
      bool _51:1;
      bool _52:1;
      bool _53:1;
      bool _54:1;
      bool _55:1;
      bool _56:1;
      bool _57:1;
      bool _58:1;
      bool _59:1;
      bool _60:1;
      bool _61:1;
      bool _62:1;
      bool _63:1;
    } bits;
  } data;

public:

  bool get(ui8 index) {
    switch (index) {
      case 0: return data.bits._0;
      case 1: return data.bits._1;
      case 2: return data.bits._2;
      case 3: return data.bits._3;
      case 4: return data.bits._4;
      case 5: return data.bits._5;
      case 6: return data.bits._6;
      case 7: return data.bits._7;
      case 8: return data.bits._8;
      case 9: return data.bits._9;
      case 10: return data.bits._10;
      case 11: return data.bits._11;
      case 12: return data.bits._12;
      case 13: return data.bits._13;
      case 14: return data.bits._14;
      case 15: return data.bits._15;
      case 16: return data.bits._16;
      case 17: return data.bits._17;
      case 18: return data.bits._18;
      case 19: return data.bits._19;
      case 20: return data.bits._20;
      case 21: return data.bits._21;
      case 22: return data.bits._22;
      case 23: return data.bits._23;
      case 24: return data.bits._24;
      case 25: return data.bits._25;
      case 26: return data.bits._26;
      case 27: return data.bits._27;
      case 28: return data.bits._28;
      case 29: return data.bits._29;
      case 30: return data.bits._30;
      case 31: return data.bits._31;
      case 32: return data.bits._32;
      case 33: return data.bits._33;
      case 34: return data.bits._34;
      case 35: return data.bits._35;
      case 36: return data.bits._36;
      case 37: return data.bits._37;
      case 38: return data.bits._38;
      case 39: return data.bits._39;
      case 40: return data.bits._40;
      case 41: return data.bits._41;
      case 42: return data.bits._42;
      case 43: return data.bits._43;
      case 44: return data.bits._44;
      case 45: return data.bits._45;
      case 46: return data.bits._46;
      case 47: return data.bits._47;
      case 48: return data.bits._48;
      case 49: return data.bits._49;
      case 50: return data.bits._50;
      case 51: return data.bits._51;
      case 52: return data.bits._52;
      case 53: return data.bits._53;
      case 54: return data.bits._54;
      case 55: return data.bits._55;
      case 56: return data.bits._56;
      case 57: return data.bits._57;
      case 58: return data.bits._58;
      case 59: return data.bits._59;
      case 60: return data.bits._60;
      case 61: return data.bits._61;
      case 62: return data.bits._62;
      case 63: return data.bits._63;
      default: return false;
    }
  }

  bool set(ui8 index, bool value) {
    switch (index) {
      case 0: return data.bits._0 = value;
      case 1: return data.bits._1 = value;
      case 2: return data.bits._2 = value;
      case 3: return data.bits._3 = value;
      case 4: return data.bits._4 = value;
      case 5: return data.bits._5 = value;
      case 6: return data.bits._6 = value;
      case 7: return data.bits._7 = value;
      case 8: return data.bits._8 = value;
      case 9: return data.bits._9 = value;
      case 10: return data.bits._10 = value;
      case 11: return data.bits._11 = value;
      case 12: return data.bits._12 = value;
      case 13: return data.bits._13 = value;
      case 14: return data.bits._14 = value;
      case 15: return data.bits._15 = value;
      case 16: return data.bits._16 = value;
      case 17: return data.bits._17 = value;
      case 18: return data.bits._18 = value;
      case 19: return data.bits._19 = value;
      case 20: return data.bits._20 = value;
      case 21: return data.bits._21 = value;
      case 22: return data.bits._22 = value;
      case 23: return data.bits._23 = value;
      case 24: return data.bits._24 = value;
      case 25: return data.bits._25 = value;
      case 26: return data.bits._26 = value;
      case 27: return data.bits._27 = value;
      case 28: return data.bits._28 = value;
      case 29: return data.bits._29 = value;
      case 30: return data.bits._30 = value;
      case 31: return data.bits._31 = value;
      case 32: return data.bits._32 = value;
      case 33: return data.bits._33 = value;
      case 34: return data.bits._34 = value;
      case 35: return data.bits._35 = value;
      case 36: return data.bits._36 = value;
      case 37: return data.bits._37 = value;
      case 38: return data.bits._38 = value;
      case 39: return data.bits._39 = value;
      case 40: return data.bits._40 = value;
      case 41: return data.bits._41 = value;
      case 42: return data.bits._42 = value;
      case 43: return data.bits._43 = value;
      case 44: return data.bits._44 = value;
      case 45: return data.bits._45 = value;
      case 46: return data.bits._46 = value;
      case 47: return data.bits._47 = value;
      case 48: return data.bits._48 = value;
      case 49: return data.bits._49 = value;
      case 50: return data.bits._50 = value;
      case 51: return data.bits._51 = value;
      case 52: return data.bits._52 = value;
      case 53: return data.bits._53 = value;
      case 54: return data.bits._54 = value;
      case 55: return data.bits._55 = value;
      case 56: return data.bits._56 = value;
      case 57: return data.bits._57 = value;
      case 58: return data.bits._58 = value;
      case 59: return data.bits._59 = value;
      case 60: return data.bits._60 = value;
      case 61: return data.bits._61 = value;
      case 62: return data.bits._62 = value;
      case 63: return data.bits._63 = value;
      default: return false;
    }
  }

  ui64 value() {return data.number;}

  BitIterator begin();
  BitIterator end();

};

//! Version of data base
struct DBVersion {
  const char file_type[9] = "BinOM.FS"; // File_Storage
  const ui16 major = 0;
  const ui16 minor = 0;
};

//! Current version of data base
constexpr DBVersion current{"BinOM.FS", 0, 1};

//! Descriptor of node page
struct NodePageDescriptor {
  real_index next_node_page = 0;
  BitMap node_map;
};

//! Descriptor of heap page
struct HeapPageDescriptor {
  real_index next_heap_page = 0;
};

//! Descriptor of BinOM node
struct NodeDescriptor {
  VarType type = VarType::end;
  virtual_index index = 0; ///< Value for primitive types
  block_size size = 0;

  static inline NodeDescriptor null() {return {VarType::invalid_type, 0xFFFFFFFFFFFFFFFF, 0};}
  bool isNull() {return type == VarType::invalid_type && index == 0xFFFFFFFFFFFFFFFF && !size;}
  bool isFree() {return type == VarType::end && !index && !size;}
  VMemoryBlock toVMemoryBlock() {return {index, size};}
};

struct ObjectNameLength {
  ValType char_type = ValType::invalid_type;
  block_size name_length = 0;
  element_cnt name_count = 0;
};

struct ObjectDescriptor {
  static constexpr block_size length_element_size = sizeof (ObjectNameLength);
  static constexpr block_size index_size = sizeof (virtual_index);

  element_cnt length_element_count = 0;
  block_size name_block_size = 0;
  element_cnt index_count = 0;
};


//! Data Base descriptor
struct DBHeader {
  enum class VersionDifference {
    identical = 0,
    file_type = 1,
    major = 2,
    minor = 3,
  };

  DBVersion version = current;
  real_index first_node_page_index = 0;
  real_index first_heap_page_index = 0;
  NodeDescriptor root_node;

  VersionDifference checkFileVersion() {
    if(memcmp(version.file_type, current.file_type, sizeof(current.file_type)))
      return VersionDifference::file_type;
    if(version.major != current.major)
      return VersionDifference::major;
    if(version.minor != current.minor)
      return VersionDifference::minor;
    return VersionDifference::identical;
  }
};


#pragma pack(pop)


class BitIterator {
  friend class BitMap;
  BitMap* map;
  ui8 bit_index;
  BitIterator(BitMap* map, ui8 bit_index = 0) : map(map), bit_index(bit_index) {}
public:
  BitIterator(BitIterator& other) : map(other.map), bit_index(other.bit_index) {}
  BitIterator(BitIterator&& other) : map(other.map), bit_index(other.bit_index) {}

  bool get() {return map->get(bit_index);}
  ui8 getBitIndex() {return bit_index;}
  bool set(bool value) {return map->set(bit_index, value);}
  BitIterator& operator*() {return *this;}

  BitIterator& operator++() {if(bit_index < 64)++bit_index;return *this;}
  BitIterator operator++(int) {BitIterator prev(*this);if(bit_index < 64)++bit_index;return prev;}

  BitIterator& operator--() {if(bit_index > 0)--bit_index;return *this;}
  BitIterator operator--(int) {BitIterator next(*this);if(bit_index > 0)--bit_index;return next;}

  BitIterator& operator+=(ui8 add) {if((bit_index += add) > 64) bit_index = 64;return *this;}
  BitIterator& operator-=(ui8 sub) {if((bit_index -= sub) > 64) bit_index = 0;return *this;}

  BitIterator operator+(ui8 add) {return BitIterator(*this) += add;}
  BitIterator operator-(ui8 sub) {return BitIterator(*this) -= sub;}

  BitIterator& operator=(BitIterator other) {map = other.map; bit_index = other.bit_index; return *this;}
  BitIterator& operator=(BitMap* map) {this->map = map; bit_index = 0; return *this;}

  bool operator==(BitIterator other) {return map == other.map && bit_index == other.bit_index;}
  bool operator!=(BitIterator other) {return map != other.map || bit_index != other.bit_index;}
  bool operator<(BitIterator other) {return bit_index < other.bit_index;}
  bool operator<=(BitIterator other) {return bit_index <= other.bit_index;}
  bool operator>(BitIterator other) {return bit_index > other.bit_index;}
  bool operator>=(BitIterator other) {return bit_index >= other.bit_index;}

  bool isBegin() const {return bit_index == 0;}
  bool isEnd() const {return bit_index >= 64;}
};

inline BitIterator BitMap::begin() {return BitIterator(this);}
inline BitIterator BitMap::end() {return BitIterator(this, 64);}

constexpr element_cnt node_page_node_count = 64;
constexpr block_size node_page_size = sizeof(NodePageDescriptor) + sizeof(NodeDescriptor)*node_page_node_count;
constexpr block_size heap_page_data_size = 4096;
constexpr block_size heap_page_size = sizeof(HeapPageDescriptor) + heap_page_data_size;
//constexpr block_size byte_page_size = sizeof(BytePageDescriptor) + 64;

}


#endif
