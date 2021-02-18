#include <iostream>
#include <vector>

#include "sync_network.h"

using namespace sync_h;

class client : public sync_network<int> {
private:
	asio::ip::tcp::socket* socket = nullptr;
	asio::ip::tcp::endpoint* endpoint = nullptr;
	std::string nick = "default > ";
public:
	client(int loop_flag) : sync_network(loop_flag) {
		socket = new asio::ip::tcp::socket(io_context);
		endpoint = new asio::ip::tcp::endpoint(asio::ip::make_address(LOCAL_ADDRESS, error), PORT);
	}
	~client() {
		disconnect();
	}
	void client_update() {
		do{
			client_write();
		} while (sync_network::server_loop == 1);
	}
	void send_packet(std::string packet) {
		try {
			socket->write_some(asio::buffer(packet, packet.size()), error);
			if (!error) {
				std::cout <<  packet << std::endl;
			}
			else {
				std::cout << "Error on client_send_packet : " << error.message() << std::endl;
			}
		}
		catch (std::exception ex) {
			std::cout << ex.what() << std::endl;
		}
	}
	void disconnect() {
		socket->close();
		delete socket;
		delete endpoint;
	}
	void client_connect(){
		socket->connect(*endpoint, error);
		if (!error) {
			if (socket->is_open()) {
				std::cout << "Connected to : " << socket->local_endpoint() << std::endl;
			}
		}
		else {
			std::cout << "Error on client_connect :" << error.message() << std::endl;
			disconnect();
		}
	}
	void client_write() {
		std::string data;
		std::cout << "You> ";
		std::getline(std::cin, data);
		if (data.compare("exit") == 0) {
			server_loop = 0;
		}
		send_packet(nick + data);
	}
};

int main() {
	client client1(1);
	client1.client_connect();
	client1.client_update();
	return 0;
}