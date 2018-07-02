var URL_API = 'http://localhost:8888/api';

var URL_UPDATE_DISTANCE = URL_API + '/distance/update/';
var URL_UPDATE_TEMPERATURE = URL_API + '/temperature/update/';
var URL_ACTIVATE_PROGRAM = URL_API + '/status/activate';
var URL_DEACTIVATE_PROGRAM = URL_API + '/status/deactivate';
var URL_GET_CURRENT_DISTANCE = URL_API + '/distance/get-current';
var URL_GET_CURRENT_TEMPERATURE = URL_API + '/temperature/get-current';
var URL_GET_CURRENT_HUMIDITY = URL_API + '/humidity/get-current';
var URL_GET_MINIMUM_DISTANCE = URL_API + '/distance/get-minimum';
var URL_GET_MINIMUM_TEMPERATURE = URL_API + '/temperature/get-minimum';
var URL_GET_STATUS_PROGRAM = URL_API + '/status/get';

var INPUT_UPDATE_TEMPERATURE = document.getElementById('inputUpdateTemperature');
var INPUT_UPDATE_DISTANCE = document.getElementById('inputUpdateDistance');

var BUTTON_UPDATE_TEMPERATURE = document.getElementById('buttonUpdateTemperature');
var BUTTON_UPDATE_DISTANCE = document.getElementById('buttonUpdateDistance');
var BUTTON_ACTIVATE_PROGRAM = document.getElementById('buttonActivateProgram');
var BUTTON_DEACTIVATE_PROGRAM = document.getElementById('buttonDeactivateProgram');

var TEXT_CURRENT_TEMPERATURE = document.getElementById('textCurrentTemperature');
var TEXT_CURRENT_DISTANCE = document.getElementById('textCurrentDistance');
var TEXT_CURRENT_STATUS = document.getElementById('textCurrentStatus');
var TEXT_CURRENT_HUMIDITY = document.getElementById('textCurrentHumidity');
var TEXT_MINIMUM_TEMPERATURE = document.getElementById('textMinimumTemperature');
var TEXT_MINIMUM_DISTANCE = document.getElementById('textMinimumDistance');


window.addEventListener('load', function () {

    BUTTON_ACTIVATE_PROGRAM.addEventListener('click', function () {
       console.log('ACTIVATE');
       activateProgram();
    });

    BUTTON_DEACTIVATE_PROGRAM.addEventListener('click', function () {
        console.log('DEACTIVATE');
        deactivateProgram();
    });

    BUTTON_UPDATE_DISTANCE.addEventListener('click', function () {
        console.log('UPDATE DISTANCE');
        updateDistance(INPUT_UPDATE_DISTANCE.value);
    });

    BUTTON_UPDATE_TEMPERATURE.addEventListener('click', function () {
        console.log('UPDATE TEMPERATURE');
        updateDistance(INPUT_UPDATE_TEMPERATURE.value);
    });

    getStatusProgram();
    getMinimumTemperature();
    getMinimumDistance();
    getCurrentDistance();
    getCurrentHumidity();
    getCurrentTemperature();

}, false);

function xhrRequest(method, url, asc, data, succ, err) {
    var req = new XMLHttpRequest();
    req.open(method, url, asc);
    req.setRequestHeader("Content-Type", "application/json");
    req.send(data);

    if (req.addEventListener("load", function (event) {
        console.log(event);
        if (req.status === 200) {
            succ(req);
            console.log("Réponse reçue: %s", req.responseText);
        } else {
            err(req.status);
            console.log("Status de la réponse: %d (%s)", req.status, req.statusText);
        }
    }));

}

function activateProgram() {
    xhrRequest('GET', URL_ACTIVATE_PROGRAM, true, null, function (succ) {
        TEXT_CURRENT_STATUS.innerText = "WORKING";
    }, function (err) {
        TEXT_CURRENT_STATUS.innerText = "ERROR";
        console.log(err);
    });
}

function deactivateProgram() {
    xhrRequest('GET', URL_DEACTIVATE_PROGRAM, true, null, function (succ) {
        TEXT_CURRENT_STATUS.innerText = "NOT WORKING";
    }, function (err) {
        TEXT_CURRENT_STATUS.innerText = "ERROR";
        console.log(err);
    });
}

function updateDistance(distance) {
    var data = {data: distance};
    xhrRequest('POST', URL_UPDATE_DISTANCE, true, data, function (succ) {
        TEXT_CURRENT_DISTANCE.innerText = succ.target.responseText;
    }, function (err) {
        TEXT_CURRENT_DISTANCE.innerText = "ERROR";
        console.log(err);
    });
}

function updateTemperature(temperature) {
    var data = {data: temperature};
    xhrRequest('POST', URL_UPDATE_TEMPERATURE, true, data, function (succ) {
        TEXT_CURRENT_TEMPERATURE.innerText = succ.target.responseText;
    }, function (err) {
        TEXT_CURRENT_TEMPERATURE.innerText = "ERROR";
        console.log(err);
    });
}

function getCurrentTemperature() {
    xhrRequest('GET', URL_GET_CURRENT_TEMPERATURE, true, null, function (succ) {
        TEXT_CURRENT_TEMPERATURE.innerText = succ.target.responseText;
    }, function (err) {
        TEXT_CURRENT_TEMPERATURE.innerText = "ERROR";
        console.log(err);
    });
}

function getCurrentDistance() {
    xhrRequest('GET', URL_GET_CURRENT_DISTANCE, true, null, function (succ) {
        TEXT_CURRENT_DISTANCE.innerText = succ.target.responseText;
    }, function (err) {
        TEXT_CURRENT_DISTANCE.innerText = 'ERROR';
        console.log(err);
    });
}

function getMinimumTemperature() {
    xhrRequest('GET', URL_GET_MINIMUM_TEMPERATURE, true, null, function (succ) {
        TEXT_MINIMUM_TEMPERATURE.innerText = succ.target.responseText;
    }, function (err) {
        TEXT_MINIMUM_TEMPERATURE.innerText = 'ERROR';
        console.log(err);
    });
}

function getMinimumDistance() {
    xhrRequest('GET', URL_GET_MINIMUM_DISTANCE, true, null, function (succ) {
        TEXT_MINIMUM_DISTANCE.innerText = succ.target.responseText;
    }, function (err) {
        TEXT_MINIMUM_DISTANCE.innerText = 'ERROR';
        console.log(err);
    });
}

function getStatusProgram() {
    xhrRequest('GET', URL_GET_STATUS_PROGRAM, true, null, function (succ) {
        TEXT_CURRENT_STATUS.innerText = succ.target.responseText;
    }, function (err) {
        TEXT_CURRENT_STATUS.innerText = 'ERROR';
        console.log(err);
    });
}

function getCurrentHumidity() {
    xhrRequest('GET', URL_GET_CURRENT_HUMIDITY, true, null, function (succ) {
        TEXT_CURRENT_HUMIDITY.innerText = succ.target.responseText;
    }, function (err) {
        TEXT_CURRENT_HUMIDITY.innerText = 'ERROR';
        console.log(err);
    });
}