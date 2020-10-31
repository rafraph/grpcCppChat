#include <algorithm>
#include <chrono>
#include <cmath>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include <grpc/grpc.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>
#include <grpcpp/security/server_credentials.h>
#ifdef BAZEL_BUILD
#include "examples/protos/chat.grpc.pb.h"
#else
#include "chat.grpc.pb.h"
#endif

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReaderWriter;
using grpc::Status;
using intelchat::Message;
using intelchat::Chat;

Message MakeMessage(std::string sender, std::string text) 
{
  Message m;
  m.set_sender(sender);
  m.set_text(text);
  return m;
}

class ChatImpl final : public Chat::Service 
{
 public:

  Status SendMsg(ServerContext* context, 
                 ServerReaderWriter< Message, Message>* stream) override;

 private:
  std::vector<ServerReaderWriter<Message, Message>*> srw;
};

Status ChatImpl::SendMsg(ServerContext* context, 
                 ServerReaderWriter< Message, Message>* stream) 
{
  Message msg;
  this->srw.push_back(stream);
  while (stream->Read(&msg)) 
  {
    std::vector<ServerReaderWriter< Message, Message>*>::iterator it;
    for (it = this->srw.begin(); it != this->srw.end(); ++it) //write to all clients
      (*it)->Write(msg);
      
  }

  return Status::OK;
}

void RunServer() 
{
  std::string server_address("0.0.0.0:50051");
  ChatImpl service;

  ServerBuilder builder;
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(&service);
  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << server_address << std::endl;

  server->Wait();
}

int main(int argc, char** argv) 
{
  RunServer();

  return 0;
}
