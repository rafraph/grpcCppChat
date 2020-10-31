#include <chrono>
#include <iostream>
#include <memory>
#include <random>
#include <string>
#include <thread>

#include <grpc/grpc.h>
#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>
#ifdef BAZEL_BUILD
#include "examples/protos/chat.grpc.pb.h"
#else
#include "chat.grpc.pb.h"
#endif

using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReaderWriter;
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

class ChatClient 
{
 public:
  ChatClient(std::shared_ptr<Channel> channel)
      : stub_(Chat::NewStub(channel)) 
  {
    
  }

  void SendMsg() 
  {
    ClientContext context;    
    std::shared_ptr<ClientReaderWriter<Message, Message> > stream(stub_->SendMsg(&context));

    std::thread writer([stream]() 
    {
      std::string sender, text = "";
      Message msg;

      std::cout << "What is your name? " << std::endl;
      std::cin >> sender;
      std::cout << "You can start chatting" << std::endl;
      while(true)
      {
        std::cin >> text;
        msg = MakeMessage(sender,text);
        stream->Write(msg);
      }
      stream->WritesDone();
    });

    Message server_msg;
    while (stream->Read(&server_msg)) 
    {
      std::cout << "from " << server_msg.sender()
                << ": " << server_msg.text() << std::endl;
    }
    writer.join();
    Status status = stream->Finish();
    if (!status.ok()) 
    {
      std::cout << "RouteChat rpc failed." << std::endl;
    }
  }

 private:
  std::unique_ptr<Chat::Stub> stub_;
};

int main(int argc, char** argv) 
{

  ChatClient guide( grpc::CreateChannel("localhost:50051",
                    grpc::InsecureChannelCredentials()));
  
  guide.SendMsg();  

  return 0;
}

