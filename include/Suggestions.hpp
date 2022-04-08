// Copyright 2022 Trushkov Ilya ilya.tr20002@gmail.com

#ifndef INCLUDE_SUGGESTIONS_HPP_
#define INCLUDE_SUGGESTIONS_HPP_

#include <string>
#include <nlohmann/json.hpp>

class Suggestions {
 private:
  nlohmann::json _collection = {};

 public:
  Suggestions() = default;
  void set_collection(nlohmann::json storage);
  nlohmann::json suggest(const std::string &input);
};

#endif  // INCLUDE_SUGGESTIONS_HPP_
