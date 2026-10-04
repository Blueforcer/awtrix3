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
#ifndef AWTRIX_DISABLE_TIMER
    // Broadcast a timer-sync packet on the LAN (propagation surface). Sent 3x for
    // best-effort delivery; receivers dedup by (src,seq).
    void sendTimerSync(const String &payload);
#endif
    bool isConnected;
    IPAddress myIP;
};

extern ServerManager_ &ServerManager;
 
#endif
