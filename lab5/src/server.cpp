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


struct User {
    string username;
    string password;

    User(const string &username, const string &password);
};

User::User(const string &username, const string &password) : username(username), password(password) {}

const char *path_file = "../../data/users.txt";


bool check_user(struct User &user);


int main() {
    io_service service;
    ip::tcp::acceptor acceptor(service, ip::tcp::endpoint(ip::tcp::v4(), 8001));
    while (true) {
        ip::tcp::socket sock(service);
        acceptor.accept(sock);
        streambuf buf;
        read_until(sock, buf, '}');
        std::istream is(&buf);
        json userJson;
        is >> userJson;


        User user(userJson["username"], userJson["password"]);

        if (check_user(user)) {
            string message = "true";
            write(sock, buffer(message));
        } else {
            string message = "false";
            write(sock, buffer(message));
        }
        sock.close();
    }


    return 0;
}

bool check_user(struct User &user) {
    bool check = false;
    std::ifstream user_file(path_file);
    if (!user_file.is_open()) {
        cerr << "Не удалось открыть файл\n";
        exit(1);
    }
    string username, password;
    while (user_file >> username >> password) {
        if (user.username == username &&
            user.password == password) {
            check = true;
            break;
        }
    }
    user_file.close();
    return check;
}