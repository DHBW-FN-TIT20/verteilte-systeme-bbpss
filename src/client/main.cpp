#include <iostream>
#include <sstream>
#include <spdlog/spdlog.h>
#include <list>

#include "client.h"

using namespace std;

void printUsageInformation();
bool isValidIPv4Address(const string &ipAddress);
int getPortNumber(const string &portStr);

/**
 * @brief Entry point of the client application.
 * 
 * @return int 
 * 
 * @note Example usage:
 * @code {.sh}
 *  ./client.exe --server-address <address> --subscribe TOPIC_NAME_1
 *  ./client.exe --server-address <address> --publish TOPIC_NAME_1 TOPIC_DATA_1
 * @endcode
 */
int main(int argc, char** argv) {
    spdlog::info("Initializing client...");
    spdlog::debug("Parsing CLI arguments...");
    // Only if a subscribe is provided, the client has to stay alive after all commands are executed
    bool hasSubscribeCommand = false;

    int port = 0;
    string serverAddress = "127.0.0.1";
    int serverPort = 8080;
    string command = "";
    list<string> arguments = {};

    // Parse CLI arguments and validate their values
    for (int i = 1; i < argc; i++) {
        string arg = argv[i];

        if (arg == "--port" && argc > i + 1) {
            string portStr = argv[i + 1];

            port = getPortNumber(portStr);

            if (port == -1) {
                spdlog::error("Invalid server port: {}", portStr);
                spdlog::error("Shutting down...");
                return 1;
            }

            spdlog::debug("Port: {}", port);

            i += 1;
        } else if (arg == "--server-address" && argc > i + 1) {
            serverAddress = argv[i + 1];

            if (!isValidIPv4Address(serverAddress)) {
                spdlog::error("Invalid server address: {}", serverAddress);
                spdlog::error("Shutting down...");
                return 1;
            }

            spdlog::debug("Server address: {}", serverAddress);

            i += 1;
        } else if (arg == "--server-port" && argc > i + 1) {
            string serverPortStr = argv[i + 1];

            serverPort = getPortNumber(serverPortStr);

            if (serverPort == -1) {
                spdlog::error("Invalid server port: {}", serverPortStr);
                spdlog::error("Shutting down...");
                return 1;
            }

            spdlog::debug("Server-Port: {}", port);

            i += 1;
        } else if (arg == "--help") {
            // Print help message and terminate
            spdlog::debug("Printing help information...");
            printUsageInformation();
            spdlog::info("Shutting down...");
            return 0;
        } else if (arg == "--debug") {
            spdlog::set_level(spdlog::level::debug);
        } else
        {
            // Argument is a command or an argument for a command is missing
            // One client can only perform one kind of command therefore the first command defines the command type
            if (command == "") {
                command = arg;
            }

            if (arg == "--subscribe" && arg == command && argc > i + 1) {
                while (argc > i + 1 && argv[i + 1][0] != '-' && argv[i + 1][1] != '-') {
                    string topicName = argv[i + 1];

                    arguments.push_back(topicName);
                
                    spdlog::debug("-> Subscribing to topic: {}", topicName);

                    hasSubscribeCommand = true;

                    i += 1;
                }
            } else if (arg == "--publish" && arguments.size() == 0 && argc > i + 2) {
                string topicName = argv[i + 1];
                string topicData = argv[i + 2];

                arguments.push_back(topicName);
                arguments.push_back(topicData);

                spdlog::debug("-> Publishing data: {} to topic: {}", topicData, topicName);

                i += 2;
            } else if (arg == "--list-topics" && arg == command) {
                spdlog::debug("-> List topics");
            } else if (arg == "--get-topic-status" && arg == command && argc > i + 1) {
                while (argc > i + 1 && argv[i + 1][0] != '-' && argv[i + 1][1] != '-') {
                    string topicName = argv[i + 1];

                    arguments.push_back(topicName);

                    spdlog::debug("-> Get topic status for topic: {}", topicName);

                    i += 1;
                }
            } else {
                // Unknown CLI argument or missing values
                spdlog::error("Unknown argument, invalid combination of commands or missing values for: {}", arg);
                spdlog::error("Shutting down...");
                return 1;
            }
        }
    }

    spdlog::debug("Successfully parsed CLI arguments.");
    spdlog::debug("Starting client...");

    Client* client;

    try {
        client = new Client(port, serverAddress, serverPort);
    } catch (const runtime_error &e) {
        spdlog::error("{}", e.what());
        spdlog::error("Shutting down...");
        return 1;
    }

    spdlog::info("Successfully started client.");
    spdlog::debug("Executing {}...", command);

    if (command == "--subscribe") {
        for (string topicName : arguments) {
            client->subscribeTopic(topicName);
        }
    } else if (command == "--publish") {
        client->publishTopic(arguments.front(), arguments.back());
    } else if (command == "--list-topics") {
        client->listTopics();
    } else if (command == "--get-topic-status") {
        for (string topicName : arguments) {
            client->getTopicStatus(topicName);
        }
    } else {
        spdlog::error("No command provided or unknown: \"{}\"", command);
        spdlog::error("Shutting down...");
        return 1;
    }

    if (hasSubscribeCommand) {
        spdlog::info("Press ENTER key to exit...");
        cin.get();
    }

    spdlog::info("Shutting down...");
    delete client;

    return 0;
}

