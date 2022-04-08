// Copyright 2022 Trushkov Ilya ilya.tr20002@gmail.com

#include "Suggestions.hpp"

void Suggestions::set_collection(nlohmann::json storage) {
  std::sort(storage.begin(), storage.end(),
            [](const nlohmann::json& a, const nlohmann::json& b) -> bool {
              return a.at("cost") < b.at("cost");
            });
  _collection = storage;
}

nlohmann::json Suggestions::suggest(const std::string &input) {
  nlohmann::json result;
  for (size_t i = 0, m = 0; i < _collection.size(); ++i) {
    if (input == _collection[i].at("id")) {
      nlohmann::json pos;
      pos["text"] = _collection[i].at("name");
      pos["position"] = m++;
      result["suggestions"].push_back(pos);
    }
  }
  return result;
}