/*
 * ActionPWMOutput.cpp
 * 
 * Basis-Klasse für alle PWM-Klassen
 *
 *  Created on: 09.12.2016
 *      Author: sven
 */

#include <Arduino.h>
#include "ActionPWMOutput.h"
#include "Consts.h"
#include "Utils.h"
#include "GPIO.h"

ActionPWMOutput::ActionPWMOutput() {
}

ActionPWMOutput::~ActionPWMOutput() {
}

void ActionPWMOutput::getHTMLConfig(String urlprefix, Controller *c) {
	Serial.println("Cfg Started");
    c->sendContent("<div class=\"column column-90\">Freq (" + getName() + "): ");
 	c->sendContent(" <a class=\"button\" href=\"" + urlprefix + "key=freq&value=" + String(100) + "\">" + String(100) + "</a>\n");
	c->sendContent(" <a class=\"button\" href=\"" + urlprefix + "key=freq&value=" + String(250) + "\">" + String(250) + "</a>\n");
	c->sendContent(" <a class=\"button\" href=\"" + urlprefix + "key=freq&value=" + String(500) + "\">" + String(500) + "</a>\n");
	c->sendContent(" <a class=\"button\" href=\"" + urlprefix + "key=freq&value=" + String(1000) + "\">" + String(1000) + "</a>\n");
	c->sendContent(" <a class=\"button\" href=\"" + urlprefix + "key=freq&value=" + String(5000) + "\">" + String(5000) + "</a>\n");
	c->sendContent(" <a class=\"button\" href=\"" + urlprefix + "key=freq&value=" + String(10000) + "\">" + String(10000) + "</a>\n");
	c->sendContent("</div>");
	c->sendContent("Aktuelle PWM Konfiguration<br>");
	c->sendContent("<svg height=\"210\" width=\"650\">");
	c->sendContent("<defs><style type=\"text/css\">");
	c->sendContent("<![CDATA[");
	c->sendContent(" line { stroke:rgb(255,0,0);stroke-width:2}");
	c->sendContent("]]>");
	c->sendContent("</style>");
	c->sendContent("</defs>");
	c->sendContent("<rect x=\"0\" y=\"0\" width=\"640\" height=\"128\"/>");
	int xfactor = 5;
	for (int i = 1; i < 128; i++) {
		c->sendContent("<line x1=\"" + String((i - 1) * xfactor) + "\" y1=\"" + String(127 - handlePwmProfil(i - 1)) + "\" x2=\"" + String(i * xfactor) + "\" y2=\"" + String(127 - handlePwmProfil(i)) + "\"  />");
	}
	for (int i = 1; i < 6; i++) {
		c->sendContent("<line x1=\"" + String(i * 25 * xfactor) + "\" y1=\"0\" x2=\"" + String(i * 25 * xfactor) + "\" y2=\"128\" />");
	}
	c->sendContent("</svg>");
}

String ActionPWMOutput::getHTMLController(String urlprefix) {
	String action = "document.getElementById('pwmValue').textContent=document.getElementById('pwmSlider').value; send('" + urlprefix + "&key=sd&value='+document.getElementById('pwmSlider').value)";
	String action2 = "document.getElementById('pwmSlider').value = 0;";
	String message = "<div class=\"row\"> <div class=\"column column-10\">";
	message += "PWM ";
	message +="</div>";
	message +="<div class=\"column column-90\">";
	message += "<div id=\"pwmValue\">0</div>";
	message +=" <input id=\"pwmSlider\" type=range min=-100 max=100 value=0 step=5 oninput=\"" + action + "\" onchange=\"" + action + "\"><br/>";
	message += "<a class=\"button\" href=\"javascript:void(0)\" onclick=\"" + action2 +";" + action + "\">Stop</a>";
	message +="</div>";
	message +="</div>";
	return message;
}

void ActionPWMOutput::setPwmProfil(uint8_t *a) {
	pwmRampe = a;
}

long ActionPWMOutput::handlePwmProfil(long pos) {
	if (pwmRampe != NULL) {
		if (pos < 0 || pos > 127 )
			return pos;
		if (pos == 0) {
			return 0;
		}
		if (pos > 0) {
			return pwmRampe[pos];
		}
		return pwmRampe[pos];
	}
	return pos;
}


void ActionPWMOutput::setFreq(uint32_t value) {
}
