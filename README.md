# MC833 Hands on Network

This repository is dedicated to learning and exploring UDP and TCP network sockets for MC833 - Network Lab classes. Here we have implemented a simple crud interface in C with creating, deleting, searching, and updating users in a mocked database.

## How to run
1 - Clone this repository on one or more computers connected to the same network or with a public IP address.
```BASH
git clone https://github.com/Joao-Pedro-MB/MC833-Hands-On-Network.git
```
2 - Go to the socket folder you want  and build the code:
2.1 - For UDP sockets:
```BASH
cd socket-udp
sh scripts/build.sh
```

2.2 - For TCP sockets:
```BASH
cd socket-tcp
sh scripts/build.sh
```

3 - Run the server:
Open a terminal and run:
```BASH
sh scripts/run-server.sh
```

4 - Run the client:
Now in a different terminal run (or a different computer):
```BASH
sh scripts/run-client.sh IP of the server
```
if no IP is provided, the client will try to connect to localhost.
**If you are running server and client in the same computer, you can and are encouraged to run the client without any arguments.**
**The PORT always will be 3490, if you want another PORT change the PORT definition in all the .h files.**

5 - All set:
Now you are good to go! You can use the client to create, delete, search, and update users in the database.