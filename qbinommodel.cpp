#include "qbinommodel.h"


QBinOMModel::operator QVariantList() {
  QVariantList data;
  switch (node->getVisitorType()) {
    case binom::VisitorType::ram_storage_visitor:
      buildData(data, {}, node->toRAMVisitor());
    break;
    case binom::VisitorType::file_storage_visitor:
      buildData(data, {}, node->toFileVisitor());
    break;
  }
  return data;
}

void QBinOMModel::buildData(QVariantList& data, binom::Path path, binom::NodeVisitor& node, quint64 depth) {
  bool node_is_object = node.getTypeClass() == binom::VarTypeClass::object;
  binom::ui64 index = 0;
  for(binom::NodeVisitor child : node) {

    binom::Path child_path = (node_is_object? (path + *child.getName()) : (path + index++));
    bool is_open = opened.contains(QString::fromStdString(child_path.toString()));
    data.push_back(QVariantMap {
                     {"type", binom::toTypeString(child.getType())},
                     {"path", QString::fromStdString(child_path.toString())},
                     {"is_open", is_open},
                     {"key", (node_is_object? QVariant(QString::fromStdString(*child.getName())) : QVariant(qulonglong(index - 1)))},
                     {"depth", QVariant(depth)},
                     {"value", (child.getTypeClass() == binom::VarTypeClass::primitive)
                               ? QVariant(qulonglong(child.getValue().asUi64()))
                               : (child.getType() == binom::VarType::byte_array)
                               ? QVariant(QString::fromStdString(child.getVariable().toBufferArray()))
                               : QVariant()}
                   });
    if(is_open)
      buildData(data, child_path, child, depth + 1);
  }
}

void QBinOMModel::buildData(QVariantList& data, binom::Path path, binom::FileNodeVisitor& node, quint64 depth) {
  bool node_is_object = node.getTypeClass() == binom::VarTypeClass::object;
  binom::ui64 index = 0;
  for(binom::FileNodeVisitor child : node) {
    binom::Path child_path = (node_is_object? (path + *child.getName()) : (path + index++));
    data.push_back(QVariantMap {
                     {"type", binom::toTypeString(child.getType())},
                     {"path", QString::fromStdString(child_path.toString())},
                     {"depth", QVariant(depth)},
                     {"value", (child.getTypeClass() == binom::VarTypeClass::primitive)
                               ? QVariant(qulonglong(child.getVariable().getValue().asUi64()))
                               : QVariant()}
                   });
    if(opened.contains(QString::fromStdString(child_path.toString())))
      buildData(data, child_path, child, depth + 1);
  }
}
