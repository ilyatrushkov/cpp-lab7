// Copyright 2022 Trushkov Ilya ilya.tr20002@gmail.com

#include "misc.hpp"

std::string make_json(const nlohmann::json& data) {
  std::stringstream ss;
  if (data.is_null())
    ss << "No suggestions";
  else
    ss << std::setw(4) << data;
  return ss.str();
}

void suggestion_updater(const std::shared_ptr<Storage> &storage,
                        const std::shared_ptr<Suggestions> &suggestions,
                        const std::shared_ptr<std::timed_mutex> &mutex) {
  using std::chrono_literals::operator""min;
  for (;;) {
    // WRITE - блокировка
    mutex->lock();
//    storage->Load();
    suggestions->set_collection(storage->get_storage());
    mutex->unlock();
    std::cout << "Suggestions updated" << std::endl;
    std::this_thread::sleep_for(15min);
  }
}
