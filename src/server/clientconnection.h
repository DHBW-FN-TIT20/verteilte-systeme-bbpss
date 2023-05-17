#ifndef CLIENTCONNECTION_H
#define CLIENTCONNECTION_H

#include <string>
#include <ctime>

using namespace std;

class ClientConnection
{
    public:
        /**
         * @brief Construct a new Client Connection object
         * 
         * @param address IPv4 Address of the client
         * @param port Port of the client
         * 
         * @details
         * Creates the Socket with the client.
         */
        ClientConnection(string address, int port);
        
        /**
         * @brief Destroy the Client Connection object
         * 
         * @details
         * Closes the socket.
         */
        ~ClientConnection();


        /**
         * @brief Sends a message to the client using the websocket
         * 
         * @param message Message to send
         * @param timestamp Timestamp of the message
         * 
         * @details
         * The sent status code is always 0 (successful).
         */
        void SendMessage(string message, time_t timestamp);
    
    private:
        string address;
        int port;
        int socket;
};

#endif