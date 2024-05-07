/************************************************************************************************************************
 * 
 mmmmmm                           mmmm     mmmm   
 ##""""##                       ##""""#   ##""##  
 ##    ##   m#####m  ##m####m  ##        ##    ## 
 #######    " mmm##  ##"   ##  ##  mmmm  ##    ## 
 ##    ##  m##"""##  ##    ##  ##  ""##  ##    ## 
 ##mmmm##  ##mmm###  ##    ##   ##mmm##   ##mm##  
 """""""    """" ""  ""    ""     """"     """"   



                                                                          mmmm                                                    
                                                                         ##"""     ##                                             
  m####m   ##m###m    m####m   ##m####m            mm#####m   m####m   #######   #######  ##      ##  m#####m   ##m####   m####m  
 ##"  "##  ##"  "##  ##mmmm##  ##"   ##            ##mmmm "  ##"  "##    ##        ##     "#  ##  #"  " mmm##   ##"      ##mmmm## 
 ##    ##  ##    ##  ##""""""  ##    ##             """"##m  ##    ##    ##        ##      ##m##m##  m##"""##   ##       ##"""""" 
 "##mm##"  ###mm##"  "##mmmm#  ##    ##            #mmmmm##  "##mm##"    ##        ##mmm   "##  ##"  ##mmm###   ##       "##mmmm# 
   """"    ## """      """""   ""    ""             """"""     """"      ""         """"    ""  ""    """" ""   ""         """""  
           ##                                                                                                                     
* 
 *************************************************************************************************************************/
 
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>

#ifndef APSSID
#define APSSID "ESP8266AP"
#define APPSK  "88888888"
#endif

#define CONST_SETTING_COUNT 5
/* Set these to your desired credentials. */
const char *ssid = APSSID;
const char *password = APPSK;

typedef struct
{
   uint8_t wifi_mode;                             
   uint8_t factory_Flag;
 
   uint16_t max_eeprom_count;                           //Count of eeprom bit be used
   uint8_t max_wifi_count; 
   uint8_t ssid_len[max_wifi_count];                                    //The lenth of ssid and password
   uint8_t password_len[max_wifi_count];                            //Max count of wifi to connected
   uint8_t ssid_address[max_wifi_count];
   uint8_t password_address[max_wifi_count];
   unsigned char *ssid[max_wifi_count]
   unsigned char *password[max_wifi_count]
  
}Wifi_Efu;

void Wifi_Efu_factory_Init(void)
{
    Wifi_Efu wifi_efu;
   
    wifi_efu.ssid_len = 25;
    wifi_efu.password_len  = 25;
    wifi_efu.max_wifi_count = 3;
    for(int i = 0; i<max_wifi_count;i++){
        wifi_efu.ssid_address[i] = wifi_efu.ssid_len*i+10;                                                       //the ssid data in eeprom: |ssid_adress1:ssidlen|ssid_adress2:ssidlen|ssid_adress3:ssidlen|
        wifi_efu.password_address[i] = wifi_efu.ssid_len*wifi_efu.max_wifi_count+ wifi_efu.password_len*i+10;    //  |password_adress1:passwordlen|password_adress2:passwordlen|password_adress3:passwordlen|
    }
    wifi_efu.wifi_mode = 0;
    
    wifi_efu.max_wifi_count = (wifi_efu.ssid_len+wifi_efu.password_len)*wifi_efu.max_wifi_count+10;
   // wifi_efu.factory_Flag = 1;
  
}

void Wifi_Efu_Init(void)
{
    Wifi_Efu wifi_efu;
    //E-Fuse Setting
    wifi_efu.factory_Flag = EEPROM.read(0);
    wifi_efu.wifi_mode =EEPROM.read(1);
    wifi_efu.max_wifi_count = EEPROM.read(2);
    //Flexible Setting  
    for(int i = 0; i<wifi_efu.max_wifi_count; i++){
        wifi_efu.ssid_len[i]= EEPROM.read(CONST_SETTING_COUNT+i);
        wifi_efu.password_len[i]  = EEPROM.read(CONST_SETTING_COUNT+wifi_efu.max_wifi_count+i);
        if(i==0) wifi_efu.ssid_address[i] =  EEPROM.read(wifi_efu.ssid_len[i]+(CONST_SETTING_COUNT+2*wifi_efu.max_wifi_count));
        else wifi_efu.ssid_address[i] = EEPROM.read(wifi_efu.ssid_address[i-1]+wifi_efu.ssid_len[i-1]);                                                                                                                       
    }
    for(int i = 0; i<wifi_efu.max_wifi_count; i++){
         if(i==0) wifi_efu.password_address[i] =  EEPROM.read(wifi_efu.password_len[wifi_efu.max_wifi_count-1]+wifi_efu.ssid_address[wifi_efu.max_wifi_count-1]);
        else wifi_efu.password_address[i] = EEPROM.read(wifi_efu.password_address[i-1]+wifi_efu.password_len[i-1]);
    }
    wifi_efu.max_wifi_count = wifi_efu.password_address[wifi_efu.max_wifi_count-1]+wifi_efu.password_len[wifi_efu.max_wifi_count-1];

}


ESP8266WebServer server(80);

/* Just a little test message.  Go to http://192.168.4.1 in a web browser
   connected to this access point to see it.
*/
void handleRoot() {
  Serial.println(server.arg("SSID"));
  Serial.println(server.arg("PASSWORD"));
  Serial.println(server.arg("WIFIID"));
   if(server.arg("WIFIID")!=NULL){
    
   }else if(server.arg("SSID") != NULL){
    
   }else if(server.arg("PASSWORD") != NULL){
    
   }else if(server.arg("OVER") != NULL){
    
   }
   
   server.send(200, "text/html", "ok");
}

void setup() {

  Serial.begin(115200);
  
  EEPROM.begin(512);
  if(EEPROM.read(0)<1){
       Wifi_Efu_factory_Init();
  }else{
       Wifi_Efu_Init();
  }

  Serial.println("EEPROM is Ready! Count of %d",COUNT_OF_EEPROM_BIT);
  Serial.print("Configuring access point...");
  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.softAP(ssid, password);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.on("/test", handleRoot);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}
