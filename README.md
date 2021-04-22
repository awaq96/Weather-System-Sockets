# Weather-System-Sockets
Weather-System-Sockets

The Server side of the program: 
	The Program will take in the weather20.txt file, read the data and store it
    in a Linked List. It will then request a port number from the user and create a socket connection
    Once the connection is set, it will receieve a city name and then search the Linked List for data regarding x city
    The city's information will be sent back to client and also displayed on server side
    If the city is not in the list, server will display and send back to client "No Data"
    When client Quits, Server will close

This Client side of the program:
	It will take in the hostname from the user followed by requesting a port #
	If the inputs are correct it will form a connection with the server. Then it will prompt the user to input a city name
	If the city is in the Weather20.txt file the program will output the weather data. If it's not in the file, the program
	will display "No Data"
	User can type Quit and the program will terminate
