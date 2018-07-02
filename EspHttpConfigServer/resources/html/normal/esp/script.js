var hostname = document.getElementById('hostname');
var disabled = document.getElementById('disabled');
var sta_pwd = document.getElementById('sta_pwd');
var sta_ssid = document.getElementById('sta_ssid');
var ap_ssid = document.getElementById('ap_ssid');
var ap_pwd = document.getElementById('ap_pwd');
var ap_pwr = document.getElementById('ap_pwr');
var ipaddr = document.getElementById('ipaddr');
var netmask = document.getElementById('netmask');
var gateway = document.getElementById('gateway');
var dns = document.getElementById('dns');
var pwmSlider = document.getElementById('pwmSlide');
var pwmText = document.getElementById('pwmText');
var pwm_gpio = document.getElementById('pwm_gpio');
var page_gpio = document.getElementById('page_gpio');
var page_conf = document.getElementById('page_conf');
var page_cmd = document.getElementById('page_cmd');
var timed = [];
var ESP_IP = 'http://192.168.4.1';
var URL_API = ESP_IP + '/api';
var URL_CONF = URL_API + '/conf';
var URL_GPIO = URL_API + '/gpio';
var URL_CMD = URL_API + '/cmd';
var URL_CONF_SAVE = URL_CONF + '/save';
var URL_CONF_RESET = URL_CONF + '/reset';
var URL_GPIO_TIME = URL_GPIO + '/time';
var URL_GPIO_STATE = URL_GPIO + '/state';
var URL_GPIO_TOGGLE = URL_GPIO + '/toggle';
var URL_GPIO_PWM = URL_GPIO + '/pwm';
var URL_CMD_LOCATE = URL_CMD + '/locate';
var URL_CMD_REBOOT = URL_CMD + '/reboot';
var URL_CMD_DISABLE = URL_CMD + '/disable';
var URL_CMD_ENABLE = URL_CMD + '/enable';
var timerValue = 0;
function load() {
	readConfiguration();
	readGpioData();
	displayPage('m1');
	setInterval(decreaseTimer, 1000);
}
function displayPage(id) {
	if (id == 'm1') {
		page_gpio.style.visibility = 'collapse';
		page_cmd.style.visibility = 'collapse';
		page_conf.style.visibility = 'visible';
	} else if (id == 'm2') {
		readGpioData();
		page_conf.style.visibility = 'collapse';
		page_cmd.style.visibility = 'collapse';
		page_gpio.style.visibility = 'visible';
	} else if (id == 'm3') {
		page_conf.style.visibility = 'collapse';
		page_gpio.style.visibility = 'collapse';
		page_cmd.style.visibility = 'visible';
	}
}
function hideFields() {
	var wifimode = document.querySelector('input[name=wifimode]:checked').value;
	if (wifimode == 1) {
		sta_pwd.disabled = false;
		sta_ssid.disabled = false;
		ap_pwd.disabled = true;
		ap_ssid.disabled = true;
	} else if (wifimode == 2) {
		sta_pwd.disabled = true;
		sta_ssid.disabled = true;
		ap_pwd.disabled = false;
		ap_ssid.disabled = false;
	} else {
		sta_pwd.disabled = false;
		sta_ssid.disabled = false;
		ap_pwd.disabled = false;
		ap_ssid.disabled = false;
	}
	var ipmode = document.querySelector('input[name=ipconfig]:checked').value == 0;
	if (wifimode == 2 || ipmode) {
		ipaddr.disabled = true;
		netmask.disabled = true;
		gateway.disabled = true;
		dns.disabled = true;
	} else {
		ipaddr.disabled = false;
		netmask.disabled = false;
		gateway.disabled = false;
		dns.disabled = false;
	}
}
function validate() {
	if (0 == hostname.value.length) {
		alert('Name cannot be empty !');
		hostname.select();
		return false;
	}
	var params = '?hostname=' + hostname.value;
	var isDisabled = disabled.checked;
	if (isDisabled) {
		if (!confirm('If module is disabled, no gpio can be modified. Are you sure ?')) {
			return false;
		}
	}
	params += '&disabled=' + (isDisabled ? '1' : '0');
	var wifimode = document.querySelector('input[name=wifimode]:checked').value;
	params += '&wifimode=' + wifimode;
	if (wifimode == 2 || wifimode == 3) {
		if (0 == ap_ssid.value.length) {
			alert('Access point WiFi name cannot be empty !');
			ap_ssid.select();
			return false;
		} else if (0 == ap_pwd.value.length) {
			alert('Access point WiFi password cannot be empty !');
			ap_pwd.select();
			return false;
		} else if (0 == ap_pwr.value.length) {
			alert('Access point WiFi power cannot be empty !');
			ap_pwr.select();
			return false;
		}
		params += '&ap_ssid=' + ap_ssid.value + '&ap_pwd=' + ap_pwd.value
				+ '&ap_pwr=' + ap_pwr.value;
	}
	if (wifimode == 1 || wifimode == 3) {
		if (0 == sta_ssid.value.length) {
			alert('Station WiFi name cannot be empty !');
			sta_ssid.select();
			return false;
		} else if (0 == sta_pwd.value.length) {
			if (!confirm('Station WiFi password is empty, old password will be used ?')) {
				sta_pwd.select();
				return false;
			}
		}
		params += '&sta_ssid=' + sta_ssid.value;
		if (0 != sta_pwd.value.length) {
			params += '&sta_pwd=' + sta_pwd.value;
		}
		var ipconfig = document.querySelector('input[name=ipconfig]:checked').value;
		params += '&ipconfig=' + ipconfig;
		if (ipconfig == 1) {
			if (0 == ipaddr.value.length) {
				alert('Station ip address cannot be empty !');
				ipaddr.select();
				return false;
			} else if (0 == netmask.value.length) {
				alert('Station mask cannot be empty !');
				netmask.select();
				return false;
			} else if (0 == gateway.value.length) {
				alert('Station gateway address cannot be empty !');
				gateway.select();
				return false;
			} else if (0 == dns.value.length) {
				if (!confirm('Station dns cannot be empty to access the Internet !')) {
					dns.select();
					return false;
				}
			}
			params += '&ipaddr=' + ipaddr.value + '&netmask=' + netmask.value
					+ '&gateway=' + gateway.value + '&dns=' + dns.value;
		}
	}
	if (document.getElementById('to_apply').checked) {
		params += '&to_apply=';
	}
	var xhr = new XMLHttpRequest();
	xhr.open('GET', URL_CONF_SAVE + params, true);
	xhr.onprogress = function() {
		updateStatus('Saving configuration...');
	};
	xhr.onload = function() {
		if (xhr.status == 200) {
			updateStatus('Configuration saved');
		} else {
			updateStatus('Fail saving configuration...');
		}
	};
	xhr.send();
	return true;
}
function readPwm(event) {
	if (document.activeElement.id == 'pwmSlide') {
		pwmText.value = pwmSlider.value;
		clearTimeout(timerValue);
		timerValue = setTimeout(sendGpioPwm, 500);
	} else if (typeof event !== 'undefined' && event.keyCode === 13) {
		if (pwmText.value < 0) {
			pwmText.value = 0;
		}
		if (pwmText.value > 1023) {
			pwmText.value = 1023;
		}
		pwmSlider.value = pwmText.value;
		sendGpioPwm();
	}
}
function updateStatus(msg) {
	var statusMsg = document.getElementById('statusMsg');
	if (typeof msg == 'undefined') {
		statusMsg.innerHTML = '';
	} else {
		statusMsg.innerHTML = msg;
		clearTimeout(timerValue);
		timerValue = setTimeout(updateStatus, 3000);
	}
}
function sendGpioValue(gpioId) {
	var gpio = document.getElementById(gpioId);
	var state = gpio.checked == true ? '1' : '0';
	var xhr = new XMLHttpRequest();
	xhr.open('GET', URL_GPIO_STATE + '?gpio=' + gpioId + '&state=' + state,
			true);
	xhr.onprogress = function() {
		updateStatus('Changing ' + gpioId + ' state...');
	};
	xhr.onload = function() {
		var msgState = state == '1' ? 'HIGH' : 'LOW';
		if (xhr.status == 200) {
			updateStatus(gpioId + ' state changed to ' + msgState);
		} else {
			gpio.checked = !gpio.checked;
			updateStatus(gpioId + ' update failed !');
		}
	};
	xhr.send();
}
function sendToggle(gpioId) {
	var xhr = new XMLHttpRequest();
	xhr.open('GET', URL_GPIO_TOGGLE + '?gpio=' + gpioId, true);
	xhr.onprogress = function() {
		updateStatus('Toggling ' + gpioId + ' state...');
	};
	xhr.onload = function() {
		if (xhr.status == 200) {
			response = JSON.parse(xhr.responseText);
			strSate = response.state == 1 ? 'HIGH' : 'LOW';
			updateStatus(gpioId + ' state toggled to ' + strSate);
			var gpio = document.getElementById(gpioId);
			gpio.checked = response.state == 1;
		} else {
			updateStatus(gpioId + ' update failed !');
		}
	};
	xhr.send();
}
function sendGpioTime(event, inputElement) {
	if (event.keyCode !== 13) {
		return false;
	}
	var gpioId = inputElement.id.split('_')[0];
	if (inputElement.value > 65535) {
		inputElement.value = 65535
	} else if (inputElement.value < 0) {
		inputElement.value = -1;
		return false;
	}
	var xhr = new XMLHttpRequest();
	xhr.open('GET', URL_GPIO_TIME + '?gpio=' + gpioId + '&time='
			+ inputElement.value, true);
	xhr.onprogress = function() {
		updateStatus('Changing ' + gpioId + ' timer...');
	};
	xhr.onload = function() {
		if (xhr.status == 200) {
			updateStatus(gpioId + ' timer setted to ' + inputElement.value);
			document.getElementById(gpioId).checked = !document
					.getElementById(gpioId).checked;
			timed[gpioId] = true;
		} else {
			updateStatus(gpioId + ' update failed !');
		}
	};
	xhr.send();
}
function sendGpioPwm() {
	var gpioId = pwm_gpio.options[pwm_gpio.selectedIndex].value;
	var pwm = pwmText.value;
	var xhr = new XMLHttpRequest();
	xhr.open('GET', URL_GPIO_PWM + '?gpio=' + gpioId + '&pwm=' + pwm, true);
	xhr.onprogress = function() {
		updateStatus('Changing ' + gpioId + ' PWM...');
	};
	xhr.onload = function() {
		if (xhr.status == 200) {
			updateStatus(gpioId + ' PWM setted to ' + pwm);
		} else {
			updateStatus(gpioId + ' PWM update failed !');
		}
	};
	xhr.send();
}
function locate() {
	var xhr = new XMLHttpRequest();
	xhr.open('GET', URL_CMD_LOCATE, true);
	xhr.onprogress = function() {
		updateStatus('Sending location command...');
	};
	xhr.onload = function() {
		updateStatus('Module is blinking...');
	};
	xhr.send();
}
function reboot() {
	if (!confirm('Are you sure you want to reboot the module ?')) {
		return false;
	}
	var xhr = new XMLHttpRequest();
	xhr.open('GET', URL_CMD_REBOOT, true);
	xhr.onprogress = function() {
		updateStatus('Sending reboot command...');
	};
	xhr.onload = function() {
		updateStatus('Module is rebooting...');
	};
	xhr.send();
}
function reset() {
	if (!confirm('Are you sure you want to reset module configuration ?')) {
		return false;
	}
	var xhr = new XMLHttpRequest();
	xhr.open('GET', URL_CONF_RESET, true);
	xhr.onprogress = function() {
		updateStatus('Sending reset command...');
	};
	xhr.onload = function() {
		updateStatus('Module is resetting...');
	};
	xhr.send();
}
function enable() {
	var url = disabled.checked ? URL_CMD_DISABLE : URL_CMD_ENABLE;
	var xhr = new XMLHttpRequest();
	xhr.open('GET', url, true);
	xhr.onprogress = function() {
		updateStatus((disabled.checked ? 'Disabling' : 'Enabling') + ' module...');
	};
	xhr.onload = function() {
		if (xhr.status == 200) {
			updateStatus('Module ' + (disabled.checked ? 'disabled' : 'enabled'));
		} else {
			updateStatus('Fail ' + (disabled.checked ? 'disabling' : 'enabling') + ' module');
		}
	};
	xhr.send();
}
function readGpioData() {
	var xhr = new XMLHttpRequest();
	xhr.open('GET', URL_GPIO, true);
	xhr.onprogress = function() {
		updateStatus('Retrieving GPIOs states...');
	};
	xhr.onload = function() {
		if (xhr.status == 200) {
			var gpios = JSON.parse(xhr.response)['gpios'];
			for ( var gpio in gpios) {
				var gpioElmt = document.getElementById(gpio);
				var gpio_time = document.getElementById(gpio + '_time');
				if (gpio != 'A0') {
					gpioElmt.checked = gpios[gpio]['state'];
					gpio_time.value = gpios[gpio]['time'];
					if (gpio_time.value > 0) {
						timed[gpio] = true;
					}
				}
			}
		} else {
			updateStatus('Fail connecting with module');
		}
	};
	xhr.send();
}
function readConfiguration() {
	var xhr = new XMLHttpRequest();
	xhr.open('GET', URL_CONF, true);
	xhr.onprogress = function() {
		updateStatus('Retrieving general configuration...');
	};
	xhr.onload = function() {
		if (xhr.status == 200) {
			var config = JSON.parse(xhr.response)['configuration'];
			if (config == false) {
				updateStatus('Device has no configuration !');
			} else {
				document.getElementById('hostname').value = config['hostname'];
				document.getElementById('uptime').innerHTML = parseDuration(config['uptime']);
				document.getElementById('mac').innerHTML = config['mac'];
				document.getElementById('channel').innerHTML = config['channel'];
				document.getElementById('disabled').checked = config['disabled'] == 1;
				var wifi_mode = 0;
				var wifimode = document.getElementsByName('wifimode');
				if (typeof config['ap'] !== 'undefined') {
					wifi_mode = 1;
					wifimode[1].checked = true;
					document.getElementById('ap_ssid').value = config['ap']['ssid'];
					document.getElementById('ap_pwd').value = config['ap']['password'];
					document.getElementById('ap_pwr').value = config['ap']['power'];
					document.getElementById('ap_ip').innerHTML = config['ap']['ip'];
				}
				if (typeof config['sta'] !== 'undefined') {
					wifi_mode == 0 ? wifimode[0].checked = true
							: wifimode[2].checked = true;
					document.getElementById('sta_ssid').value = config['sta']['ssid'];
					document.getElementById('rssi').innerHTML = config['sta']['rssi']
							+ ' dB';
					document.getElementById('bssid').innerHTML = config['sta']['bssid'];
					var ipmode = config['sta']['mode'];
					var ip_mode = document.getElementsByName('ipconfig');
					ipmode == 'dhcp' ? ip_mode[0].checked = true
							: ip_mode[1].checked = true;
					if (ipmode == 'static') {
						document.getElementById('ipaddr').value = config['sta']['ip'];
						document.getElementById('netmask').value = config['sta']['netmask'];
						document.getElementById('gateway').value = config['sta']['gateway'];
						document.getElementById('dns').value = config['sta']['dns'];
					}
				}
			}
		} else {
			updateStatus('Fail connecting with module');
		}
		hideFields();
	};
	xhr.send();
}
function decreaseTimer() {
	for ( var gpio in timed) {
		if (!timed[gpio]) {
			continue;
		}
		var element = document.getElementById(gpio + '_time');
		if (element.value > 0) {
			element.value--;
			if (element.value == 0) {
				timed[gpio] = false;
				document.getElementById(gpio).checked = !document
						.getElementById(gpio).checked;
			}
		}
	}
}
function parseDuration(duration) {
	var sec_num = parseInt(duration, 10);
	var hours = Math.floor(sec_num / 3600);
	var minutes = Math.floor((sec_num - (hours * 3600)) / 60);
	var seconds = sec_num - (hours * 3600) - (minutes * 60);
	if (hours < 10) {
		hours = '0' + hours;
	}
	if (minutes < 10) {
		minutes = '0' + minutes;
	}
	if (seconds < 10) {
		seconds = '0' + seconds;
	}
	return hours + ':' + minutes + ':' + seconds;
}