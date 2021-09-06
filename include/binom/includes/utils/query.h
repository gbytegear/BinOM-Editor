#ifndef QUERY_H
#define QUERY_H

#include "path.h"

namespace binom {

enum class QProp : ui8 {
  type,
  type_class,
  value_type,
  element_count,
  index,
  name,
  value,
  sub_exp,
};

enum class QOper : ui8 {
  equal,
  not_equal,
  highter,
  highter_equal,
  lower,
  lower_equal,
  in_range,
  out_range,
  reg_test,
  contains
};

enum class QRel : ui8 {
  AND,
  OR,
  XOR
};

enum class QValType : ui8 {
  type,
  type_class,
  value_type,
  number,
  range,
  string
};

struct Range {
  i64 from;
  i64 to;

  bool in(i64 number) const {return number >= from && number <= to;}
  bool out(i64 number) const {return number < from || number > to;}
};

/*
{ // Query Initer
  {QProp::..., ?{path...}, QOper::..., QValue(...), QRel::...}, // Expression lvl#0

  {{ // Expression lvl#0
    {QProp::..., ?{path...}, QOper::..., QValue(...), QRel::...}, // SubExpression lvl#1
    {QProp::..., ?{path...}, QOper::..., QValue(...), QRel::...}, // SubExpression lvl#1
    {QProp::..., ?{path...}, QOper::..., QValue(...), QRel::...}, // SubExpression lvl#1

    {{ // SubExpression lvl#1
      {QProp::..., ?{path...}, QOper::..., QValue(...), QRel::...}, // SubExpression lvl#2
      {QProp::..., ?{path...}, QOper::..., QValue(...), QRel::...}, // SubExpression lvl#2
      {QProp::..., ?{path...}, QOper::..., QValue(...), QRel::...}, // SubExpression lvl#2
    }, QRel::...}

  }, QRel::...}
}

// Data model
[flag][path][value]
[flag][value:
  [size]
  [flag][path][value]
  [flag][path][value]
  [flag][path][value]
  [flag][value:
    [size]
    [flag][path][value]
    [flag][path][value]
    [flag][path][value]
  ]
]

*/

class Query {
  struct Flag;
  struct QValue;
public:
  class QueryLiteral;
  class QueryEpression;
  class iterator;

private:
  typedef std::initializer_list<QueryLiteral> QExprInitList;
  ByteArray data;

  static ByteArray buildSubexpression(QExprInitList& exprs);

  Query(ByteArray data);
public:
  Query(QExprInitList exprs);

  Query(const Query& other);
  Query(Query&& other);
  Query& operator=(const Query& other);
  Query& operator=(Query&& other);

  bool isEmpty() const;

  iterator begin();
  iterator end();

  ByteArray toByteArray() const;
  static Query fromByteArray(ByteArray data);
  static Query fromString(std::string str);

};

struct Query::Flag {
  // Octet #1
  QProp prop : 3;
  QOper oper : 4;
  bool has_path : 1;

  // Octet #2
  QValType val_type : 3;
  QRel next_rel : 2;

  Flag(QProp prop, QOper oper, bool has_path, QValType val_type, QRel next_rel);
};

struct Query::QValue {
  QValType value_type;
  union Data {
    VarType type;
    VarTypeClass type_class;
    ValType value_type;
    i64 number;
    Range range;
    BufferArray string; // or subexpression

    ~Data();
    Data(VarType type);
    Data(VarTypeClass type_class);
    Data(ValType value_type);
    Data(i64 number);
    Data(Range range);
    Data(BufferArray string);
    Data(QValType vtype, const Data& other);
  } data;

  QValue(VarType type);
  QValue(VarTypeClass type_class);
  QValue(ValType value_type);
  QValue(i64 number);
  QValue(Range range);
  QValue(BufferArray string);
  QValue(QValue&& other);
  ~QValue();
};

class Query::QueryLiteral {
  Flag flag;
  Path path;
  QValue value;
  friend ByteArray Query::buildSubexpression(Query::QExprInitList& exprs);
public:
  QueryLiteral(QProp prop, Path path, QOper oper, QValue val, QRel next_rel = QRel::AND);
  QueryLiteral(QProp prop, QOper oper, QValue val, QRel next_rel = QRel::AND);
  QueryLiteral(QExprInitList subexprs, QRel next_rel = QRel::AND);
  QueryLiteral(const QueryLiteral&) = delete;
  QueryLiteral(QueryLiteral&&) = delete;
};

class Query::iterator {
  ByteArray::iterator it;
public:
  iterator(ByteArray::iterator it);
  iterator(iterator& other);
  iterator(iterator&& other);

  QueryEpression& operator*();
  QueryEpression* operator->();

  iterator& operator++();

  iterator operator++(int);

  bool operator==(iterator other) const;
  bool operator!=(iterator other) const;
  bool operator>(iterator other) const;
  bool operator>=(iterator other) const;
  bool operator<(iterator other) const;
  bool operator<=(iterator other) const;
};

class Query::QueryEpression {
  ByteArray::iterator it;

  Flag& getFlag();
  byte* getDataPointer();
  byte* getPathPointer();

public:

  // Flag getters
  QProp getProp();
  QOper getOper();
  bool hasPath();
  QValType getQValType();
  QRel getNextRel();

  Path getPath();

  // Value getters
  VarType getVarType();
  VarTypeClass getVarTypeClass();
  ValType getValType();
  ui64 getUNumber();
  i64 getNumber();
  Range getRange();
  BufferArray getString();

  iterator begin();
  iterator end();
};

}


#endif // QUERY_H
