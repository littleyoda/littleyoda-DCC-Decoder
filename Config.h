/*
 * WlanConfig.h
 *
 *  Created on: 27.08.2016
 *      Author: sven
 */

#ifndef CONFIG_H_
#define CONFIG_H_

	char configuration[] =
			"{ \"action\":["
					"{\"m\":\"led\", \"gpio\":\"D1\", \"locoid\":\"70\", \"func\":\"0\"}, "
					"{\"m\":\"servo\", \"gpio\":\"D8\", \"locoid\":\"1\"},"
					"{\"m\":\"turnout\", \"enable\":\"D4\", \"dir1\":\"D1\", \"dir2\":\"D2\", \"addr\":\"4\"},"
					"{\"m\":\"turnout\", \"enable\":\"D3\", \"dir1\":\"D1\", \"dir2\":\"D2\", \"addr\":\"15\"}"
					"]}";


// WLAN-Config
const char* ssid = "Z21_90xx";
const char* password = "xxxxxxxxxxx";


#endif /* WLANCONFIG_H_ */
