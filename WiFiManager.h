/**************************************************************
   WiFiManager is a library for the ESP8266/Arduino platform
   (https://github.com/esp8266/Arduino) to enable easy
   configuration and reconfiguration of WiFi credentials using a Captive Portal
   inspired by:
   http://www.esp8266.com/viewtopic.php?f=29&t=2520
   https://github.com/chriscook8/esp-arduino-apboot
   https://github.com/esp8266/Arduino/tree/master/libraries/DNSServer/examples/CaptivePortalAdvanced
   Built by AlexT https://github.com/tzapu
   Licensed under MIT license
 **************************************************************/

#ifndef WiFiManager_h
#define WiFiManager_h

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <memory>
#include <EEPROM.h>

extern "C"
{
#include "user_interface.h"
}

const char HTTP_HEAD[] PROGMEM = "<!DOCTYPE html><html lang=\"en\"><head><meta name=\"viewport\"content=\"width=device-width,initial-scale=1,user-scalable=no\" /><title>{v}</title>";
const char HTTP_STYLE[] PROGMEM = "<style> *, *:before, *:after{box-sizing:border-box;}body{ background-color: #1e242f;color:white; } .c{text-align:center;}a {color:white;cursor:pointer;}div,input{padding:5px;font-size:1em;}form{margin-top:8px; }input{width:100%;padding:13px;margin-bottom:5px;border-radius:8px;font-size:1.1em;}body{text-align:center;font-family:verdana;}button{cursor:pointer;padding:13px;border:0;border-radius:8px;background-color:#333f52;color:#fff;width:100%;font-size:1.2em;}button:hover{background-color: #4c596f;}button.main{background-color:#288e91;margin-top:8px;}button.main:hover{background-color:#57c6ca;margin-top:8px;}button a{text-decoration:none;} .q{float:right;text-align:right;} .p{width:50px;height:1em;float:right;position:relative;margin-bottom:5px;} .p> div{position:absolute;top:0;left:0;height:100%;width:100%;background-image:linear-gradient(90deg,transparent 50%,white 50%);background-size:12px 100%;} .l{margin-top:2px;width:1em;display:block;position:relative;height:1em;float:left;margin-right:5px;background-repeat:no-repeat;background:url(data:image/svg+xml;base64,PD94bWwgdmVyc2lvbj0iMS4wIiBlbmNvZGluZz0idXRmLTgiPz48c3ZnIHZlcnNpb249IjEuMSIgaWQ9IkNhcGFfMSIgeG1sbnM9Imh0dHA6Ly93d3cudzMub3JnLzIwMDAvc3ZnIiB4bWxuczp4bGluaz0iaHR0cDovL3d3dy53My5vcmcvMTk5OS94bGluayIgeD0iMHB4IiB5PSIwcHgiIHZpZXdCb3g9IjAgMCAzNjEuMSAzNjEuMSIgc3R5bGU9ImVuYWJsZS1iYWNrZ3JvdW5kOm5ldyAwIDAgMzYxLjEgMzYxLjE7IiB4bWw6c3BhY2U9InByZXNlcnZlIj48c3R5bGUgdHlwZT0idGV4dC9jc3MiPi5zdDB7ZmlsbDojRkZGRkZGO308L3N0eWxlPjxnPjxnIGlkPSJfeDMyXzM3Ll9Mb2NrZWQiPjxnPjxwYXRoIGNsYXNzPSJzdDAiIGQ9Ik0yNzQuOCwxNDEuM1Y5NC4yYzAtNTItNDIuMi05NC4yLTk0LjItOTQuMmMtNTIsMC05NC4yLDQyLjItOTQuMiw5NC4ydjQ3LjFDNjksMTQxLjMsNTUsMTU1LjQsNTUsMTcyLjd2MTU3YzAsMTcuMywxNC4xLDMxLjQsMzEuNCwzMS40aDE4OC40YzE3LjMsMCwzMS40LTE0LjEsMzEuNC0zMS40di0xNTdDMzA2LjIsMTU1LjQsMjkyLjEsMTQxLjMsMjc0LjgsMTQxLjN6IE0xMTcuOCw5NC4yYzAtMzQuNywyOC4xLTYyLjgsNjIuOC02Mi44YzM0LjcsMCw2Mi44LDI4LjEsNjIuOCw2Mi44djQ3LjFIMTE3LjhWOTQuMnogTTI3NC44LDMyOS43SDg2LjR2LTE1N2gxODguNEwyNzQuOCwzMjkuN0wyNzQuOCwzMjkuN3ogTTE2NC45LDI2Mi42djIwLjFjMCw4LjcsNywxNS43LDE1LjcsMTUuN2M4LjcsMCwxNS43LTcsMTUuNy0xNS43di0yMC4xYzkuMy01LjQsMTUuNy0xNS41LDE1LjctMjdjMC0xNy4zLTE0LjEtMzEuNC0zMS40LTMxLjRzLTMxLjQsMTQuMS0zMS40LDMxLjRDMTQ5LjIsMjQ3LjEsMTU1LjUsMjU3LjEsMTY0LjksMjYyLjZ6Ii8+PC9nPjwvZz48L2c+PC9zdmc+);background-size:1em;} </style>";
const char HTTP_SCRIPT[] PROGMEM = "<script>function c(l) {document.getElementById('s').value= l.innerText||l.textContent;document.getElementById('p').focus(); } </script>";
const char HTTP_HEAD_END[] PROGMEM = "</head><body><div style=\"text-align:left;display:inline-block;width:100%;max-width:320px;\"><img style='width:70px;height:auto;margin:8px 0;'src='data:image/svg+xml;base64,PHN2ZyB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciIHZpZXdCb3g9IjAgMCAyNzkuMzEgMjA4Ljk4Ij48ZGVmcz48c3R5bGU+LmF7ZmlsbDojZmZmO30uYntmaWxsOm5vbmU7fTwvc3R5bGU+PC9kZWZzPjx0aXRsZT5sb2dvLXdoaXRlPC90aXRsZT48cGF0aCBjbGFzcz0iYSIgZD0iTTIzNi44MywzNmE5Ni45MSw5Ni45MSwwLDAsMC0xMC42Ni42Miw0Ni42OSw0Ni42OSwwLDAsMC0xMiwzLjExLDYwLjQ1LDYwLjQ1LDAsMCwwLTEzLDcuMjgsNzcuOTQsNzcuOTQsMCwwLDAtMTMuNiwxM1Y0My44MkEyLjE5LDIuMTksMCwwLDAsMTg0LjE1LDQybC0xMC43Nyw3LjYydjEwNy4yYTIyNy4yMiwyMjcuMjIsMCwwLDAtMTEuODMtMjMuNjJBNjguODksNjguODksMCwwLDAsODYuMzQsNDAuNDYsMjM0LjgyLDIzNC44MiwwLDAsMCw2MS42OSwyMWMtLjg5LS42LTEuNzctMS4xOC0yLjY0LTEuNzUtMS4zLS44NS0yLjU5LTEuNjUtMy44NS0yLjQybC0xLjI2LS43NmgwbC0xLS41OWMtLjY3LS4zOS0xLjM0LS43Ny0yLTEuMTNsLTEtLjU0LTEuNy0uOS0xLjU2LS43OWMtLjQ4LS4yNC0xLS40OC0xLjQ0LS43LS44OS0uNDItMS43Ny0uODItMi42NC0xLjE5cy0xLjUtLjYzLTIuMjQtLjkyQTQxLjcyLDQxLjcyLDAsMCwwLDI2LjU0LDYuMDdsMCwwaDBjLS4xNi0uMjItLjMxLS40Mi0uNDgtLjYyYTIuNTEsMi41MSwwLDAsMC0uMjYtLjMxaDBxLS4zMi0uMzctLjY2LS43MmE3LjI3LDcuMjcsMCwwLDAtLjU3LS41NGwtLjQyLS4zOC0uMjUtLjItLjQyLS4zMi0uNTctLjQxLS42LS4zOGEzLjM4LDMuMzgsMCwwLDAtLjQxLS4yNEE3LjM2LDcuMzYsMCwwLDAsMjEsMS40NWwtLjM2LS4xNkwyMCwxbC0uMi0uMDhMMTkuNTQuODVsLS4xMywwTDE5LjEzLjcxLDE4LjU4LjU0LDE4LjMuNDYsMTcuNjEuMzFsLS4yLDAtLjEsMGgtLjA3czAsMCwwLDBsLS4wNywwTDE3LC4ybC0uMTMsMC0uMTksMEEzLjU1LDMuNTUsMCwwLDAsMTYuMjMuMUwxNS40NSwwaC0uOTRsLS42LDBxLS4zNywwLS43NS4wNmE0LjQzLDQuNDMsMCwwLDAtLjUxLjA2bC0uMjQsMC0uNy4xM2gtLjA2bC0uNDUuMS0uMiwwLS41Ny4xNkwxMC4yMi43UTEwLC43OCw5LjY4Ljg4bC0uNTcuMjItLjYuMjYtLjQyLjJhMTQuNzIsMTQuNzIsMCwwLDAtMS4zOS44Yy0uMjIuMTQtLjQzLjI5LS42NC40NGExMi40MSwxMi40MSwwLDAsMC0xLjIsMUw0LjMsNC4zbC0uNDkuNTFhMTMuMiwxMy4yLDAsMCwwLS45LDEuMDhjLS4yOC4zOC0uNTQuNzctLjc5LDEuMTctLjEyLjIxLS4yNC40MS0uMzUuNjJhMi4xMiwyLjEyLDAsMCwwLS4xMS4yMWMtLjEyLjIyLS4yMy40Ni0uMzQuNjlsMCwuMDgsMCwuMDksMCwuMDdjLS4xMS4yNi0uMjIuNTMtLjMyLjgxLDAsMCwwLC4wNywwLC4xMXMtLjEzLjM3LS4xOS41NkExMy4yMywxMy4yMywwLDAsMCwuMywxMS43MWMwLC4yNC0uMDkuNDgtLjEzLjczcy0uMDcuNDgtLjA5LjczQTE0Ljc2LDE0Ljc2LDAsMCwwLDAsMTQuNjdjMCwuMjcsMCwuNTQsMCwuODFzMCwuNDYuMDYuNjkuMDUuNDQuMDguNjZ2MGEuMTEuMTEsMCwwLDEsMCwwYzAsLjI3LjA5LjU1LjE1LjgzLDAsLjA2LDAsLjExLDAsLjE3LDAsLjIxLjEuNDMuMTYuNjVzLjA5LjMxLjE0LjQ3LjA4LjI2LjEzLjQuMDYuMTkuMS4yOGgwQTEuNDYsMS40NiwwLDAsMSwxLDE5LjlIMWMuMTIuMzIuMjYuNjQuNC45NSwwLDAsMCwwLDAsMGwuMTkuMzljLjA3LjEzLjE0LjI2LjIuMzlhMSwxLDAsMCwxLC4wOC4xNGwuMi4zNS4wOS4xNi4yNi40LjIuMjguMTUuMjIuMDYuMDljMCwuMDguMTEuMTUuMTYuMjJhMS44MywxLjgzLDAsMCwxLC4xMy4xNmMuMTcuMjIuMzUuNDMuNTMuNjRzLjM0LjM4LjUyLjU2QTE1Ljc3LDE1Ljc3LDAsMCwwLDUuODgsMjYuNGgwYy4yNy4yLjU1LjM5Ljg0LjU4bC4wNiwwYTYuNSw2LjUsMCwwLDAsLjYzLjM3LjYuNiwwLDAsMCwuMTUuMDlsLjI1LjEzLjEyLjA3LjYuM2gwdjBjMCwuNDYuMDYuOTIuMSwxLjM5LjE0LDEuNTcuMzYsMy4yMS42Nyw0LjkzLjE0LjgzLjMxLDEuNjguNSwyLjU1cy40LDEuNzkuNjQsMi43Mkg3LjU1VjE2Ny44NUE5LjQ3LDkuNDcsMCwwLDAsMTcsMTc3LjMyaDQuNzRWNjguNDljMi45NCw1LjkxLDYuMzIsMTIuMDgsMTAuMDgsMTguNDJhNjguOTEsNjguOTEsMCwwLDAsNjUuNDMsOTAuNDEsNjkuODIsNjkuODIsMCwwLDAsOC44NC0uNTZjMjQuMDYsMjEsNDQuMzcsMzIuMjIsNTgsMzIuMjJhMTcuMTMsMTcuMTMsMCwwLDAsMTEuMjMtMy42NGM2LjItNS4xLDcuOC0xNC42Myw0Ljg0LTI4aDcuNFY3Ni43M3ExMS0xMi42MiwyMi4zLTE5LjU1dDIzLTYuOTNhNDQuMTYsNDQuMTYsMCwwLDEsMTUsMi4yMywyMywyMywwLDAsMSw5Ljk1LDYuNzUsMjcuNjUsMjcuNjUsMCwwLDEsNS41MSwxMS41NSw3Mi4wOCw3Mi4wOCwwLDAsMSwxLjY5LDE2LjYydjg5LjkyaDE0LjIyVjg2LjE1UTI3OS4zMSwzNiwyMzYuODMsMzZabS04My42OCw3Mi40MmE1NS42Myw1NS42MywwLDAsMS0uODMsOS42MywzOTcuNTQsMzk3LjU0LDAsMCwwLTI2LjQyLTM2Yy04LjctMTAuNTktMTcuNy0yMC41Mi0yNi43LTI5LjQ3QTU1LjkyLDU1LjkyLDAsMCwxLDE1My4xNSwxMDguNDZaTTE0LjMzLDI0LjMyaC0uMTdsLTEuNDYuMDYtMS41Mi0uNywwLDAtLjA1LDBoMGwtLjQyLS4xOWMtLjIxLS4xLS40Mi0uMi0uNjMtLjMyYTMuNjcsMy42NywwLDAsMS0uNi0uMzZjLS4xOS0uMTItLjM4LS4yNi0uNTYtLjRhNS42Niw1LjY2LDAsMCwxLS41NC0uNDMsNS40Nyw1LjQ3LDAsMCwxLS41LS40N0E1LjI1LDUuMjUsMCwwLDEsNy4zNSwyMWwtLjQzLS41M2gwYTUuMTIsNS4xMiwwLDAsMS0uMzktLjU2LDUuMDgsNS4wOCwwLDAsMS0uMzUtLjU5aDBhNC42LDQuNiwwLDAsMS0uMjktLjU5Yy0uMS0uMjEtLjE4LS40Mi0uMjYtLjYzYTcuMzIsNy4zMiwwLDAsMS0uMzktMS4zMWMtLjA1LS4yMy0uMDktLjQ1LS4xMy0uNjhzMC0uMjMsMC0uMzUsMC0uMjIsMC0uMzQsMC0uNDYsMC0uN0E5LjY3LDkuNjcsMCwwLDEsMTQuNjYsNWwuNTgsMCwuNTcsMCwuNDIuMDYuMzguMDdjLjMxLjA2LjYyLjE0LjkyLjIzYTcuMzQsNy4zNCwwLDAsMSwuODkuMzNMMTksNmMuMTkuMS4zOS4yLjU3LjMxaDBjLjE1LjA5LjMuMTguNDQuMjhhNC43MSw0LjcxLDAsMCwxLC40OC4zNGwuMjIuMThhLjczLjczLDAsMCwxLC4yMi4xOCwzLjc4LDMuNzgsMCwwLDEsLjQzLjM4Yy4xMS4xLjIxLjIuMzEuMzFhMi4yMSwyLjIxLDAsMCwxLC4yOS4zM2wuMjMuMjhhNi44NSw2Ljg1LDAsMCwxLC41Ny43NmgwbC4wNi4wOEwyMy45MiwxMWguMTVsLjE3LDIuMzl2LjA1YTcuNDUsNy40NSwwLDAsMSwuMDcuOTVjMCwuMDgsMCwuMTcsMCwuMjZhOC4xMiw4LjEyLDAsMCwxLDAsLjg2aDBhMi41NywyLjU3LDAsMCwxLS4wNS40YzAsLjEyLDAsLjIzLS4wNS4zNGEuMDYuMDYsMCwwLDEsMCwwdjBhMSwxLDAsMCwxLS4wNS4yNWMwLC4xMi0uMDUuMjQtLjA4LjM2TDI0LDE3LjJhNy4yNCw3LjI0LDAsMCwxLS4yNS43OWMtLjA1LjE1LS4xMS4zLS4xNy40NHMtLjEyLjI4LS4xOS40Mi0uMTQuMjgtLjIyLjQyYTMuMTgsMy4xOCwwLDAsMS0uMjMuNGMtLjA4LjE0LS4xNi4yNi0uMjUuMzlzLS4xOC4yNi0uMjcuMzhhOC4wNiw4LjA2LDAsMCwxLS41OS43Miw0LjE3LDQuMTcsMCwwLDEtLjM2LjM3LDIsMiwwLDAsMS0uMy4yOCwzLjM4LDMuMzgsMCwwLDEtLjM1LjMxbC0uNDEuMzItLjQyLjI5LS40Mi4yNy0uNDguMjZhMy44OCwzLjg4LDAsMCwxLS40Ni4yMiw0Ljg5LDQuODksMCwwLDEtLjQ4LjIsOS40Nyw5LjQ3LDAsMCwxLTMuNDcuNjVaTTMwLjUsNzAuMjZjLS42MS0xLjEtMS4xOS0yLjE3LTEuNzQtMy4yMy0yLjgxLTUuMjctNS4xMS0xMC4wOS03LTE0LjQ5VjM5LjU5SDE3LjEyYzAtLjE4LS4wOS0uMzUtLjE0LS41Mi0uMTctLjYzLS4zMy0xLjI0LS40Ny0xLjg0YTUwLjI5LDUwLjI5LDAsMCwxLTEuMzQtNy45MXYwbC41NCwwaDBsLjU0LS4wNWgwbC4zMywwLC4xOCwwLC41My0uMDkuNTItLjEuMTgsMCwuMzQtLjA5LjM1LS4wOS4xNy0uMDUuNS0uMTcuMSwwLC41My0uMmEzLjY5LDMuNjksMCwwLDAsLjQyLS4xN2wuMjUtLjFhMTYuMTYsMTYuMTYsMCwwLDAsMS44My0xbC41Ny0uMzkuMDYsMCwuNS0uMzcuNDItLjM1aDBsLjMzLS4yOWMuMTEtLjA5LjIxLS4xOS4zMi0uM2EuOC44LDAsMCwwLC4xNy0uMTVsLjIyLS4yMS4wNi0uMDcuMDktLjA5LjIzLS4yNC4xNS0uMTdjLjA2LS4wOC4xMy0uMTUuMi0uMjNzLjMzLS40MS40OS0uNjEuMzItLjQzLjQ3LS42NWwuMjMtLjM1YS4zNS4zNSwwLDAsMSwuMDUtLjA4czAsMCwwLDBsLjE2LS4yN2EuNTYuNTYsMCwwLDAsMC0uMDhzMCwwLDAsMCwuMTEtLjE4LjE1LS4yN2wuMDktLjE2YS4yNS4yNSwwLDAsMCwwLS4wN2wwLS4wOHEuMTUtLjI3LjI3LS41NGMuMDgtLjE3LjE2LS4zNC4yMy0uNTEsMCwwLDAsMCwwLDBhLjc2Ljc2LDAsMCwwLDAtLjExczAsMCwwLDBsMCwwTDI4LjMsMjBsLjA5LS4yMmMuMS0uMjYuMTktLjUzLjI3LS44cy4xMS0uMzYuMTYtLjU1aDBsLjA5LS4zNkEuODMuODMsMCwwLDAsMjksMThjMC0uMDYsMC0uMTIsMC0uMTguMDctLjMuMTItLjYxLjE3LS45MnMuMDYtLjQ1LjA4LS42OCwwLS40LjA1LS42djBhLjQ5LjQ5LDAsMCwwLDAtLjEyYzAtLjI1LDAtLjUsMC0uNzVoMHYtLjA3YzAtLjE3LDAtLjMzLDAtLjQ5czAtLjI1LDAtLjM3YS4yMi4yMiwwLDAsMSwwLS4wOGgwdjBhNDAsNDAsMCwwLDEsNy44NCwyLjIzbDEuMjQuNDksMS40NS42MSwxLjcxLjc3aDBxMS4xNS41NCwyLjM0LDEuMTRoMGwxLjY2Ljg2LDEuMjcuNjhjMS4yNy43LDIuNTgsMS40NiwzLjkyLDIuMjVhMTkwLjc5LDE5MC43OSwwLDAsMSwyMy41MywxN2gwYzEsLjg0LDIsMS43MSwzLjA4LDIuNjFoMGwuMTguMTVoMEE2OSw2OSwwLDAsMCwzNS4yMyw3OC41NFEzMi42OCw3NC4yNCwzMC41LDcwLjI2Wm02Ni43OCw5NC4wNmE1NS44Nyw1NS44NywwLDAsMS04LTExMS4xNmgwbDIuNDMsMi4zNSwxLjY2LDEuNjRjLjU1LjU0LDEuMDksMS4wOCwxLjY0LDEuNjRxMi40OSwyLjQ5LDUsNS4xLDMuMzMsMy40OCw2LjY5LDcuMTcsNi43NCw3LjQxLDEzLjU0LDE1LjY3YTM4MS43NiwzODEuNzYsMCwwLDEsMjkuNDksNDAuOTFoMEE1Niw1NiwwLDAsMSw5Ny4yOCwxNjQuMzJabTczLjQ1LDM1LjM5Yy0uMjMuMTktLjQ4LjM3LS43My41NGE4LjE1LDguMTUsMCwwLDEtLjguNDksNC44NSw0Ljg1LDAsMCwxLS42MS4yOSwzLjc0LDMuNzQsMCwwLDEtLjUxLjIybC0uNDguMTgtLjIuMDdhNS4yOSw1LjI5LDAsMCwxLS43MS4yLDYuODMsNi44MywwLDAsMS0uODEuMTgsOS42Nyw5LjY3LDAsMCwxLTEuMzYuMmgtLjA2YTEyLjYzLDEyLjYzLDAsMCwxLTEuNTMuMDhjLS41OCwwLTEuMTksMC0xLjgyLS4wOS0uMzEsMC0uNjMtLjA2LTEtLjExcy0uNzYtLjEtMS4xNS0uMTdjLS43MS0uMTItMS40NS0uMjgtMi4yMy0uNDhsLS43OC0uMjFjLS40NC0uMTItLjg4LS4yNS0xLjM0LS40bC0uNzQtLjI0Yy0uNS0uMTYtMS0uMzUtMS41Mi0uNTRhNTYuNzQsNTYuNzQsMCwwLDEtNS43Ni0yLjU0Yy0xLS40OC0yLTEtMy0xLjU3bC0xLS41OC0xLjQxLS44MnEtMi42Ny0xLjU2LTUuNTYtMy41MWMtLjY3LS40NC0xLjM2LS45MS0yLjA2LTEuNC0uNDgtLjMzLTEtLjY3LTEuNDYtMS0uOTQtLjY3LTEuOS0xLjM2LTIuODgtMi4wOS00LjMzLTMuMjItOS03LTE0LjA2LTExLjQyYTY5LDY5LDAsMCwwLDQyLjU5LTMzLjU1YzMuNDUsNi4zNCw2LjQ1LDEyLjUsOC45MywxOC4zN0MxNzUuMiwxNzkuODcsMTc2LjcxLDE5NC44LDE3MC43MywxOTkuNzFaIi8+PHBhdGggY2xhc3M9ImEiIGQ9Ik0yLjYsMjNsLS4yLS4yOC4yNy40LjA4LjFabTEuNTUsMS44OUExMi4wNiwxMi4wNiwwLDAsMCw1Ljg4LDI2LjQsMTUuNzcsMTUuNzcsMCwwLDEsNC4xNSwyNC44N1pNNS44OSwyNi40aDBhMCwwLDAsMCwwLDAsMHEuNDEuMy44NC41N0M2LjQ0LDI2Ljc5LDYuMTYsMjYuNiw1Ljg5LDI2LjRaTTguNTQsMjhsLS42LS4zYy4yLjExLjM5LjIyLjYuMzFoMHYwWm0yLjY0LTQuMywxLjUyLjcsMS40Ni0uMDZBOS43OCw5Ljc4LDAsMCwxLDExLjE4LDIzLjY4Wm00LDUuNjJ2MGE0LjczLDQuNzMsMCwwLDAsLjU0LDBabS41NSwwaC4xOGwuMzYsMFpNMjAuMzcsMS4xNkEzLjYxLDMuNjEsMCwwLDAsMjAsMWwuNzEuMjlaTTIwLjcsMjhsLjYzLS4zLjYxLS4zMy41OS0uMzZBMTYuMTYsMTYuMTYsMCwwLDEsMjAuNywyOFptMS44My0xLC40My0uMjhMMjMsMjYuN2EuMi4yLDAsMCwwLC4wNy0uMDZabS42My0uNDMuMjctLjE5LjIzLS4xN2gwWm0uNS0uMzdhLjcxLjcxLDAsMCwwLC4xNC0uMWwwLDBjLjA4LS4wNi4xNi0uMTQuMjQtLjIxWk0yNC4wNywxMWgtLjE1TDIyLjgsOS40N2E5LjY1LDkuNjUsMCwwLDEsMS40NCwzLjk0Wm0uODQsMTQuMTFhLjguOCwwLDAsMS0uMTcuMTVjLS4xMS4xMS0uMjEuMjEtLjMyLjMuMjEtLjE4LjQxLS4zNS42MS0uNTUsMCwwLDAsMCwwLDBsLjA2LS4wN1ptLjc1LS43OGgwbC4xOS0uMjNDMjUuNzksMjQuMiwyNS43MiwyNC4yNywyNS42NiwyNC4zNVptMS40Ni0yLC4xMi0uMTgsMC0uMDlabS4xNi0uMjdoMGEuMi4yLDAsMCwwLDAtLjA4QS41Ni41NiwwLDAsMSwyNy4yOCwyMi4xMlptLjkzLTEuODZoMHYwUzI4LjIxLDIwLjI1LDI4LjIxLDIwLjI2Wm0xLjA2LTYuNjFhLjIyLjIyLDAsMCwwLDAsLjA4di0uMDhaIi8+PHBhdGggY2xhc3M9ImIiIGQ9Ik02LjczLDI3cS0uNDQtLjI3LS44NC0uNTdoMEM2LjE2LDI2LjYsNi40NCwyNi43OSw2LjczLDI3WiIvPjxwYXRoIGNsYXNzPSJiIiBkPSJNOC42NSwyOS4zOWMtLjA1LS40OC0uMDgtLjk0LS4xMS0xLjRoMHYwQzguNTgsMjguNDYsOC42MSwyOC45Miw4LjY1LDI5LjM5WiIvPjxwYXRoIGNsYXNzPSJiIiBkPSJNMTQuODUsMGgwWiIvPjxwYXRoIGNsYXNzPSJiIiBkPSJNMTUuNzEsMjkuMjhjLS4xOCwwLS4zNiwwLS41NCwwaDBaIi8+PHBhdGggY2xhc3M9ImIiIGQ9Ik0xNi4yNiwyOS4yM2wtLjM2LDBoLS4xOFoiLz48cGF0aCBjbGFzcz0iYiIgZD0iTTIyLjUzLDI3bC0uNTkuMzYtLjYxLjMzLS42My4zQTE2LjE2LDE2LjE2LDAsMCwwLDIyLjUzLDI3WiIvPjxwYXRoIGNsYXNzPSJiIiBkPSJNMjMuMSwyNi42NGEuMi4yLDAsMCwxLS4wNy4wNmwtLjA3LjA1LS40My4yOFoiLz48cGF0aCBjbGFzcz0iYiIgZD0iTTI0LjA4LDI1Ljg4Yy0uMDguMDctLjE2LjE1LS4yNC4yMWwwLDAtLjE0LjExLS4yMy4xNy0uMjcuMTkuNS0uMzdaIi8+PHBhdGggY2xhc3M9ImIiIGQ9Ik01LjYyLDE4LjA1YTcuMzIsNy4zMiwwLDAsMS0uMzktMS4zMUE4LjU3LDguNTcsMCwwLDAsNS42MiwxOC4wNVoiLz48cGF0aCBjbGFzcz0iYiIgZD0iTTE0LjE2LDI0LjMybC0xLjQ2LjA2LTEuNTItLjdBOS43OCw5Ljc4LDAsMCwwLDE0LjE2LDI0LjMyWiIvPjxwYXRoIGNsYXNzPSJiIiBkPSJNMTguMTMsMjMuNjhhOS40Nyw5LjQ3LDAsMCwxLTMuNDcuNjVoLS4zM2wuNjcsMEE5Ljg0LDkuODQsMCwwLDAsMTguMTMsMjMuNjhaIi8+PHBhdGggY2xhc3M9ImIiIGQ9Ik0yNC4yNCwxMy40MUE5LjY1LDkuNjUsMCwwLDAsMjIuOCw5LjQ3TDIzLjkyLDExaC4xNVoiLz48cGF0aCBjbGFzcz0iYiIgZD0iTTI0LjI5LDE0LjE1aDBjMC0uMjMsMC0uNDYtLjA1LS42OGE3LjQ1LDcuNDUsMCwwLDEsLjA3Ljk1QTIuNDYsMi40NiwwLDAsMSwyNC4yOSwxNC4xNVoiLz48cGF0aCBjbGFzcz0iYiIgZD0iTTI1LjEzLDI0LjkybC0uMDYuMDdzMCwwLDAsMGwtLjEyLjFaIi8+PHBhdGggY2xhc3M9ImIiIGQ9Ik0yNS44NiwyNC4xMmwtLjE5LjIzYTEuMjksMS4yOSwwLDAsMS0uMTYuMTdsLjE1LS4xN0MyNS43MiwyNC4yNywyNS43OSwyNC4yLDI1Ljg2LDI0LjEyWiIvPjxwYXRoIGNsYXNzPSJiIiBkPSJNMjEsMS40NWwtLjM2LS4xNkwyMCwxYTMuNjEsMy42MSwwLDAsMSwuNDIuMTZBNS4zNyw1LjM3LDAsMCwxLDIxLDEuNDVaIi8+PHBhdGggY2xhc3M9ImIiIGQ9Ik0yNy4zMywyMmEuMi4yLDAsMCwxLDAsLjA4LjQxLjQxLDAsMCwwLS4wNS4wOWwtLjEyLjE4LjE2LS4yN0EuNTYuNTYsMCwwLDAsMjcuMzMsMjJaIi8+PHBhdGggY2xhc3M9ImIiIGQ9Ik0yOC4yNCwyMC4yMWwwLC4wNSwwLC4xMWEuNzYuNzYsMCwwLDAsMC0uMTFzMCwwLDAsMFoiLz48cGF0aCBjbGFzcz0iYiIgZD0iTTI5LjI4LDEzLjY1di4wOGEuMjIuMjIsMCwwLDEsMC0uMDhaIi8+PHBhdGggY2xhc3M9ImIiIGQ9Ik0yOS4yOSwxNS41N2MwLC4xOSwwLC4zOCwwLC41NywwLDAsMCwwLDAsMEMyOS4yNiwxNiwyOS4yOCwxNS43NywyOS4yOSwxNS41N1oiLz48cGF0aCBjbGFzcz0iYiIgZD0iTTE0Ni42NCwxOTcuMzhjLTEtLjQ4LTItMS0zLTEuNTdDMTQ0LjY3LDE5Ni4zNywxNDUuNjcsMTk2Ljg5LDE0Ni42NCwxOTcuMzhaIi8+PHBhdGggY2xhc3M9ImIiIGQ9Ik0xNTksMjAxLjc5Yy0uNzEtLjEyLTEuNDUtLjI4LTIuMjMtLjQ4QzE1Ny41NSwyMDEuNSwxNTguMjksMjAxLjY2LDE1OSwyMDEuNzlaIi8+PHBhdGggY2xhc3M9ImIiIGQ9Ik0xNjQuNDYsMjAyLjA4YTEyLjYzLDEyLjYzLDAsMCwxLTEuNTMuMDhjLS41OCwwLTEuMTksMC0xLjgyLS4wOUExOS43NSwxOS43NSwwLDAsMCwxNjQuNDYsMjAyLjA4WiIvPjxwYXRoIGNsYXNzPSJiIiBkPSJNMTY1Ljg4LDIwMS44OGE5LjY3LDkuNjcsMCwwLDEtMS4zNi4yQzE2NSwyMDIsMTY1LjQ1LDIwMiwxNjUuODgsMjAxLjg4WiIvPjxwYXRoIGNsYXNzPSJhIiBkPSJNNzcuMzgsNDIuMzFsLjE3LjE2aDBaTTIuNiwyM2wtLjItLjI4LjI3LjQuMDguMVptMi42My02LjI0YTcuMzIsNy4zMiwwLDAsMCwuMzksMS4zMUE4LjU3LDguNTcsMCwwLDEsNS4yMywxNi43NFpNMTUsMjQuM2wtLjY3LDBoLjMzYTkuNDcsOS40NywwLDAsMCwzLjQ3LS42NUE5Ljg0LDkuODQsMCwwLDEsMTUsMjQuM1ptOS4zMS05Ljg5YTcuNDUsNy40NSwwLDAsMC0uMDctLjk1YzAsLjIyLDAsLjQ1LjA1LjY4aDBBMi40NiwyLjQ2LDAsMCwwLDI0LjMxLDE0LjQxWm00LjkzLDEuNzZjMCwuMjMsMCwuNDUtLjA4LjY4bC4wOS0uNzFjMC0uMTksMC0uMzgsMC0uNTdDMjkuMjgsMTUuNzcsMjkuMjYsMTYsMjkuMjQsMTYuMTdabTAtMi41NHYwYTQwLjcyLDQwLjcyLDAsMCwxLDcuODQsMi4yMUE0MCw0MCwwLDAsMCwyOS4yOCwxMy42M1oiLz48L3N2Zz4=' />";
const char HTTP_PORTAL_OPTIONS[] PROGMEM = "<form action=\"/wifi\"method=\"get\"><button class=\"main\">Configure WiFi</button></form><br/><form action=\"/r\"method=\"post\"><button>Reset</button></form>";
const char HTTP_ITEM[] PROGMEM = "<div><i class=\"{i}\"></i><a href='#'onclick='c(this)'>{v}</a><span class=\"p\"><div style=\"width: {r}%\"></div></div>";
const char HTTP_FORM_START[] PROGMEM = "<form method='get'action='wifisave'><input id='s'name='s'length=32 placeholder='SSID'><br/><input id='p'name='p'length=64 type='password'placeholder='password'><br/>";
const char HTTP_FORM_PARAM[] PROGMEM = "<br/><input id='{i}'name='{n}'length='{l}'placeholder='{p}'value='{v}' {c}>";
const char HTTP_FORM_END[] PROGMEM = "<br/><button type='submit'class=\"main\">Save</button></form>";
const char HTTP_SCAN_LINK[] PROGMEM = "<br/><button class=\"c\"><a href=\"/wifi\">Scan</a></button>";
const char HTTP_SAVED[] PROGMEM = "<div>Credentials Saved<br/>Trying to connect to network.<br/>If it fails,reconnect to try again</div>";
const char HTTP_END[] PROGMEM = "</div></body></html>";

