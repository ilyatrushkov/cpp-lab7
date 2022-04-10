// Copyright 2022 Trushkov Ilya ilya.tr20002@gmail.com

#ifndef INCLUDE_STORAGE_HPP_
#define INCLUDE_STORAGE_HPP_

#include <string>
#include <nlohmann/json.hpp>

class Storage {
 private:
  std::string _path_to_file;
  nlohmann::json _storage;

 public:
  explicit Storage(const std::string &filename);
  nlohmann::json get_storage();
};

#endif  // INCLUDE_STORAGE_HPP_
