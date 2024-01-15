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
        while (true) {
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

                streambuf buf_task1_data;
                read_until(sock, buf_task1_data, '\n');
                istream is_task1_data(&buf_task1_data);
                string message_data;
                is_task1_data >> message_data;
                nlohmann::json parsedJson = nlohmann::json::parse(message_data);
                std::cout << parsedJson.dump(4) << std::endl;
            }else if (data_message == "2") {
                streambuf buf_task2_data;
                read_until(sock, buf_task2_data, '\n');
                istream is_task1_data(&buf_task2_data);
                string message_data;
                is_task1_data >> message_data;
                nlohmann::json parsedJson = nlohmann::json::parse(message_data);
                std::cout << parsedJson.dump(7) << std::endl;
            }else{
                break;
            }
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