#ifndef WIFI_MANAGER_MAX_PARAMS
#define WIFI_MANAGER_MAX_PARAMS 10
#endif

class WiFiManagerParameter
{
public:
  /** 
        Create custom parameters that can be added to the WiFiManager setup web page
        @id is used for HTTP queries and must not contain spaces nor other special characters
    */
  WiFiManagerParameter(const char *custom);
  WiFiManagerParameter(const char *id, const char *placeholder, const char *defaultValue, int length);
  WiFiManagerParameter(const char *id, const char *placeholder, const char *defaultValue, int length, const char *custom);
  ~WiFiManagerParameter();

  const char *getID();
  const char *getValue();
  const char *getPlaceholder();
  int getValueLength();
  const char *getCustomHTML();

private:
  const char *_id;
  const char *_placeholder;
  char *_value;
  int _length;
  const char *_customHTML;

  void init(const char *id, const char *placeholder, const char *defaultValue, int length, const char *custom);

  friend class WiFiManager;
};

class WiFiManager
{
public:
  WiFiManager();
  ~WiFiManager();

  int _webSocketPortAddress = 0; // should only need 16 bits
  int _webSocketIPAddress = 4;   // needs at least 32 bits
  String _webSocketPort = "5000";
  String _webSocketIP = "192.168.0.19";

