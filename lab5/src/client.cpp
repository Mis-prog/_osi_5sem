#include <iostream>
#include <boost/asio.hpp>
#include <string>
#include "src_lib/calculation.h"
#include "json.hpp"

using json = nlohmann::json;
using namespace boost::asio;
using ip::tcp;
using std::string;
using std::cin;
using std::cout;
using std::cerr;
using std::endl;

struct User {
    string username;
    string password;
};


void set_name(struct User &user);

int main() {
    io_service service;
    ip::tcp::socket sock(service);
    ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), 8001);
    sock.connect(ep);

    User user;
    set_name(user);
    json userJson = {
            {"username", user.username},
            {"password", user.password}
    };

    boost::system::error_code error;
    write(sock, buffer(userJson.dump()), error);

    boost::asio::streambuf receive_buffer;
    boost::asio::read(sock, receive_buffer, boost::asio::transfer_all(), error);
    const char *data = boost::asio::buffer_cast<const char *>(receive_buffer.data());

    if (strcmp(data, "true") == 0) {
        cout << 1 << endl;
    } else {
        cout << 2 << endl;
    }
    sock.close();
    return 0;
}

void set_name(struct User &user) {
    cout << "Введите имя:\n";
    cin >> user.username;
    cout << "Введите пароль:\n";
    cin >> user.password;
}