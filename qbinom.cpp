#include "qbinom.h"

// BinOMFile & BinOMFile::File

binom::VarType toVarType(QString type_string) {
#define RET_TYPE(var_type) std::pair<QString, binom::VarType>{#var_type, binom::VarType::var_type}
  static const QMap<QString, binom::VarType> str_switch = {
    RET_TYPE(byte),
    RET_TYPE(word),
    RET_TYPE(dword),
    RET_TYPE(qword),
    RET_TYPE(byte_array),
    RET_TYPE(word_array),
    RET_TYPE(dword_array),
    RET_TYPE(qword_array),
    RET_TYPE(array),
    RET_TYPE(object)
  };
#undef RET_TYPE
  if(!str_switch.contains(type_string))
    return binom::VarType::invalid_type;
  return str_switch[std::move(type_string)];
}



BinOMFile::BinOMFile(binom::FileType file_type, QString file_path)
  : file_type(file_type),
    storage_union(file_type, file_path),
    model(getRoot()) {}

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




QBinOM::QBinOM()
  : QObject(nullptr),
//    config(
//      settings_path.toStdString() + "/binom_manager_config.binomdb",
//      binom::vobj{{"file_history", binom::varr{}}}
//      ),
    selected_file(files.end()) {
//  binom::ui64 index = 0;
//  binom::FileNodeVisitor history = config.getRoot()("file_history");
//  for(binom::FileNodeVisitor file : history) {
//    if(!fs::exists(file["path"].getVariable().toBufferArray().toString()))
//      history.remove(index);
//    ++index;
//  }
}

bool QBinOM::openFile(QString file_path) {
//  binom::FileNodeVisitor history = config.getRoot()("file_history");
  fs::path path = file_path.toStdString();
  if(!fs::exists(path)) return false;
  binom::FileType file_type = binom::checkFileType(file_path.toStdString());
  if(file_type == binom::FileType::undefined_file) return false;
  QString file_name = QString::fromStdString(path.filename().string());
  files.emplace(file_name, std::unique_ptr<BinOMFile>(new BinOMFile(file_type, file_path)));
  selectFile(std::move(file_name));
//  history.pushBack(binom::vobj{{"path", path.string()}});
  emit openFilesChanged(getOpenFiles());
//  emit historyChanged(getHistory());
  return true;
}

bool QBinOM::createFile(QString file_path, QJSValue value, QString expected_root_type) {
  std::string file_path_std = file_path.toStdString();
  std::string file_extension = file_path_std.substr(file_path_std.find_last_of(".") + 1);
  binom::FileType file_type = (file_extension == "binom")
                              ? binom::FileType::serialized_file_storage
                              : (file_extension == "binomdb")
                                ? binom::FileType::file_storage
                                : binom::FileType::undefined_file;
  if(file_type == binom::FileType::undefined_file)
    return false;

  binom::Variable var_value = tryConvert(value.toVariant(), toVarType(expected_root_type));
  if(var_value.isNull()) return false;

  switch (file_type) {
    default: return false;
    case binom::FileType::serialized_file_storage: {
      binom::SerializedStorage storage(file_path_std);
      storage.write(var_value);
    } break;
    case binom::FileType::file_storage: {
      binom::FileStorage storage(file_path_std, var_value, true);
    } break;
  }

  return openFile(file_path);
}

void QBinOM::closeFile(QString file_name) {
  std::map<QString, std::unique_ptr<BinOMFile>>::iterator it = files.find(file_name);
  if(it == selected_file) {
    selected_file = files.end();
    emit isFileSelectedChanged(selected_file != files.cend());
    emit fileTypeChanged(getFileType());
    emit fileNameChanged(getFileName());
    emit treeModelChanged(QVariantList());
  }
  if(it != files.cend())
    files.erase(it);
}

bool QBinOM::selectFile(QString file_name) {
  selected_file = files.find(file_name);
  emit isFileSelectedChanged(selected_file != files.cend());
  emit treeModelChanged(getTreeModel());
  emit fileTypeChanged(getFileType());
  emit fileNameChanged(getFileName());
  return selected_file != files.cend();
}

