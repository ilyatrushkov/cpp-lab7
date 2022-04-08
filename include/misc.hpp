// Copyright 2022 Trushkov Ilya ilya.tr20002@gmail.com

#ifndef TEMPLATE_MISC_HPP
#define TEMPLATE_MISC_HPP

#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <mutex>
#include <thread>
#include <nlohmann/json.hpp>

#include "Storage.hpp"
#include "Suggestions.hpp"

std::string make_json(const nlohmann::json& data);

void suggestion_updater(const std::shared_ptr<Storage> &storage,
    const std::shared_ptr<Suggestions> &suggestions,
    const std::shared_ptr<std::timed_mutex> &mutex);

#endif  // TEMPLATE_MISC_HPP
