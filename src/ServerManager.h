#ifndef ServerManager_h
#define ServerManager_h

#include <Arduino.h>

class ServerManager_
{
private:
    ServerManager_() = default;

public:
    static ServerManager_ &getInstance();
    void setup();
    void tick(); 
    void loadSettings();
    void sendButton(byte btn, bool state);
    void erase();
    void sendTCP(String message);
    bool isConnected;
    IPAddress myIP;
};

extern ServerManager_ &ServerManager;
 
#endif
