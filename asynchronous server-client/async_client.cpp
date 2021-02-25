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

class client {
private:
	asio::error_code error;
	asio::io_context io_context;
	asio::ip::tcp::endpoint* endpoint = nullptr;
	std::thread context_thrd;
	std::vector<int8_t> buffer;
public:
	client() {
		endpoint = new asio::ip::tcp::endpoint(asio::ip::make_address("127.0.0.1", error), PORT);
	}
	~client() {
		delete endpoint;
	}
	void client_connect() {
		std::shared_ptr<asio::ip::tcp::socket> socket(new asio::ip::tcp::socket(io_context));
		socket->async_connect(*endpoint, [&](asio::error_code error) {
			if (!error) {
				std::cout << "Connected to : " << socket->local_endpoint() << std::endl;
				client_write(*socket);
			}
			else {
				std::cout << "Error on client_async_connect :" << error.message() << std::endl;
			}
			});
		
		io_context.run();
		std::this_thread::sleep_for(std::chrono::seconds(10));

	}
	void client_write(asio::ip::tcp::socket& socket) {
		//std::vector<char> buffer(1024);
		std::string nick = "Default> ";
		std::string data;
		std::cout << "You> ";
		std::getline(std::cin, data);
	/*	socket.async_read_some(asio::buffer(buffer.data(), buffer.size()), [&](asio::error_code error, std::size_t size) {
			if (!error) {
				std::cout << buffer.data() << std::endl;
				//client_write(socket);
			}
			else {
				std::cout << "Error on client_async_read_some : " << error.message() << std::endl;
			}
			});*/
		 std::string packet = nick + data;
		
		try {
			socket.async_write_some(asio::buffer(packet.data(), packet.size()),
				[=,&socket](std::error_code error, std::size_t size) {
					if (!error) {
						if (size > 0) {
							std::cout << "size = " << packet.size() << packet << std::endl;
							client_write(socket);
						}			
					}
					else {
						std::cout << "Error on client_async_write : " << error.message() << std::endl;
					}
				});
				
		}
		catch (std::exception ex) {
			std::cout << ex.what() << std::endl;
		}
	}
};

int main() {
	client client1;
	client1.client_connect();
	return 0;
}
