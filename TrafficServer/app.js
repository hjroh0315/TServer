const express = require('express')
const app = express()
const fs = require('fs');
const path = require('path')
var bodyParser = require('body-parser')
const port = 3000
const interval = 30
var currTime = 0
const grLight_lLim = 10
const grLight_uLim = 20
var patMax = 0
var pplCnt = 0
var grLightStd = grLight_lLim
var grLight = 'red'

app.use(bodyParser.urlencoded({ extended: false }))
app.use(bodyParser.json());



app.get('/addpatient', (request, response) => {
    response.sendFile(path.join(__dirname + '/index.html'));
})

app.get('/api/light', (request, response) => {
    response.send(grLight);
})

app.post("/api/patient", (request, response) => {
    patMax = Math.max(patMax, request.body.time);
})

app.listen(port, (err) => {
    if (err) {
        return console.log('something bad happened', err);
    }

    console.log(`server is listening on ${port}`);
})

function grLightUpdate() {
    if (grLight == 'red') {
        grLightStd = Math.min(grLight_lLim + Math.max((Math.floor(pplCnt / 5) * 2),patMax), grLight_uLim);
    }
    else {
        patMax = 0;
    }
    if (grLightStd < currTime) grLight = 'green';
    else grLight = 'red';
}

function pplUpdate() {
    //pplCnt = 0;
    fs.readFile('pplCount.txt', 'utf8', function (err, data) {
        pplCnt = parseInt(data);
    });
}

function timeUpdate() {
    grLightUpdate();
    pplUpdate();
    currTime++;
    if (currTime > interval) currTime = 1;
    console.log("Time: " + currTime + ", People:" + pplCnt +
        "\n Patient Max Bias: " + patMax + ", grLight Standard: " + grLightStd +
        "\n grLight: " + grLight);
}


//setInterval(grLightUpdate, 1000)
//setInterval(pplUpdate, 1000)
setInterval(timeUpdate, 1000)