  int _serverPortAddress = 12;
  int _serverIPAddress = 16;
  String _serverPort = "3000";
  String _serverIP = "192.168.0.12";

  boolean autoConnect();
  boolean autoConnect(char const *apName, char const *apPassword = NULL);

  //if you want to always start the config portal, without trying to connect first
  boolean startConfigPortal();
  boolean startConfigPortal(char const *apName, char const *apPassword = NULL);

  // get the AP name of the config portal, so it can be used in the callback
  String getConfigPortalSSID();

  void resetSettings();

  //sets timeout before webserver loop ends and exits even if there has been no setup.
  //useful for devices that failed to connect at some point and got stuck in a webserver loop
  //in seconds setConfigPortalTimeout is a new name for setTimeout
  void setConfigPortalTimeout(unsigned long seconds);
  void setTimeout(unsigned long seconds);

  //sets timeout for which to attempt connecting, useful if you get a lot of failed connects
  void setConnectTimeout(unsigned long seconds);

  void setDebugOutput(boolean debug);
  //defaults to not showing anything under 8% signal quality if called
  void setMinimumSignalQuality(int quality = 8);
  //sets a custom ip /gateway /subnet configuration
  void setAPStaticIPConfig(IPAddress ip, IPAddress gw, IPAddress sn);
  //sets config for a static IP
  void setSTAStaticIPConfig(IPAddress ip, IPAddress gw, IPAddress sn);
  //called when AP mode and config portal is started
  void setAPCallback(void (*func)(WiFiManager *));
  //called when settings have been changed and connection was successful
  void setSaveConfigCallback(void (*func)(void));
  //adds a custom parameter, returns false on failure
  bool addParameter(WiFiManagerParameter *p);
  //if this is set, it will exit after config, even if connection is unsuccessful.
  void setBreakAfterConfig(boolean shouldBreak);
  //if this is set, try WPS setup when starting (this will delay config portal for up to 2 mins)
  //TODO
  //if this is set, customise style
  void setCustomHeadElement(const char *element);
  //if this is true, remove duplicated Access Points - defaut true
  void setRemoveDuplicateAPs(boolean removeDuplicates);

private:
  std::unique_ptr<DNSServer> dnsServer;
  std::unique_ptr<ESP8266WebServer> server;