binom::Variable QBinOM::tryConvert(QVariant value, binom::VarType type) {
  switch (type) {
    case binom::VarType::byte:
      switch (value.type()) {
        case QVariant::Bool:
        return binom::Variable(static_cast<binom::ui8>(value.toBool()));
        case QVariant::Int:
        return binom::Variable(static_cast<binom::i8>(value.toInt()));
        case QVariant::UInt:
        return binom::Variable(static_cast<binom::ui8>(value.toUInt()));
        case QVariant::LongLong:
        return binom::Variable(static_cast<binom::i8>(value.toLongLong()));
        case QVariant::ULongLong:
        return binom::Variable(static_cast<binom::ui8>(value.toULongLong()));
        case QVariant::Double:
        return binom::Variable(static_cast<binom::i8>(value.toDouble()));
        case QVariant::Char:
        return binom::Variable(static_cast<binom::i8>(value.toChar().unicode()));
        default: return binom::Variable();
      }

    case binom::VarType::word:
      switch (value.type()) {
        case QVariant::Bool:
        return binom::Variable(static_cast<binom::ui16>(value.toBool()));
        case QVariant::Int:
        return binom::Variable(static_cast<binom::i16>(value.toInt()));
        case QVariant::UInt:
        return binom::Variable(static_cast<binom::ui16>(value.toUInt()));
        case QVariant::LongLong:
        return binom::Variable(static_cast<binom::i16>(value.toLongLong()));
        case QVariant::ULongLong:
        return binom::Variable(static_cast<binom::ui16>(value.toULongLong()));
        case QVariant::Double:
        return binom::Variable(static_cast<binom::i16>(value.toDouble()));
        case QVariant::Char:
        return binom::Variable(static_cast<binom::i16>(value.toChar().unicode()));
        default: return binom::Variable();
      }

    case binom::VarType::dword:
      switch (value.type()) {
        case QVariant::Bool:
        return binom::Variable(static_cast<binom::ui32>(value.toBool()));
        case QVariant::Int:
        return binom::Variable(static_cast<binom::i32>(value.toInt()));
        case QVariant::UInt:
        return binom::Variable(static_cast<binom::ui32>(value.toUInt()));
        case QVariant::LongLong:
        return binom::Variable(static_cast<binom::i32>(value.toLongLong()));
        case QVariant::ULongLong:
        return binom::Variable(static_cast<binom::ui32>(value.toULongLong()));
        case QVariant::Double:
        return binom::Variable(static_cast<binom::f32>(value.toDouble()));
        case QVariant::Char:
        return binom::Variable(static_cast<binom::i32>(value.toChar().unicode()));
        default: return binom::Variable();
      }

    case binom::VarType::qword:
      switch (value.type()) {
        case QVariant::Bool:
        return binom::Variable(static_cast<binom::ui64>(value.toBool()));
        case QVariant::Int:
        return binom::Variable(static_cast<binom::i64>(value.toInt()));
        case QVariant::UInt:
        return binom::Variable(static_cast<binom::ui64>(value.toUInt()));
        case QVariant::LongLong:
        return binom::Variable(static_cast<binom::i64>(value.toLongLong()));
        case QVariant::ULongLong:
        return binom::Variable(static_cast<binom::ui64>(value.toULongLong()));
        case QVariant::Double:
        return binom::Variable(static_cast<binom::f64>(value.toDouble()));
        case QVariant::Char:
        return binom::Variable(static_cast<binom::i64>(value.toChar().unicode()));
        default: return binom::Variable();
      }

    case binom::VarType::byte_array: {
      switch (value.type()) {
        case QVariant::List:{
          binom::BufferArray data(binom::VarType::byte_array);
          for(auto element : value.toList()) {
            binom::Variable var_element = tryConvert(element, binom::VarType::byte);
            if(var_element.isNull()) return binom::Variable();
            data.pushBack(var_element.getValue().asUi64());
          }
          return data;
        }
        case QVariant::String:
        return binom::Variable(value.toString().toStdString());

        case QVariant::StringList:{
          binom::BufferArray data(binom::VarType::byte_array);
          for(auto element : value.toStringList()) {
            binom::Variable var_element(element.toStdString());
            data.pushBack(var_element.toBufferArray());
          }
          return data;
        }

        case QVariant::ByteArray: {
          binom::ByteArray data(value.toByteArray().length());
          memcpy(data.begin(), value.toByteArray().data(), data.length());
          return binom::Variable(data);
        }
        default: return binom::Variable();
      }
    }

    case binom::VarType::word_array:
      switch (value.type()) {
        case QVariant::List:{
          binom::BufferArray data(binom::VarType::word_array);
          for(auto element : value.toList()) {
            binom::Variable var_element = tryConvert(element, binom::VarType::word);
            if(var_element.isNull()) return binom::Variable();
            data.pushBack(var_element.getValue().asUi64());
          }
          return data;
        }
        default: return binom::Variable();
      }


    case binom::VarType::dword_array:
      switch (value.type()) {
        case QVariant::List:{
          binom::BufferArray data(binom::VarType::dword_array);
          for(auto element : value.toList()) {
            binom::Variable var_element = tryConvert(element, binom::VarType::dword);
            if(var_element.isNull()) return binom::Variable();
            data.pushBack(var_element.getValue().asUi64());
          }
          return data;
        }
        default: return binom::Variable();
      }

    case binom::VarType::qword_array:
      switch (value.type()) {
        case QVariant::List:{
          binom::BufferArray data(binom::VarType::qword_array);
          for(auto element : value.toList()) {
            binom::Variable var_element = tryConvert(element, binom::VarType::qword);
            if(var_element.isNull()) return binom::Variable();
            data.pushBack(var_element.getValue().asUi64());
          }
          return data;
        }
        default: return binom::Variable();
      }

    case binom::VarType::array:
      switch (value.type()) {
        case QVariant::List:{
          binom::Array data;
          for(auto element : value.toList()) {
            if(element.type() != QVariant::Map) return binom::Variable();
            QVariantMap map_element = element.toMap();
            if(!map_element.contains("type") || !map_element.contains("value")) return binom::Variable();
            if(map_element["type"].type() != QVariant::String) return binom::Variable();
            binom::Variable var_element = tryConvert(map_element["value"], toVarType(map_element["type"].toString()));
            if(var_element.isNull()) return binom::Variable();
            data.pushBack(var_element);
          }
          return data;
        }
        default: return binom::Variable();
      }

    case binom::VarType::object:
      switch (value.type()) {
        case QVariant::List: {
          binom::Object data;
          for(auto element : value.toList()) {
            if(element.type() != QVariant::Map) return binom::Variable();
            QVariantMap map_element = element.toMap();
            if(!map_element.contains("type") || !map_element.contains("value") ||
               !map_element.contains("key_type") || !map_element.contains("key"))
              return binom::Variable();
            if(map_element["type"].type() != QVariant::String ||
               map_element["key_type"].type() != QVariant::String)
              return binom::Variable();
            binom::VarType name_type = toVarType(map_element["key_type"].toString());
            if(binom::toTypeClass(name_type) != binom::VarTypeClass::buffer_array)
              return binom::Variable();
            binom::Variable var_key = tryConvert(map_element["key"], name_type);
            binom::Variable var_element = tryConvert(map_element["value"], toVarType(map_element["type"].toString()));
            if(var_element.isNull() || var_key.isNull()) return binom::Variable();
            data.insert(var_key.toBufferArray(), var_element);
          }
          return data;
        }
        default: return binom::Variable();
      }
    break;
    default: return binom::Variable();
  }
}

bool QBinOM::setNode(QString path, QJSValue value, QString expected_type_str) {
  binom::Variable var_value = tryConvert(value.toVariant(), toVarType(expected_type_str));
  if(var_value.isNull()) return false;
  auto node = selected_file->second->getRoot();
  if(node->isNull()) return false;
  (*node)(path.isEmpty()? binom::Path() : binom::Path::fromString(path.toStdString())).setVariable(var_value);
  emit treeModelChanged(getTreeModel());
  return true;
}

QVariantList QBinOM::getHistory() {
  return QVariantList();
//  binom::FileNodeVisitor history = config.getRoot()("file_history");
//  QVariantList list;
//  for(binom::FileNodeVisitor file : history) {
//    list.push_back(QVariantMap{
//                     {"path", QString::fromStdString(file["path"].getVariable().toBufferArray())}
//                   });
//  }
//  return list;
}
