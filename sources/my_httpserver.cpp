// Copyright 2022 Trushkov Ilya ilya.tr20002@gmail.com

#include "my_httpserver.hpp"

// Report a failure
void fail(beast::error_code ec, char const* what) {
  std::cerr << what << ": " << ec.message() << "\n";
}

// Handles an HTTP server connection
void do_session(boost::asio::ip::tcp::socket &socket,
                const std::shared_ptr<Suggestions> &collection,
                const std::shared_ptr<std::timed_mutex> &mutex) {
  bool close = false;
  beast::error_code ec;

  // Этот буфер необходим для сохранения во время чтения
  beast::flat_buffer buffer;

  // Эта лямбда используется для отправки сообщений
  send_lambda<boost::asio::ip::tcp::socket> lambda{socket, close, ec};
  for (;;) {
    // Чтение запроса
    http::request<http::string_body> req;
    http::read(socket, buffer, req, ec);
    if (ec == http::error::end_of_stream) break;
    if (ec) return fail(ec, "read");

    // Отправка ответа
    handle_request(mutex, collection, std::move(req), lambda);
    if (ec) return fail(ec, "write");
  }

  // Отправка TCP-сокета завершающего соединение
  socket.shutdown(boost::asio::ip::tcp::socket::shutdown_send, ec);
  // В этот момент соединение корректно закрывается
}

int server_working(int argc, char *argv[]) {
  std::shared_ptr<std::timed_mutex> mutex =
      std::make_shared<std::timed_mutex>();
  std::shared_ptr<Storage> storage = std::make_shared<Storage>(
      "./suggestions.json");
  std::shared_ptr<Suggestions> suggestions =
      std::make_shared<Suggestions>();
  try {
    // Проверка аргументов командной строки
    if (argc != 3) {
      std::cerr << "Usage: suggestion_server <address> <port>\n"
                << "Example:\n"
                << "    http-server-sync 0.0.0.0 8080\n";
      return EXIT_FAILURE;
    }
    auto const address = net::ip::make_address(argv[1]);
    auto const port = static_cast<uint16_t>(std::atoi(argv[2]));

    // io_context требуется для всех операций ввода-вывода
    net::io_context ioc{1};

    // Акцептор принимает входящие соединения
    boost::asio::ip::tcp::acceptor acceptor{ioc, {address, port}};

    // Выделяем поток, который будет обновлять коллекцию
    // и отсоединяем его, так как он не будет возвращать данные для работы
    std::thread{suggestion_updater, storage, suggestions, mutex}.detach();
    for (;;) {
      // Создаем новое соединение
      boost::asio::ip::tcp::socket socket{ioc};

      // Блокируем, пока мы не получим соединение
      acceptor.accept(socket);

      // Запустите сеанс, передав право собственности на сокет
      std::thread{std::bind(&do_session, std::move(socket), suggestions, mutex)}
          .detach();
    }
  } catch (std::exception& e) {
    std::cerr << e.what() << '\n';
    return EXIT_FAILURE;
  }
}