  //const int     WM_DONE                 = 0;
  //const int     WM_WAIT                 = 10;

  //const String  HTTP_HEAD = "<!DOCTYPE html><html lang=\"en\"><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"/><title>{v}</title>";

  void setupConfigPortal();
  void startWPS();

  const char *_apName = "no-net";
  const char *_apPassword = NULL;
  String _ssid = "";
  String _pass = "";

  unsigned long _configPortalTimeout = 0;
  unsigned long _connectTimeout = 0;
  unsigned long _configPortalStart = 0;

  IPAddress _ap_static_ip;
  IPAddress _ap_static_gw;
  IPAddress _ap_static_sn;
  IPAddress _sta_static_ip;
  IPAddress _sta_static_gw;
  IPAddress _sta_static_sn;

  int _paramsCount = 0;
  int _minimumQuality = -1;
  boolean _removeDuplicateAPs = true;
  boolean _shouldBreakAfterConfig = false;
  boolean _tryWPS = false;

  const char *_customHeadElement = "";

  //String        getEEPROMString(int start, int len);
  //void          setEEPROMString(int start, int len, String string);

  int status = WL_IDLE_STATUS;
  int connectWifi(String ssid, String pass);
  uint8_t waitForConnectResult();

  void handleRoot();
  void handleWifi(boolean scan);
  void handleWifiSave();
  void handleWifiSavePost();
  void handleInfo();
  void handleReset();
  void handleNotFound();
  void handle204();
  boolean captivePortal();
  boolean configPortalHasTimeout();

  // DNS server
  const byte DNS_PORT = 53;

  //helpers
  int getRSSIasQuality(int RSSI);
  boolean isIp(String str);
  String toStringIp(IPAddress ip);

  boolean connect;
  boolean _debug = true;

  void (*_apcallback)(WiFiManager *) = NULL;
  void (*_savecallback)(void) = NULL;

  int _max_params;
  WiFiManagerParameter **_params;

  template <typename Generic>
  void DEBUG_WM(Generic text);

  template <class T>
  auto optionalIPFromString(T *obj, const char *s) -> decltype(obj->fromString(s))
  {
    return obj->fromString(s);
  }
  auto optionalIPFromString(...) -> bool
  {
    DEBUG_WM("NO fromString METHOD ON IPAddress, you need ESP8266 core 2.1.0 or newer for Custom IP configuration to work.");
    return false;
  }
};

#endif
