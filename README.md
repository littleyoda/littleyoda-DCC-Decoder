# littleyoda-DCC-Decoder (ESP8266 based decoder for model railway)

Ein flexibeler Decoder für Modelleisenbahnen, der den Prozessor vom Typ ESP8266 nutzt, DCC-Befehle auswerten kann und über eine WLAN-Schnittstelle verfügt und verschiedene Geräte (Weichenmotoren, LED, Servos) steuern kann. 

## Status
 * in Entwicklung
 * Prototypen funktionieren

## Beschreibung

Mit diesem Decoder möchte ich eine möglichst flexibele Platform nutzen.

Aus diesem Grund setze ich auf die Prozessoren vom Typ [ESP8266](https://de.wikipedia.org/wiki/ESP8266) und hier insbesondere auf die fertigen [NodeMCU-Boards](https://en.wikipedia.org/wiki/NodeMCU).  Sie verfügen über 8 nutzbare GPIO, jeder GPIO ist u.a. als PWM-Ausgang nutzbar. Selbst ein kleiner Webserver läuft auf diesem Prozessor.

### Input
Befehle können über die folgenden Wege empfangen werden:
 * klassisch über DCC
 * drahtlos über WLAN in Verbindung mit einer [Z21](http://www.z21.eu/). Wenn mit einem Handgerät ein Schaltbefehl ausgeführt wird, wird dieser Befehl per WLAN an den Decoder weitergeleitet.

### Aktoren
 * Weichenmotoren (LGB, PIKO Weichenantriebe)
 * LED (zur Zeit nur an oder aus)
 * Servo

### Stromversorgung
Entweder über die Schienen oder für eine seperate Stormversorgung. 6 Volt reichen, sofern keine Motorsteuerung gewünscht ist.

### Konfiguration
Zur Zeit über einen json-String in Config.h. Langfristig sol die Konfiguration über den Webserver hochgeladen werden.
Eine Konfiguration über DCC wird wegen der fehlenden Flexibilität nicht unterstützt.

### Schaltplan
[Aktueller Entwurf](http://gartenbahntechnik.de/forum/viewtopic.php?f=24&t=347&start=20#p2242)

Je nach Bedarf, können die verschiedenen Komponenten kombiniert werden.



## Geschichte

Im [Gartenbahntechnik-Forum](http://gartenbahntechnik.de/forum/viewtopic.php?f=24&t=347) kann man die Entwicklungsgeschichte verfolgen.

Videos:
* http://www.open4me.de/spurg/WeicheVersion0.mp4
* http://www.open4me.de/spurg/WeicheVersion1.mp4
* http://www.open4me.de/spurg/WeicheServo.mp4


