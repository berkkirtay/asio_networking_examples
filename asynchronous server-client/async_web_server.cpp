#include "async_server.cpp"

using server_handler = std::enable_shared_from_this<int>;

class web_server : public server<server_handler>{ //public server<std::enable_shared_from_this<server_handler>>
private:
    asio::error_code error;
    asio::io_context io_context;
    std::unique_ptr<asio::ip::tcp::acceptor> acceptor{ nullptr };
    std::string new_packet;
public:
    web_server() : server() {
        server<server_handler>::set_server_type(1);
    }
    ~web_server() {
        server<server_handler>::server_stop();
    }
    void server_accept() {
        std::shared_ptr<asio::ip::tcp::socket> socket(new asio::ip::tcp::socket(io_context));
        get_clients().push_back(socket);

        acceptor->async_accept([=](std::error_code ec, asio::ip::tcp::socket socket) {
            if (!ec) {
                std::cout << "Someone came! IP:Port > " << socket.remote_endpoint() << std::endl;
                web_server_update(socket);
                server_accept();
            }
            else {
                std::cout << "Error on server_accept :" << error.message() << std::endl;
            }
            });
        io_context.run();
    }
    void web_server_update(asio::ip::tcp::socket& socket) {
        std::string test = "HTTP / 1.0 200 OK\r\nContent - Length: 45\r\n\r\n<html><body><b>Test Async Web Server!<br /></i></body></html>";
        asio::async_write(socket, asio::buffer(test.data(), test.size()),
            [&, this](std::error_code error, std::size_t size) {
                if (!error) {
                    std::cout << socket.remote_endpoint() << ": async_write successful!" << std::endl;
                    // web_server_update(socket);
                }
                else {
                    std::cout << "Error : " << error.message() << std::endl;
                }
                //  std::this_thread::sleep_for(std::chrono::seconds(1));
            });
    }
    void packet_input() {
        std::ifstream PACKET("text.html");
        if (PACKET) {
            std::stringstream buffer;
            buffer << PACKET.rdbuf();
            new_packet = buffer.str();
        }
        else {
            std::cout << "Packet read error!" << std::endl;
        }
    }
    void start_web_server() {
        packet_input();
        std::string test = "HTTP / 1.0 200 OK\r\nContent - Length: 1000";
        new_packet = test + new_packet;
        acceptor = std::make_unique<asio::ip::tcp::acceptor>(asio::ip::tcp::acceptor(io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), PORT)));
        acceptor->set_option(asio::ip::tcp::acceptor::reuse_address(true));
        acceptor->listen();
        server_accept();
    }
};
int main() {
    web_server test_server;
    test_server.start_web_server();
}