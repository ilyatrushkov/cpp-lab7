// Copyright 2022 Trushkov Ilya ilya.tr20002@gmail.com

#ifndef INCLUDE_MY_HTTPSERVER_HPP_
#define INCLUDE_MY_HTTPSERVER_HPP_
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/config.hpp>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <mutex>
#include <utility>
#include <nlohmann/json.hpp>

#include "Suggestions.hpp"
#include "Storage.hpp"
#include "misc.hpp"

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using json = nlohmann::json;
//using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

// This function produces an HTTP response for the given
// request. The type of the response object depends on the
// contents of the request, so the interface requires the
// caller to pass a generic lambda for receiving the response.
template<class Body, class Allocator, class Send>
void handle_request(const std::shared_ptr<std::timed_mutex> &mutex,
    const std::shared_ptr<Suggestions> &collection,
    http::request<Body, http::basic_fields<Allocator>> &&req, Send &&send) {
  // Returns a bad request response
  auto const bad_request =
      [&req](beast::string_view why)
  {
    http::response<http::string_body> res{http::status::bad_request,
                                          req.version()};
    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(http::field::content_type, "text/html");
    res.keep_alive(req.keep_alive());
    res.body() = std::string(why);
    res.prepare_payload();
    return res;
  };

  // Returns a not found response
  auto const not_found =
      [&req](beast::string_view target)
  {
    http::response<http::string_body> res{http::status::not_found,
                                          req.version()};
    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(http::field::content_type, "text/html");
    res.keep_alive(req.keep_alive());
    res.body() = "The resource '" + std::string(target) + "' was not found.";
    res.prepare_payload();
    return res;
  };

  // Make sure we can handle the method
  if (req.method() != http::verb::post &&
      req.method() != http::verb::get &&
      req.method() != http::verb::head)
    return send(bad_request("Unknown HTTP-method"));

  // Request path must be correct: "/v1/api/suggest"
  if (req.target() != "/v1/api/suggest") {
    return send(not_found(req.target()));
  }

//+++++++
  // Парсим данные из тела запроса
  json input_body;
  try {
    input_body = json::parse(req.body());
  } catch (std::exception& e) {
    return send(bad_request(e.what()));
  }
  boost::optional<std::string> input;
  try {
    input = input_body.at("input").get<std::string>();
  } catch (std::exception& e) {
    return send(bad_request(R"(JSON format: {"input": "<user_input>"}!)"));
  }
  if (!input.has_value()) {
    return send(bad_request(R"(JSON format: {"input": "<user_input>"}!)"));
  }

  // Формируем тело ответа - suggestion

  // READ - блокировка
  mutex->lock();
  auto result = collection->suggest(input.value());
  mutex->unlock();
  http::string_body::value_type body = make_json(result);
  auto const size = body.size();
//+++++++

  // Respond to HEAD request
  if (req.method() == http::verb::head) {
    http::response<http::empty_body> res{http::status::ok, req.version()};
    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(http::field::content_type, "application/json");
    res.content_length(size);
    res.keep_alive(req.keep_alive());
    return send(std::move(res));
  }

  // Respond to GET request
  http::response<http::file_body> res{
      std::piecewise_construct,
      std::make_tuple(std::move(body)),
      std::make_tuple(http::status::ok, req.version())};
  res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
  res.set(http::field::content_type, "application/json");
  res.content_length(size);
  res.keep_alive(req.keep_alive());
  return send(std::move(res));
}

// This is the C++11 equivalent of a generic lambda.
// The function object is used to send an HTTP message.
template<class Stream>
struct send_lambda {
  Stream& stream_;
  bool& close_;
  beast::error_code& ec_;

  explicit send_lambda(Stream& stream, bool& close, beast::error_code& ec)
      : stream_(stream)
        , close_(close)
        , ec_(ec) {}

  template<bool isRequest, class Body, class Fields>
  void operator()(http::message<isRequest, Body, Fields>&& msg) const {
    // Determine if we should close the connection after
    close_ = msg.need_eof();

    // We need the serializer here because the serializer requires
    // a non-const file_body, and the message oriented version of
    // http::write only works with const messages.
    http::serializer<isRequest, Body, Fields> sr{msg};
    http::write(stream_, sr, ec_);
  }
};

// Report a failure
void fail(beast::error_code ec, char const* what);

// Handles an HTTP server connection
void do_session(boost::asio::ip::tcp::socket& socket,
                const std::shared_ptr<Suggestions> &collection,
                const std::shared_ptr<std::timed_mutex> &mutex);

int server_working(int argc, char *argv[]);

#endif  // INCLUDE_MY_HTTPSERVER_HPP_
