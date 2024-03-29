/****************************************************************************************************************************
  Async_UdpClient.ino
  AsyncUDP_WT32_ETH01 is a Async UDP library for the WT32_ETH01 (ESP32 + LAN8720)
  Based on and modified from ESPAsyncUDP Library (https://github.com/me-no-dev/ESPAsyncUDP)
  Built by Khoi Hoang https://github.com/khoih-prog/AsyncUDP_WT32_ETH01
  Licensed under MIT license
  Version: 2.0.1
  
  Version Modified By   Date      Comments
  ------- -----------  ---------- -----------
  2.0.0   K Hoang      10/07/2021 Initial coding for WT32_ETH01. Bump up version to v2.0.0 to sync with AsyncUDP v2.0.0
  2.0.1   K Hoang      12/07/2021 Update to use WebServer_WT32_ETH01 v1.2.0
 *****************************************************************************************************************************/
#include <Arduino.h>

#define ASYNC_UDP_WT32_ETH01_DEBUG_PORT      Serial

// Use from 0 to 4. Higher number, more debugging messages and memory usage.
#define _ASYNC_UDP_WT32_ETH01_LOGLEVEL_      1

#include <AsyncUDP_WT32_ETH01.h>

IPAddress remoteIPAddress = IPAddress(192, 168, 0, 22);

#define UDP_REMOTE_PORT         5698

/////////////////////////////////////////////

// Select the IP address according to your local network
IPAddress myIP(192, 168, 0, 232);
IPAddress myGW(192, 168, 0, 58);
IPAddress mySN(255, 255, 255, 0);

// Google DNS Server IP
IPAddress myDNS(8, 8, 8, 8);

/////////////////////////////////////////////

AsyncUDP udp;

void parsePacket(AsyncUDPPacket packet)
{
  Serial.print("UDP Packet Type: ");
  Serial.print(packet.isBroadcast() ? "Broadcast" : packet.isMulticast() ? "Multicast" : "Unicast");
  Serial.print(", From: ");
  Serial.print(packet.remoteIP());
  Serial.print(":");
  Serial.print(packet.remotePort());
  Serial.print(", To: ");
  Serial.print(packet.localIP());
  Serial.print(":");
  Serial.print(packet.localPort());
  Serial.print(", Length: ");
  Serial.print(packet.length());
  Serial.print(", Data: ");
  Serial.write(packet.data(), packet.length());
  Serial.println();
  //reply to the client
  packet.printf("Got %u bytes of data", packet.length());
}

void setup()
{
  Serial.begin(115200);
  while (!Serial);

  Serial.print("\nStarting Async_UDPClient on " + String(ARDUINO_BOARD));
  Serial.println(" with " + String(SHIELD_TYPE));
  Serial.println(WEBSERVER_WT32_ETH01_VERSION);
  Serial.println(ASYNC_UDP_WT32_ETH01_VERSION);

  Serial.setDebugOutput(true);
  Serial.println("OK");

  //bool begin(uint8_t phy_addr=ETH_PHY_ADDR, int power=ETH_PHY_POWER, int mdc=ETH_PHY_MDC, int mdio=ETH_PHY_MDIO,
  //           eth_phy_type_t type=ETH_PHY_TYPE, eth_clock_mode_t clk_mode=ETH_CLK_MODE);
  //ETH.begin(ETH_PHY_ADDR, ETH_PHY_POWER, ETH_PHY_MDC, ETH_PHY_MDIO, ETH_PHY_TYPE, ETH_CLK_MODE);
  ETH.begin(ETH_PHY_ADDR, ETH_PHY_POWER);
  Serial.println("OK eth phy");

  // Static IP, leave without this line to get IP via DHCP
  //bool config(IPAddress local_ip, IPAddress gateway, IPAddress subnet, IPAddress dns1 = 0, IPAddress dns2 = 0);
  ETH.config(myIP, myGW, mySN, myDNS);
  Serial.println("OK config");

  WT32_ETH01_onEvent();
  Serial.println("OK event");

  // WT32_ETH01_waitForConnect();
  Serial.println("OK wait");

  // Client address
  Serial.print("Async_UDPClient started @ IP address: ");
  Serial.println(ETH.localIP());

  if (udp.connect(remoteIPAddress, UDP_REMOTE_PORT))
  {
    Serial.println("UDP connected");

    udp.onPacket([](AsyncUDPPacket packet)
    {
      parsePacket( packet);
    });

    //Send unicast
    udp.print("Hello Server!");
  }
}

void loop()
{
  delay(10000);
  //Send broadcast on port UDP_REMOTE_PORT = 1234
  udp.broadcastTo("Anyone here?", UDP_REMOTE_PORT);
}