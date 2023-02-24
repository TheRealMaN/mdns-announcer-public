# mDNS Announcer for Homebridge
It started when I wanted to have a running instance of Homebridge to use my smart devices with an Apple Home app. I didn't want to have a 24/7 running mac and didn't have any Raspberry Pi, Orange Pi to install HB on. But I had a remote VPS running a docker, so I thought that it would be great to run a Homebridge in a container there and then point all my iOS devices to it.

However soon I found out that Homebridge doesn't work in a docker bridge mode and you can't access it remotely from Home.app, so sad I know...((. The researching of this problem got me to realise that I should have a mDNS announcer on local network. I tried different approaches and found the one that suites me best - use ESP8266 Wemos D1 mini.

## Advertising a Homekit bridge
## Get config from Homebridge remote server
You need to get the bridge parameters from the running Homebridge container. You can do this by running the console command inside the container installing the avahi-utils (avahi should be listed as default service in your homebridge.conf) and then resolving mDNS record from "_hap._tcp" service like this:

```sh
apt update && apt install avahi-utils
avahi-browse -r _hap._tcp
```

Now save the printed configuration for later and choose your way to run mDNS announcer on your local network.
There are different options.

### macOS - using a built-in 'dns-sd' command example (but your mac should be always on):
Command arguments are:
dns-sd -P <Name> <Type> <Domain> <Port> <Host> <IP> [<TXT>...]

In our case for a remote Homebridge server running in a docker container on VPS the command would be:
```sh
dns-sd -P "Homebridge A3BA 0694" _hap._tcp local 51226 "myvps.ddns.net" "23.100.200.1" "pv=1.1" "ff=0" "s#=1" "md=homebridge" "c#=4" "id=05:5F:66:56:A3:BA" "sf=0" "ci=2" "sh=3E6uB3=="
```


### Using Node Javascript library Bonjour or Ciao
You can announce using Node with these libraries
* [Bonjour](https://github.com/homebridge/bonjour)
* [Ciao](https://github.com/homebridge/ciao)

Code examples in folder "app/via-bonjour.js" and "app/via-ciao.js"


### Using ESP8266 module
For using the ESP board with a remote Homebridge server you need to replace "local" domain prefix that is added to mDNS hostname by ESP8266mDNS library thats located in "Arduino15/packages/esp8266/hardware/esp8266/3.1.1/libraries/ESP8266mDNS/src/). 

You can do that by adding custom domain in string constants (file "LEAmDNS_Transfer.cpp" at line around ~44.):
```cpp
/**
    CONST STRINGS
*/
static const char* scpcLocal    = "local";
static const char* scpcDDNS     = "ddns";  // your added lines
static const char* scpcRemote   = "net";   // your added lines
static const char* scpcServices = "services";
static const char* scpcDNSSD    = "dns-sd";
static const char* scpcUDP      = "udp";
```

Also changing the "add local domain part" in "MDNSResponder::_buildDomainForHost" function.
(file "LEAmDNS_Transfer.cpp" at line around ~980.):

So after the fixing the function should look like (in my case for domain "myvps.ddns.net"):

```cpp
bool MDNSResponder::_buildDomainForHost(const char*                      p_pcHostname,
                                            MDNSResponder::stcMDNS_RRDomain& p_rHostDomain) const
    {
        p_rHostDomain.clear();
        bool bResult = ((p_pcHostname) && (*p_pcHostname)
                        && (p_rHostDomain.addLabel(p_pcHostname))
                        && (p_rHostDomain.addLabel(scpcDDNS))
                        && (p_rHostDomain.addLabel(scpcRemote))
                        && (p_rHostDomain.addLabel(0))
                        );
        DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(
            PSTR("[MDNSResponder] _buildDomainForHost: FAILED!\n")););
        return bResult;
    }
```
That's all, it should compile successfully and work properly) Customize it for your needs and make sure you properly configured your bridge parameters.

Arduino IDE code example in folder "app/via-esp8266.js" 

### Using mdns-utils (mDNSResponder package) on a router with OpenWRT firmawre
You can use "dns-sd" command like on macOS, but it needs additional mdns-utils and coreutils-nohup packages.

After installing these packages you can add command to startup script:
```sh
nohup dns-sd -P "Homebridge A2BA 0231" _hap._tcp local 51226 "vps.ddns.net" "22.132.1.1" "pv=1.1" "ff=0" "s#=1" "md=homebridge" "c#=4" "id=0E:5F:63:56:A3:BA" "sf=0" "ci=2" "sh=231uIQ==" &
```

### Using an Avahi on linux
I dont have exmaple at the moment, but it shuld be something like that:
```sh
avahi-publish -s [options] name service-type port [TXT data ...]
```

```sh
avahi-publish -s "Homebridge A7BA 0754" _hap._tcp 51266 -H "myvps.ddns.net" -v "pv=1.1" "ff=0" "s#=1" "md=homebridge" "c#=4" "id=04:5F:63:56:A6:BA" "sf=0" "ci=2" "sh=8Y1uIH=="
```
