#pragma once
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

namespace sync_h {

#define PORT 8080
#define LOCAL_ADDRESS "127.0.0.1"
#define REMOTE_ADDRESS "0.0.0.0"

	template<class packet_type>
	class sync_network {
	protected:
		std::string remoteAddress;
		std::vector<int8_t> packets;
		asio::error_code error;
		asio::io_context io_context;
		int server_loop = 0;
		int packet_flag = 0;
	public:
		sync_network(int server_loop) : server_loop(server_loop) {}
		virtual void send_packet(packet_type packet) { // We declare that any type of packet can be sent and received.
			if (packet) {
				packet_flag = 1;
			}
		}
		virtual void disconnect() { }
	};
}