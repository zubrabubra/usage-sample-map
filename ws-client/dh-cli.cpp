// Compile with:
// g++ -std=gnu++0x example-client-cpp11.cpp -o example-client-cpp11
#include "../../easywsclient/easywsclient.hpp"
//#include "easywsclient.cpp" // <-- include only if you don't want compile separately
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <memory>
#include <unistd.h>

int main(int argc, char ** argv)
{
  
  if (argc == 1) {
    printf("dh-cli <number of forks> <server name> <port number> <url end point> <guid/name> <count>\n");
    return 1;
  }

  int forkCount = atoi(argv[1]);
  std::string url = std::string("ws://") + std::string(argv[2]) + std::string(":") + std::string(argv[3]) + std::string(argv[4]);
  
  while (forkCount > 0) {
    forkCount--;
    pid_t pid = fork();
  
    if (pid == 0) {
      using easywsclient::WebSocket;

      std::unique_ptr<WebSocket> ws(WebSocket::from_url(url));
      assert(ws);
      //authenticate device
      ws->send("{'action': 'authenticate', 'accessKey': '1jwKgLYi/CdfBTI9KByfYxwyQ6HUIEfnGSgakdpFjgk='}");
      ws->poll(-1);
      ws->dispatch( [](const std::string & message) {
          printf(">>A>> %s\n", message.c_str());
        } );

      ws->send("{'action': 'device/save', 'deviceId': '"+std::string(argv[5])+"', 'device': {'deviceGuid': '"+std::string(argv[5])+"', 'name': '"+std::string(argv[5])+"', 'deviceClass': { 'name': 'Rest Device from Authorized User', 'version': '1.0'}, 'network' : { 'name' : 'VirtualLed Sample Network' }, 'status' : 'online', 'data': {'jsonString': 'string'}, 'blocked': false}}");
      ws->poll(-1);

      for (int message_num = 0; message_num < atoi(argv[6]); message_num++) {
        ws->send("{'action': 'notification/insert', 'deviceGuid': '"+std::string(argv[5])+"', 'notification': { 'notification': 'values', 'parameters': { 'temp': '23.3C', 'pressure': '764mm' } }}");
        ws->poll(-1);
        ws->dispatch( [](const std::string & message) {
            printf(">>A>> %s\n", message.c_str());
        } );
        sleep(1);
      }

      sleep(10);
      return 0;
      /*
      ws->send("goodbye");
      ws->send("hello");
      while (ws->getReadyState() != WebSocket::CLOSED) {
        WebSocket::pointer wsp = &*ws; // <-- because a unique_ptr cannot be copied into a lambda
        ws->poll();
        ws->dispatch([wsp](const std::string & message) {
            printf(">>> %s\n", message.c_str());
            if (message == "world") { wsp->close(); }
        });
      }
      */
    }
  }
    // N.B. - unique_ptr will free the WebSocket instance upon return:
  return 0;
}