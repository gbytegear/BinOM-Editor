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

void QBinOMModel::buildData(QVariantList& data, binom::Path path, binom::NodeVisitor& node) {
  bool node_is_object = node.getTypeClass() == binom::VarTypeClass::object;
  binom::ui64 index = 0;
  for(binom::NodeVisitor child : node) {

    binom::BufferArray name = *node.getName();

    binom::Path child_path = (node_is_object? (path + *node.getName()) : (path + index++));
    data.push_back(QVariantMap {
                     {"type", binom::toTypeString(child.getType())},
                     {"path", QString::fromStdString(child_path.toString())},
                     {"value", (child.getTypeClass() == binom::VarTypeClass::primitive)
                               ? QVariant(qulonglong(child.getValue().asUi64()))
                               : QVariant()}
                   });
    if(opened.contains(QString::fromStdString(child_path.toString())))
      buildData(data, child_path, child);
  }
}

void QBinOMModel::buildData(QVariantList& data, binom::Path path, binom::FileNodeVisitor& node) {
  bool node_is_object = node.getTypeClass() == binom::VarTypeClass::object;
  binom::ui64 index = 0;
  for(binom::FileNodeVisitor child : node) {
    binom::Path child_path = (node_is_object? (path + *node.getName()) : (path + index++));
    data.push_back(QVariantMap {
                     {"type", binom::toTypeString(child.getType())},
                     {"path", QString::fromStdString(child_path.toString())},
                     {"value", (child.getTypeClass() == binom::VarTypeClass::primitive)
                               ? QVariant(qulonglong(child.getVariable().getValue().asUi64()))
                               : QVariant()}
                   });
    if(opened.contains(QString::fromStdString(child_path.toString())))
      buildData(data, child_path, child);
  }
}
