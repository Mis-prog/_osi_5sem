#include <iostream>
#include <boost/asio.hpp>
#include <string>
#include <fstream>
#include "src_lib/calculation.h"
#include "json.hpp"

using json = nlohmann::json;
using namespace boost::asio;
using ip::tcp;
using std::string;
using std::cout;
using std::cerr;
using std::endl;
using std::istream;


const char *path_file = "../../data/users.txt";


bool check_user(json &user);


int main() {
    io_service service;
    ip::tcp::acceptor acceptor(service, ip::tcp::endpoint(ip::tcp::v4(), 8001));
    while (true) {
        ip::tcp::socket sock(service);
        acceptor.accept(sock);
        streambuf buf;
        read_until(sock, buf, '\n');
        istream is(&buf);
        json user;
        is >> user;

        if (check_user(user)) {
            json message;
            message["message"] = "true";
            write(sock, buffer(message.dump() + '\n'));
        } else {
            json message;
            message["message"] = "Ошибка ауентификации";
            write(sock, buffer(message.dump() + '\n'));
            sock.close();
        }
        streambuf buf_command;
        read_until(sock, buf_command, '\n');
        istream is_command(&buf_command);
        json message;
        is_command >> message;
        if (message["command"] == "1") {
            message.clear();
            message["message"] = "Введите:x, y, z";
            write(sock, buffer(message.dump() + '\n'));

            streambuf buf_data;
            read_until(sock, buf_data, '\n');
            istream is_data(&buf_data);
            message.clear();
            is_data >> message;
            cout << message << endl;
        }
        if (message["command"] == "2") {

        }
    }
    return 0;
}


bool check_user(json &user) {
    bool check = false;
    std::ifstream user_file(path_file);
    if (!user_file.is_open()) {
        cerr << "Не удалось открыть файл\n";
        exit(1);
    }
    string username, password;
    while (user_file >> username >> password) {
        if (user["username"] == username &&
            user["password"] == password) {
            check = true;
            break;
        }
    }
    user_file.close();
    return check;
}