void printUsageInformation() {
    cout << endl << "Commandline Tool to start the subscriber client" << endl;
    cout << endl << "Options:" << endl;
    cout << "    " <<"--help: " << endl;
    cout << "    " <<"    " <<"Print this help message" << endl;
    cout << "    " <<"--port <PORT>: " << endl;
    cout << "    " <<"    " <<"Port for connections to this client" << endl;
    cout << "    " <<"    " <<"Default: 0 (random available port)" << endl;
    cout << "    " <<"--server-address <ADDRESS>: " << endl;
    cout << "    " <<"    " <<"IPv4 address of the publisher server" << endl;
    cout << "    " <<"    " <<"Default: 127.0.0.1" << endl;
    cout << "    " <<"--server-port <PORT>: " << endl;
    cout << "    " <<"    " <<"Listening port of the publisher server" << endl;
    cout << "    " <<"    " <<"Default: 8080" << endl;
    cout << endl << "Commands (only one per client):" << endl;
    cout << "    " <<"--subscribe <TOPIC1> <...> <TOPICn>: " << endl;
    cout << "    " <<"    " <<"Subscribe to (multiple) topics" << endl;
    cout << "    " <<"--publish <TOPIC> <MESSAGE>: " << endl;
    cout << "    " <<"    " <<"Publish a message to a topic" << endl;
    cout << "    " <<"--list-topics: " << endl;
    cout << "    " <<"    " <<"List all existing topics" << endl;
    cout << "    " <<"--get-topic-status <TOPIC1> <...> <TOPICn>: " << endl;
    cout << "    " <<"    " <<"Get the status of (multiple) topics" << endl;
    cout << endl << "Debug:" << endl;
    cout << "    " <<"--debug: " << endl;
    cout << "    " <<"    " <<"Enable logging debug information" << endl;
    cout << endl;
}

int getPortNumber(const string& portStr) {
    int port = -1;

    try {
        port = stoi(portStr, nullptr, 10);
    } catch (const exception &e) {
        return -1;
    }

    // Port numbers are available from 0 to 65535
    if (port > 65535 || port < 0) {
        return -1;
    }

    return port;
}

bool isValidIPv4Address(const string& ipAddress) {
    int octetCount = 0;
    stringstream ss(ipAddress);
    string octet;

    if (ipAddress.back() == '.')
        return false;

    while (getline(ss, octet, '.')) {
        try {
            int value = std::stoi(octet);

            if (value < 0 || value > 255)
                return false;

            octetCount++;
        } catch (const invalid_argument&) {
            return false;
        } catch (const std::out_of_range&) {
            return false;
        }
    }

    if (octetCount != 4)
        return false;

    return true;
}