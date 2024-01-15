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
            while (true){
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

                    struct data *task1_data = task1(message["x"], message["y"], message["z"]);

                    int i = 0;
                    nlohmann::json main_task1_data;
                    nlohmann::json data_task1_cur;

                    while (task1_data[i].n_point != -1) {
                        data_task1_cur.clear();
                        data_task1_cur["n_point"] = task1_data[i].n_point;
                        data_task1_cur["temp"] = task1_data[i].temp;
                        data_task1_cur["dx"] = task1_data[i].dx;
                        main_task1_data["data"].push_back(data_task1_cur);
                        i++;
                    }
                    write(sock, buffer(main_task1_data.dump()+'\n'));
                }else if (message["command"] == "2") {
                    struct data *task2_data = task2();

                    int i = 0;
                    nlohmann::json main_task2_data;
                    nlohmann::json data_task2_cur;

                    while (i!=7) {
                        data_task2_cur.clear();
                        data_task2_cur["time"] = task2_data[i].time;
                        data_task2_cur["temp_mean"] = task2_data[i].temp_mean;
                        data_task2_cur["dx_mean"] = task2_data[i].dx_mean;
                        main_task2_data["data"].push_back(data_task2_cur);
                        i++;
                    }
                    write(sock, buffer(main_task2_data.dump() + '\n'));
                }else{
                    break;
                }
            }
        } else {
            json message;
            message["message"] = "Ошибка ауентификации";
            write(sock, buffer(message.dump() + '\n'));
            sock.close();
            continue;
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