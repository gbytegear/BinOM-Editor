#ifndef NODE_VISITOR_H
#define NODE_VISITOR_H

#include "node_visitor_base.h"

#include <vector>
#include <functional>

namespace binom {

class NodeVisitor;

class NodeVisitor : public NodeVisitorBase {
  enum class RefType : ui8 {
    variable,
    named_variable,
    value
  };

  union Ref {
    void* ptr;
    Variable* variable;
    NamedVariable* named_variable;
    ValueRef value;

    Ref() : ptr(nullptr) {}
    Ref(Variable* var) : variable(var) {}
    Ref(NamedVariable* named_var) : named_variable(named_var) {}
    Ref(ValueRef val) : value(val) {}
    Ref(const Ref& ref) {memcpy(this, &ref, sizeof (Ref));}
    Ref(const Ref&& ref) {memcpy(this, &ref, sizeof (Ref));}

    Ref& operator=(Ref ref) {return *new(this) Ref(ref);}

  };

  RefType ref_type;
  Ref ref;

  bool test(Query query, ui64 index) noexcept;

  void setNull();

  friend class UnionNodeVisitor;

public:

  class NodeIterator;

  NodeVisitor(decltype(nullptr) null = nullptr);
  NodeVisitor(Variable* var);
  NodeVisitor(NamedVariable* named_var);
  NodeVisitor(ValueRef val);
  NodeVisitor(const NodeVisitor& other);
  NodeVisitor(NodeVisitor&& other);

  NodeVisitor& operator=(Variable* var);
  NodeVisitor& operator=(NamedVariable* named_var);
  NodeVisitor& operator=(ValueRef val);
  NodeVisitor& operator=(const NodeVisitor& other);

  VisitorType getVisitorType() const override {return VisitorType::ram_storage_visitor;}
  VarType getType() const override;

  bool isNull() const override;
  bool isNamed() const {return ref_type == RefType::named_variable;}
  bool isValueRef() const override {return ref_type == RefType::value;}

  ui64 getElementCount() const override;

  NodeVisitor& stepInside(ui64 index) override;
  NodeVisitor& stepInside(BufferArray name) override;
  NodeVisitor& stepInside(Path path) override;

  BufferArray& rename(BufferArray old_name, BufferArray new_name);

  ValueRef getValue() const;
  ValueRef getValue(ui64 index) const;
  Variable& getVariable() const;
  Variable& getVariable(ui64 index) const;
  Variable& getVariable(BufferArray name) const;
  Variable& getVariable(Path path) const;
  std::optional<BufferArray> getName() const;

  void setVariable(Variable var) override;
  void pushBack(Variable var) override;
  void pushFront(Variable var) override;
  void insert(ui64 index, Variable var) override;
  void insert(BufferArray name, Variable var) override;
  void remove(ui64 index, ui64 count = 1) override;
  void remove(BufferArray name) override;
  void remove(Path path) override;

  NodeVisitor getChild(ui64 index) {return NodeVisitor(*this).stepInside(index);}
  NodeVisitor getChild(BufferArray name) {return NodeVisitor(*this).stepInside(std::move(name));}
  NodeVisitor getChild(Path path) {return NodeVisitor(*this).stepInside(std::move(path));}

  NodeVisitor operator[](ui64 index) const {return NodeVisitor(*this).stepInside(index);}
  NodeVisitor operator[](BufferArray name) const {return NodeVisitor(*this).stepInside(std::move(name));}
  NodeVisitor operator[](Path path) const {return NodeVisitor(*this).stepInside(std::move(path));}

  NodeVisitor& operator()(ui64 index) override {return stepInside(index);}
  NodeVisitor& operator()(BufferArray name) override {return stepInside(std::move(name));}
  NodeVisitor& operator()(Path path) override {return stepInside(std::move(path));}

  NodeVector findAll(Query query, NodeVector node_vector = NodeVector());
  NodeVisitor find(Query query);

  NodeIterator begin();
  NodeIterator end();

  // Functional

  NodeVisitor& ifNotNull(std::function<void(NodeVisitor&)> callback);
  NodeVisitor& ifNull(std::function<void()> callback);
  void foreach(std::function<void(NodeVisitor&)> callback);

  NodeVisitor& toRAMVisitor() = delete;
  FileNodeVisitor& toFileVisitor() = delete;

};



class NodeVisitor::NodeIterator {
  Variable* parent = nullptr;

