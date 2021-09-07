#include "qbinom.h"

// BinOMFile & BinOMFile::File

BinOMFile::BinOMFile(binom::FileType file_type, QString file_path)
  : file_type(file_type),
    storage_union(file_type, file_path),
    tree_model(*getRoot()) {}

BinOMFile::~BinOMFile() {
  switch (file_type) {
    case binom::FileType::undefined_file: return;
    case binom::FileType::file_storage:
      storage_union.file.~FileStorage();
    break;
    case binom::FileType::serialized_file_storage:
      storage_union.serialized.~SerializedStorage();
    break;
  }
}

std::unique_ptr<binom::NodeVisitorBase> BinOMFile::getRoot() {
  switch (file_type) {
    default: return nullptr;
    case binom::FileType::file_storage:
    return std::unique_ptr<binom::NodeVisitorBase>(new binom::FileNodeVisitor(storage_union.file.root_node));
    case binom::FileType::serialized_file_storage:
    return std::unique_ptr<binom::NodeVisitorBase>(new binom::NodeVisitor(storage_union.serialized.root_node));
  }
}

QBinOMTreeModel* BinOMFile::getTreeModel() {return &tree_model;}

BinOMFile::File::~File() {}

BinOMFile::File::File(binom::FileType file_type, QString file_path) {
  switch (file_type) {
    case binom::FileType::undefined_file:
    return;
    case binom::FileType::file_storage:
      new(&file.storage) binom::FileStorage(file_path.toStdString());
      new(&file.root_node) binom::FileNodeVisitor(file.storage);
    break;
    case binom::FileType::serialized_file_storage:
      new(&serialized.storage) binom::SerializedStorage(file_path.toStdString());
      new(&serialized.var) binom::Variable(serialized.storage);
      new(&serialized.root_node) binom::NodeVisitor(serialized.var);
    break;
  }
}




bool QBinOM::openFile(QString file_path) {
  fs::path path = file_path.toStdString();
  if(!fs::exists(path)) return false;
  binom::FileType file_type = binom::checkFileType(file_path.toStdString());
  if(file_type == binom::FileType::undefined_file) return false;
  files.emplace(QString::fromStdString(path.filename().string()), std::unique_ptr<BinOMFile>(new BinOMFile(file_type, file_path)));
  emit openFilesChanged(getOpenFiles());
  return true;
}

void QBinOM::closeFile(QString file_name) {
  std::map<QString, std::unique_ptr<BinOMFile>>::iterator it = files.find(file_name);
  if(it == selected_file) {
    selected_file = files.end();
    emit isFileSelectedChanged(selected_file != files.cend());
  }
  if(it != files.cend())
    files.erase(it);
}

bool QBinOM::selectFile(QString file_name) {
  selected_file = files.find(file_name);
  emit isFileSelectedChanged(selected_file != files.cend());
  emit treeModelChanged(getTreeModel());
  return selected_file != files.cend();
}
