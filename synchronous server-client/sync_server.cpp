#include <iostream>
#include <vector>
#include "sync_network.h"

using namespace sync_h;

class server : public sync_network<int>{
private:
    asio::ip::tcp::socket* socket = nullptr;
    asio::ip::tcp::acceptor* acceptor = nullptr;
public:
    server(int loop_flag) : sync_network(loop_flag) {
        socket = new  asio::ip::tcp::socket(io_context);
        acceptor = new asio::ip::tcp::acceptor(io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), PORT));
        acceptor->set_option(asio::ip::tcp::acceptor::reuse_address(true));
    }
    ~server() {
        disconnect();
    }
    template<class packet_type>
    void send_packet(packet_type packet) {
        try {
            socket->write_some(asio::buffer(packet, packet.size()), error);
            if (!error) {
                std::cout << packet.data() << std::endl;       
            }
            else {
                std::cout << "Error on server_send_packet : " << error.message() << std::endl;
            }
        }
        catch (std::exception ex) {
            std::cout << ex.what() << std::endl;
        }
    }
    void disconnect() {
        socket->close();
        delete socket;
        delete acceptor;
    }
    void server_accept() {
        acceptor->accept(*socket, error);
        if (!error) {
            if (socket->is_open()) {
                std::cout << "Someone came! IP:Port > " << socket->remote_endpoint() << std::endl;
            }
        }
        else{
            std::cout << "Error on server_accept :" << error.message() << std::endl;
        }
    }
    void server_listen() {
        try {
            server_accept();
        }
        catch (std::exception ex) {
            std::cout << ex.what() << std::endl;
        }
    }
    void server_read() {
        std::vector<char> new_packet(1024);
        socket->read_some(asio::buffer(new_packet.data(), new_packet.size()), error);
        if (!error) {
            if (new_packet.size() > 0) {
                std::cout << new_packet.data() << std::endl;
                if (sync_network::server_loop == 1) { // When host is supposed to take packets continuously, we call server_read() recursively.
                 /* std::string s1;
                    std::getline(std::cin, s1);
                    send_packet(s1);*/
                    server_read();
                }
            }
        }
        else {
            std::cout << "Error on server_read :" << error.message() << std::endl;
            disconnect();
        }   
    }
};

int main() {
    std::unique_ptr<server> test1(new server(1));
    test1->server_listen();
    test1->server_read();

    return 0;
}