  union Ptr {
    Variable* variable;
    NamedVariable* named_variable;
    ValueIterator value_it;

    Ptr(VarTypeClass type, Variable& parent, bool is_end = false) {
      switch (type) {
        case VarTypeClass::buffer_array: value_it = is_end? parent.toBufferArray().end() : parent.toBufferArray().begin(); return;
        case VarTypeClass::array:        variable = is_end? parent.toArray().end() : parent.toArray().begin(); return;
        case VarTypeClass::object:       named_variable = is_end? parent.toObject().end() : parent.toObject().begin(); return;
        default: throw Exception(ErrCode::binom_invalid_type);
      }
    }

    Ptr(VarTypeClass type, Ptr& other) {
      switch (type) {
        case VarTypeClass::buffer_array: value_it = other.value_it; return;
        case VarTypeClass::array:        variable = other.variable; return;
        case VarTypeClass::object:       named_variable = other.named_variable; return;
        default: throw Exception(ErrCode::binom_invalid_type);
      }
    }

  };

  VarTypeClass type;
  Ptr ptr;

public:
  NodeIterator(NodeVisitor& node, bool is_end = false)
    : parent(&node.getVariable()),
      type(toTypeClass(node.getType())),
      ptr(type, *parent, is_end) {}

  NodeIterator(NodeIterator& other)
    : parent(other.parent),
      type(other.type),
      ptr(type, other.ptr) {}

  NodeIterator(NodeIterator&& other)
    : parent(other.parent),
    type(other.type),
    ptr(type, other.ptr) {}

  NodeIterator& operator++() {
    switch (type) {
      case VarTypeClass::buffer_array: ++ptr.value_it; break;
      case VarTypeClass::array:        ++ptr.variable; break;
      case VarTypeClass::object:       ++ptr.named_variable; break;
      default: throw Exception(ErrCode::binom_invalid_type);
    }
    return *this;
  }

  NodeIterator operator++(int) {
    NodeIterator tmp(*this);
    switch (type) {
      case VarTypeClass::buffer_array: ++ptr.value_it; break;
      case VarTypeClass::array:        ++ptr.variable; break;
      case VarTypeClass::object:       ++ptr.named_variable; break;
      default: throw Exception(ErrCode::binom_invalid_type);
    }
    return tmp;
  }

  NodeIterator& operator--(){
    switch (type) {
      case VarTypeClass::buffer_array: --ptr.value_it; break;
      case VarTypeClass::array:        --ptr.variable; break;
      case VarTypeClass::object:       --ptr.named_variable; break;
      default: throw Exception(ErrCode::binom_invalid_type);
    }
    return *this;
  }

  NodeIterator operator--(int) {
    NodeIterator tmp(*this);
    switch (type) {
      case VarTypeClass::buffer_array: --ptr.value_it; break;
      case VarTypeClass::array:        --ptr.variable; break;
      case VarTypeClass::object:       --ptr.named_variable; break;
      default: throw Exception(ErrCode::binom_invalid_type);
    }
    return tmp;
  }

  bool operator==(NodeIterator& other) {
    if(type != other.type) return false;
    switch (type) {
      case VarTypeClass::buffer_array: return ptr.value_it == other.ptr.value_it;
      case VarTypeClass::array:        return ptr.variable == other.ptr.variable;
      case VarTypeClass::object:       return ptr.named_variable == other.ptr.named_variable;
      default: throw Exception(ErrCode::binom_invalid_type);
    }
  }

  bool operator!=(NodeIterator& other) {
     if(type != other.type) return false;
     switch (type) {
       case VarTypeClass::buffer_array: return ptr.value_it != other.ptr.value_it;
       case VarTypeClass::array:        return ptr.variable != other.ptr.variable;
       case VarTypeClass::object:       return ptr.named_variable != other.ptr.named_variable;
       default: throw Exception(ErrCode::binom_invalid_type);
     }
  }

  NodeVisitor operator*() {
    switch (type) {
      case VarTypeClass::buffer_array: return *ptr.value_it;
      case VarTypeClass::array:        return ptr.variable;
      case VarTypeClass::object:       return ptr.named_variable;
      default: throw Exception(ErrCode::binom_invalid_type);
    }
  }


};

}

#endif
