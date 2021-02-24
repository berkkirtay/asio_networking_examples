#include "sync_server.cpp"
using namespace sync_h;

class web_server : public server {
private:
   // std::vector<int8_t> new_packet;
    std::string new_packet;
public:
	web_server(int loop_flag) : server(loop_flag) {
          
	}
	~web_server() {

	}
    void start_web_server() {
        packet_input();
        std::string test = "HTTP / 1.0 200 OK\r\nContent - Length: 1000";
        send_packet(test);
       do {
            try {
                send_packet("\r\n\r\n" + new_packet);
            }
            catch (std::exception ex) {
                std::cout << ex.what() << std::endl;
                break;
            }
            std::this_thread::sleep_for(std::chrono::seconds(1));
       } while (sync_network::server_loop == 1);
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
};

int main() {
    web_server test_server(1);
    test_server.server_listen();
    test_server.start_web_server();
}
