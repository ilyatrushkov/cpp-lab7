// Copyright 2022 Trushkov Ilya ilya.tr20002@gmail.com

#ifndef TEMPLATE_SUGGESTIONS_HPP
#define TEMPLATE_SUGGESTIONS_HPP

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

#endif  // TEMPLATE_SUGGESTIONS_HPP
