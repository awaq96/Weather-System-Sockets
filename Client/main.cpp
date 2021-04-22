#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>

using namespace std;
/*


This is the client side of the program. It will take in the hostname from the user followed by requesting a port #
If the inputs are correct it will form a connection with the server. Then it will prompt the user to input a city name
If the city is in the Weather20.txt file the program will output the weather data. If it's not in the file, the program
will display "No Data"
User can type Quit and the program will terminate


*/

int main()
{
    //	Create a socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    // If socket creation fails
    if (sock == -1)
    {
        return 1;
    }


    // This section will get the host name from the user
    string hostName;
    cout <<"Enter the Server host name: ";
    cin>>hostName;
    // Convert the string the user input to format that can be used
    const char* hostName2char = hostName.c_str();
    struct hostent *hp;
    // Gets the host from the inputted name
    hp=gethostbyname(hostName2char);





// Request port number from user
    int port;
    cout<< "Enter the server port number: ";
    cin>>port;


//Get the buffer from the hostent structure associated with local host
    char *ipBuffer;
    ipBuffer= inet_ntoa(*((struct in_addr*)hp->h_addr_list[0]));


// The Socket Code implemented is based off the Piazza Tutorials and this Youtube Tutorial:
// Link: https://www.youtube.com/watch?v=fmn-pRvNaho

    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(port);
    inet_pton(AF_INET, ipBuffer, &hint.sin_addr);


    //	Connect to the server on the socket
    int connectRes = connect(sock, (sockaddr*)&hint, sizeof(hint));
    if (connectRes == -1)
    {
        return 1;
    }


    char buf[4096]; // Buffer since we don't know how long the input will be
    string userInput;

    bool run = false;
    // Loop allows the program to run multiple queries until the User is done

    do {
        //Enter requested city
        cout << "Enter a city name: ";
        if(!run){
            cin.ignore();
            run = true;}

        getline(cin, userInput);
        // User writing in Quit will terminate program

        if(userInput=="Quit")
            break;

        //	Send the requested city to the server
        int sendRes = send(sock, userInput.c_str(), userInput.size() + 1, 0);

       // If connection to server fails
        if (sendRes == -1)
        {
            cout << "Could not send to server!\r\n";
            continue;
        }

        //		Wait for response
        memset(buf, 0, 4096);
        int bytesReceived = recv(sock, buf, 4096, 0);
        // If no response from server
        if (bytesReceived == -1)
        {
            cout << "There was an error getting response from server\r\n";
        }
        else
        {
            //		Display response
            string response = string(buf, bytesReceived);
            // If City requested is not in the weather20.txt file then the server sends back No Data
            if(response.length()==8){
                cout <<response <<endl;
            }
            else{
                // If City requested was in the file we get back the comma seperated string
                // Parse it into 3 segments and display
                string temp= response.substr(0,response.find(","));
                string condition = response.substr(response.find(",")+1);
                cout<<endl<<"Tomorrow's maximum temperature is "<<temp<<" F"<<endl;
                cout<<"Tomorrow's sky condition is "<<condition<<endl<<endl;
            }
        }
        // Give User opportunity to end program
        cout <<endl<<"To end program type 'Quit'"<<endl;
    } while(true);

    //	Close the socket once they are done
    close(sock);

    return 0;
}