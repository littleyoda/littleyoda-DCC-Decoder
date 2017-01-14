# littleyoda-DCC-Decoder (ESP8266 based decoder for model railway)

Ein flexibles Framework für Modelleisenbahnen-Decoder, auf Basis eines ESP8266. Er kann DCC-Befehle auswerten und verfügt über eine WLAN-Schnittstelle. Es können verschiedene Aktoren  (Weichenmotoren, LED, Servos, DC Motoren) angesteuert werden.

Der Funktionsumfang lässt sich leicht durch weitere Klassen erweitern.

## Status
 * in Entwicklung, wird für zwei Dekoder genutzt

Die folgenden Dekoder nutzen diese Software:
 * [Weichendekoder](http://spurg.open4me.de/wordpress/784/Weichendecoder)
 * [ein sehr einfacher Lokdekoder](http://spurg.open4me.de/wordpress/786/Lokdekoder?2)

## Beschreibung

Mit diesem Decoder möchte ich eine möglichst flexible Platform nutzen.

Aus diesem Grund setze ich auf die Prozessoren vom Typ [ESP8266](https://de.wikipedia.org/wiki/ESP8266) und hier insbesondere auf die fertigen [NodeMCU-Boards](https://en.wikipedia.org/wiki/NodeMCU).  Sie verfügen über 8 nutzbare GPIO, jeder GPIO ist u.a. als PWM-Ausgang nutzbar. Selbst ein kleiner Webserver läuft auf diesem Prozessor.

### Input
Befehle können über die folgenden Wege empfangen werden:
 * klassisch über DCC
 * drahtlos über WLAN in Verbindung mit einer [Z21](http://www.z21.eu/). Wenn mit einem Handgerät ein Befehl ausgeführt wird, wird dieser Befehl per WLAN an den Decoder weitergeleitet.
 * Über einen Webbrowser

Andere Zentralen, die über einen Netzwerkanschluß verfügen, können relativ einfach hinzugefügt werden. Hierzu muss eine Klasse analog zur Klasse CmdReceiverZ21Wlan implementiert werden.

### Aktoren
 * Weichenmotoren (LGB, PIKO Weichenantriebe)
 * LED (zur Zeit nur an oder aus)
 * Servo
 * DC Motoren (über PWM)


### Konfiguration
Zur Zeit über einen json-String in Config.h. Langfristig soll die Konfiguration über den Webserver hochgeladen werden.
Eine Konfiguration über DCC wird wegen der fehlenden Flexibilität nicht unterstützt.


## Geschichte

Im [Gartenbahntechnik-Forum](http://gartenbahntechnik.de/forum/viewtopic.php?f=24&t=347) kann man die Entwicklungsgeschichte verfolgen.

Videos:
* http://www.open4me.de/spurg/WeicheVersion0.mp4
* http://www.open4me.de/spurg/WeicheVersion1.mp4
* http://www.open4me.de/spurg/WeicheServo.mp4


