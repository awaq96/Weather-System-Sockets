#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>
#include <fstream>
#include <sstream>
/*

This is the Server side of the program. The Program will take in the weather20.txt file, read the data and store it
    in a Linked List. It will then request a port number from the user and create a socket connection
    Once the connection is set, it will receieve a city name and then search the Linked List for data regarding x city
    The city's information will be sent back to client and also displayed on server side
    If the city is not in the list, server will display and send back to client "No Data"
    When client Quits, Server will close

 */

using namespace std;

// Using Linked list to store the different cities data
//Node is split into the 3 data entries: City name, Temperature and Sky Conditions
struct node{
    string city;
    string temp;
    string status;
    node *next;
};

class List {
private:
    node *head, *tail;
public:
    List() {
        head = NULL;
        tail = NULL;
    }

//Adds City to the Linked List

    void add(string city,string temp,string status) {

        node *tmp;
        tmp = new node();
        tmp->city = city;
        tmp->temp=temp;
        tmp->status=status;
        tmp->next = NULL;

        if (head == NULL) {
            head = tmp;
            tail = tmp;
        } else {
            tail->next = tmp;
            tail = tail->next;
        }

    }

    // Checks to see if the requested city was one from the weather20.txt file.
    bool haveData(string city){
        node *tmp=head;
        // If the city is found in the list return true otherwise false
        while(tmp!=NULL){
            if(tmp->city==city)
                return true;
            tmp=tmp->next;
        }
        return false;


    }
    // Finds and returns the Temperature of the requested city
    string findTemp(string city) {
        node *tmp = head;
        while(tmp!=NULL){
            if(tmp->city==city)
                return tmp->temp;
            tmp=tmp->next;
        }
    }

    // Finds and returns the sky conditions of the requested city

    string findStatus(string city){
        node *tmp=head;
        while(tmp!=NULL){
            if(tmp->city==city)
                return tmp->status;

            tmp=tmp->next;
        }


    }

    // This function is for deubgging. It will print out the LL so you can check and make sure all the data from the
    // weather20.txt file was stored
    void Print(){

        node *tmp=head;
        while(tmp!=NULL){
            cout<<tmp->city<<"\t"<<tmp->temp<<"\t"<<tmp->status<<endl;
            tmp=tmp->next;
        }
    }

};
int main()
{
// Program first takes in the weather20 file
    ifstream file("weather20.txt");
    List cities;

    string line,city,temperature,status,substring;
    size_t comma;
    string b;
    getline(file,b);


// Parse and store the first line from the file
    comma=b.find(",");
    city = b.substr(0,comma);
    substring = b.substr(comma+1);

    comma=substring.find(",");
    temperature=substring.substr(0,comma);

    status = substring.substr(comma+1);

    cities.add(city,temperature,status);

    // While loop Parses and stores the rest of the input file
    while(getline(file,line)){

        istringstream iss(line);
        //cout<<line<<endl; // Use this line to debug if needed
        // The values are seperated by commas so the program searches for commas and splits the line based off them
        comma=line.find(",");
        // First comma splits the city
        city = line.substr(0,comma);
        substring = line.substr(comma+1);
        // Second comma splits temperature and sky conditions

        comma=substring.find(",");
        temperature=substring.substr(0,comma);

        status = substring.substr(comma+1);
        // Once the line is divided into the 3 segments, send the info to the LL to have is added and stored
        cities.add(city,temperature,status);
    }

    file.close();

    // Request port number from Userr
    int portNum;
    cout<<"Enter Server Port Number: ";
    cin>>portNum;
   // cout<<"\nThe selected Port Number is "<<portNum<<endl;

// The socket code implemented is based off of piazza notes and this YouTube Tutotial:
// Link: https://www.youtube.com/watch?v=cNdlrbZSkyQ

    // Create a socket
    int listening = socket(AF_INET, SOCK_STREAM, 0);
    // If Socket Creation Fails
    if (listening == -1)
    {
        cerr << "Can't create a socket!" << endl;
        return -1;
    }

    // Bind the ip address and port to a socket
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(portNum);
    inet_pton(AF_INET, "0.0.0.0", &hint.sin_addr);

    bind(listening, (sockaddr*)&hint, sizeof(hint));

    // Tell Winsock the socket is for listening
    listen(listening, SOMAXCONN);

    // Wait for a connection
    sockaddr_in client;
    socklen_t clientSize = sizeof(client);

    int clientSocket = accept(listening, (sockaddr*)&client, &clientSize);

    char host[NI_MAXHOST];      // Client's remote name
    char service[NI_MAXSERV];   // Service (i.e. port) the client is connect on

    memset(host, 0, NI_MAXHOST); // same as memset(host, 0, NI_MAXHOST);
    memset(service, 0, NI_MAXSERV);


        inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);


    // Close listening socket
    close(listening);

    // While loop: accept and echo message back to client
    char buf[4096];

    // Keep while loop in indefinite run until user is done
    while (true)
    {
        memset(buf, 0, 4096);

        // Wait for client to send data
        int bytesReceived = recv(clientSocket, buf, 4096, 0);
        if (bytesReceived == -1)
        {
            cerr << "Error in recv(). Quitting" << endl;
            break;
        }

        if (bytesReceived == 0)
        {
            cout << "Client disconnected " << endl;
            break;
        }

        // Store the city name sent from client
        string input= string(buf,0,bytesReceived);

        // Checks if the city requested is in the list
        bool havedata = cities.haveData(input);

        if(havedata) {
            // If the City requested is in the file, display info on server side
            string temp2display = cities.findTemp(input);
            string status2display = cities.findStatus(input);
            cout<<"Weather report for "<<input<<endl;
            cout<<"Tomorrow's maximum temperature is "<<temp2display<<" F"<<endl;
            cout<<"Tomorrow's sky condition is "<<status2display<<endl<<endl;

            // Concatenate the data and send it to the client
            string sendback = temp2display+","+status2display;
            send(clientSocket, sendback.c_str(), sendback.size()+1, 0);


        }
        else{
            // If the requested city is no in the file, display No Data
            string result = "No Data";
            cout<<"Weather report for "<<input<<endl<<"No Data"<<endl<<endl;
            // Send "No Data" to client
            send(clientSocket,result.c_str(),result.size()+1,0);

        }

    }

    // Close the socket
    close(clientSocket);

    return 0;
}