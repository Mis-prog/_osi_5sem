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
using std::istream;


void set_name(json &user);

int main() {
    json test;
    test={{0.01:{time:12}}};
    io_service service;
    ip::tcp::socket sock(service);
    ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), 8001);
    sock.connect(ep);

    json user;
    set_name(user);

    write(sock, buffer(user.dump() + '\n'));

    streambuf buf;
    read_until(sock, buf, '\n');
    istream is(&buf);
    json message;
    is >> message;

    if (message["message"] == "true") {
        string data_message;
        cout << "Введите номер задания (1,2)\n";
        cin >> data_message;
        message.clear();
        message["command"] = data_message;
        write(sock, buffer(message.dump() + '\n'));
        if (data_message == "1") {
            streambuf buf;
            read_until(sock, buf, '\n');
            istream is(&buf);
            json message;
            is >> message;
            cout << message << endl;

            double x, y, z;
            cin >> x >> y >> z;
            json data;
            data["x"] = x;
            data["y"] = y;
            data["z"] = z;
//            cout << data << endl;
            write(sock, buffer(data.dump() + '\n'));
        }
        if (data_message == "2") {

        }

    } else {
        cout << message << endl;
        sock.close();
        return 0;
    }

    sock.close();

    return 0;
}

void set_name(json &user) {
    string data;
    cout << "Введите имя:\n";
    cin >> data;
    user["username"] = data;
    cout << "Введите пароль:\n";
    data = "";
    cin >> data;
    user["password"] = data;
}