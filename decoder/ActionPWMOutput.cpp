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

String ActionPWMOutput::getHTMLCfg(String urlprefix) {
	Serial.println("Cfg Started");
	String message = "";
	if (message.reserve(6500)) {
		message += "<div class=\"column column-90\">Freq: ";
		message += " <a class=\"button\" href=\"" + urlprefix + "key=freq&value=" +  String(100) + "\">" + String(100) + "</a>\n";
		message += " <a class=\"button\" href=\"" + urlprefix + "key=freq&value=" +  String(250) + "\">" + String(250) + "</a>\n";
		message += " <a class=\"button\" href=\"" + urlprefix + "key=freq&value=" +  String(500) + "\">" + String(500) + "</a>\n";
		message += " <a class=\"button\" href=\"" + urlprefix + "key=freq&value=" +  String(1000) + "\">" + String(1000) + "</a>\n";
		message += " <a class=\"button\" href=\"" + urlprefix + "key=freq&value=" +  String(5000) + "\">" + String(5000) + "</a>\n";
		message += " <a class=\"button\" href=\"" + urlprefix + "key=freq&value=" +  String(10000) + "\">" + String(10000) + "</a>\n";
		message += "</div>";
		message += "<svg height=\"210\" width=\"650\">";
		message += "<defs><style type=\"text/css\">";
		message +=  "<![CDATA[";
		message +=  " line { stroke:rgb(255,0,0);stroke-width:2}";
		message +=  "]]>";
		message +=  "</style>";
		message += "</defs>";
		message += "<rect x=\"0\" y=\"0\" width=\"640\" height=\"128\"/>";
		int xfactor = 5;
		for (int i = 1; i < 128; i++) {
			message += "<line x1=\"" + String((i-1) * xfactor) + "\" y1=\"" + String(127 - getValue(i-1))  + "\" x2=\"" +  String(i * xfactor) + "\" y2=\"" +  String(127 - getValue(i)) + "\"  />";
		}
		for (int i = 1; i < 6; i++) {
			message += "<line x1=\"" + String(i * 25 * xfactor) + "\" y1=\"0\" x2=\"" +  String(i * 25 * xfactor) + "\" y2=\"128\" />";
		}
		message += "</svg>";
		return message;
	} else {
		return "Out of Memory";
	}
}

String ActionPWMOutput::getHTMLController(String urlprefix) {
	String action = "document.getElementById('pwmValue').textContent=this.value; send('" + urlprefix + "&key=sd&value='+this.value)";
	String message =  "<div class=\"row\"> <div class=\"column column-10\">";
	message += "PWM ";
	message += "</div>";
	message += "<div class=\"column column-90\">";
	message +=  "<div id=\"pwmValue\">0</div>";
	message += " <input type=range min=-100 max=100 value=0 step=5 oninput=\"" + action + "\" onchange=\"" + action + "\"><br/>";
	message += "</div>";

	message += "</div>";
	return message;
}


void ActionPWMOutput::setPwmValues(uint8_t* a) {
	arr = a;
}

uint8_t ActionPWMOutput::getValue(uint8_t pos) {
	if (arr != NULL) {
		return arr[pos];
	}
	return pos;
}
