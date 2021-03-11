#include "async_server.cpp"

using server_handler = std::enable_shared_from_this<int>;

class web_server : public server<server_handler>{ //public server<std::enable_shared_from_this<server_handler>>
private:
    asio::error_code error;
    asio::io_context io_context;
    std::unique_ptr<asio::ip::tcp::acceptor> acceptor{ nullptr };
    std::vector<std::unique_ptr<std::thread>> context_threads;
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
        std::ostringstream ostr;
        ostr << "HTTP/1.0 " << 200 << " OK\r\n";
        ostr << "Content-Type: text/html\r\n";
        ostr << "Content-Length: " << new_packet.size() << "\r\n\r\n" << new_packet;
        std::string test = ostr.str();
        web_server_write(socket, test);
    }
    void web_server_write(asio::ip::tcp::socket& socket, std::string packet) {
        asio::async_write(socket, asio::buffer(packet.data(), packet.size()),
            [&, this](std::error_code error, std::size_t size) {
                if (!error) {
                    std::cout << socket.remote_endpoint() << ": async_write successful!" << std::endl;
                    // web_server_update(socket);
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));  
                    // After writing operation is complete, we can close the socket in order to prevent continuously writing to the client.
                    socket.close();
                }
                else {
                    std::cout << "Error : " << error.message() << std::endl;
                }
            });
    }
    // With packet_input we will get input source from the local computer.
    void packet_input() {
        std::ifstream PACKET("test.html");
        if (PACKET) {
            std::stringstream buffer;
            buffer << PACKET.rdbuf();
            new_packet = buffer.str();
        }
        else {
            std::cout << "Packet read error!" << std::endl;
        }
        PACKET.close();
    }
    void start_web_server() {
        packet_input();
        acceptor = std::make_unique<asio::ip::tcp::acceptor>(asio::ip::tcp::acceptor(io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), PORT)));
        acceptor->set_option(asio::ip::tcp::acceptor::reuse_address(true));
        acceptor->listen();
        server_accept();
    }
};
int main() {
    web_server test_server;
    test_server.start_web_server();
    return 0;
}