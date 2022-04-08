// Copyright 2022 Trushkov Ilya ilya.tr20002@gmail.com

#include <fstream>
#include "Storage.hpp"

Storage::Storage(const std::string filename)
    : _path_to_file(filename) {
  std::ifstream in(_path_to_file);
  in >> _storage;
}

nlohmann::json Storage::get_storage() {
  return _storage;
}