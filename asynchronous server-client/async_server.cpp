#define ASIO_STANDALONE

#ifdef _WIN32
#define _WIN32_WINNT 0x0A00
#endif

#include <asio.hpp>
#include <asio/ts/buffer.hpp>
#include <vector>
#include <cstdint>
#include <chrono>
#include <iostream>
#include <fstream>

#define PORT 8080
#define LOCAL_ADDRESS "127.0.0.7"
#define REMOTE_ADDRESS "0.0.0.0"

template<typename server_handler>
class server : public std::enable_shared_from_this<server_handler> {
private:
    asio::error_code error;
    asio::io_context io_context;
    std::unique_ptr<asio::ip::tcp::acceptor> acceptor{ nullptr };
    std::vector<std::shared_ptr<asio::ip::tcp::socket>> clients;
    std::vector<std::unique_ptr<std::thread>> context_threads;
    int client_number = 0;
    int server_type = 0; // default 0 for server-client, 1 for web-server
public:
    server() {

    }
    ~server() {
        server_stop();

    }
    void server_start() {
        acceptor = std::make_unique<asio::ip::tcp::acceptor>(asio::ip::tcp::acceptor(io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), PORT)));
        acceptor->set_option(asio::ip::tcp::acceptor::reuse_address(true));
        acceptor->listen();
        server_accept();
    }
    void server_stop() {
        clients.clear();
        io_context.stop();
    }
    void set_server_type(int type) {
        server_type = type;
    }
    virtual void server_accept() {
        std::shared_ptr<asio::ip::tcp::socket> socket(new asio::ip::tcp::socket(io_context));
        clients.push_back(socket);
        client_number++;
        acceptor->async_accept([=](std::error_code ec, asio::ip::tcp::socket socket) {
            if (!ec) {
                std::cout << "Someone came! IP:Port > " << socket.remote_endpoint() << std::endl;
                server_read(socket);
                server_accept();
            }
            else {
                std::cout << "Error on server_accept :" << error.message() << std::endl;
            }
            });
        //  context_threads.emplace_back([this] {  io_context.run(); });
        io_context.run();
    }
    void server_read(asio::ip::tcp::socket& socket) {
        std::vector<int8_t> buffer(1024);
        socket.async_read_some(asio::buffer(buffer.data(), buffer.size()),
            [=, &socket](std::error_code error, std::size_t size) {
                if (!error) {
                    if (size > 0) {
                        std::cout << buffer.data() << std::endl;
                        //       std::cout << "|||| Packet size = " << buffer.size() << "-" << size  << std::endl;

                              // std::this_thread::sleep_for(std::chrono::seconds(1));
                        server_read(socket);
                    }
                }
                else {
                    std::cout << "Error on server_read :" << error.message() << std::endl;
                }
                //   std::this_thread::sleep_for(std::chrono::seconds(1));

            });
    }
    std::vector<std::shared_ptr<asio::ip::tcp::socket>> get_clients() {
        return clients;
    }

};

/*int main(){
    server<int> test1;
    test1.server_start();

    return 0;
}*/
