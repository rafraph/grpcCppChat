# grpcCppChat

This c++ project demonstrate simple chat app using grpc 

## chat.proto
Defines the protocol. 

One bidirectional streaming service - SendMsg.

One message - Message, for the sender name and the text of the message.

## chat_server.cc
The server implementation.

It implements the SendMsg service.

This service store all streams in a vector.

It read meassegs from the clients using read stream, and write the messasges to all clients by looping over all write streams which stored in the vector.

## chat_client.cc
The client implementation.

It using the SendMsg service from the stub in order to write messages to the server (which distribute the message to all clients), and to read the stream that coming from the server.

## How to run it
To run the server type: chat_server

To run the client type: chat_client

## Build the proto
In 
type: make chat.grpc.pb.cc chat.pb.cc

## Build the code
In 
type: make
