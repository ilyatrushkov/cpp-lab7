// Copyright 2022 Trushkov Ilya ilya.tr20002@gmail.com

#ifndef TEMPLATE_STORAGE_HPP
#define TEMPLATE_STORAGE_HPP

#include <string>
#include <nlohmann/json.hpp>

class Storage {
 private:
  std::string _path_to_file;
  nlohmann::json _storage;

 public:
  explicit Storage(const std::string filename);
  nlohmann::json get_storage();
};

#endif  // TEMPLATE_STORAGE_HPP
