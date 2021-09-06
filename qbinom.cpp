#include "qbinom.h"

// BinOMFile & BinOMFile::File

BinOMFile::BinOMFile(binom::FileType file_type, QString file_path)
  : file_type(file_type),
    storage_union(file_type, file_path) {}

BinOMFile::BinOMFile(BinOMFile&& other)
  : file_type(other.file_type),
    storage_union(std::move(other.storage_union)) {
  other.file_type = binom::FileType::undefined_file;
}

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
  binom::FileType file_type = binom::checkFileType(file_path.toStdString()); // checkFileType(std::basic_string_view<char, std::char_traits<char> >) binom::checkFileType(std::basic_string_view<char, std::char_traits<char> >)
  if(file_type == binom::FileType::undefined_file) return false;
  files.emplace(path.filename().string(), BinOMFile(file_type, file_path));
  return true;
}
