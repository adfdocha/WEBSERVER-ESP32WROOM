// WEBSERVER + WEBSOCKETS + DHT11 + SOIL(ADC34) + PH(ADC35) + OLED SH1106 (U8g2)
// Para: ESP32-WROOM, sensor suelo en GPIO34, sensor pH en GPIO35, DHT11 en GPIO33

#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <DHT.h>
#include <U8g2lib.h>
#include <Wire.h>

extern String htmlPage;   // ← Declaración global

// ======================
// CONFIG SENSORES
// ======================
#define DHTPIN 33
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

#define SOIL_PIN 34
#define PH_PIN   35



// ======================
// CONFIG WiFi
// ======================
//const char* ssid     = "MEGADEATH";
//const char* password = "4RC4NG31Z0M8";
//const char* ssid     = "System_House_RD";
//const char* password = "Sys_Hou#2024**";
//const char* ssid     = "Adr";
//const char* password = "adrianosqui";
//const char* ssid     = "MONTERREY";
//const char* password = "octavio123*.*";
const char* ssid     = "TG_TIC*";
const char* password = "3143265oj";



// ======================
// SERVIDORES
// ======================
WebServer server(80);
WebSocketsServer webSocket(81);

// ======================
// OLED SH1106 (128x64)
// ======================
U8G2_SH1106_128X64_NONAME_F_HW_I2C oled(U8G2_R0, U8X8_PIN_NONE);

// ======================
// Variables globales
// ======================
float temp = 0.0f;
float hum = 0.0f;
float soilValue = 0.0f;
float ITHValue = 0.0f;
float phValue = 0.0f;

// ======================
// LECTURA HUMEDAD DE SUELO
// ======================
float readSoil() {
  int raw = analogRead(SOIL_PIN);
  float mapped = -0.03454f * raw + 141.45f;

  if (mapped < 0.0f) mapped = 0.0f;
  if (mapped > 100.0f) mapped = 100.0f;

  return round(mapped * 100) / 100.0f;
}

// ======================
// LECTURA SENSOR pH
// ======================
float readPH() {
  int raw = analogRead(PH_PIN);
  float voltage = raw * 3.3f / 4095.0f;

  float ph = 7.0f + ((2.5f - voltage) / 0.18f);

  if (ph < 0.0f) ph = 0.0f;
  if (ph > 14.0f) ph = 14.0f;

  return round(ph * 100) / 100.0f;
}

// ======================
// ACTUALIZAR OLED
// ======================
void updateOLED() {
  oled.clearBuffer();

  // Zona superior — estado WiFi
  oled.setFont(u8g2_font_5x7_tf);

  if (WiFi.status() == WL_CONNECTED) {
    oled.drawStr(0, 10, "WiFi: ON");

    char ipbuf[20];
    WiFi.localIP().toString().toCharArray(ipbuf, 20);
    oled.drawStr(64, 10, ipbuf);
  } else {
    oled.drawStr(0, 10, "WiFi: OFF");
  }

  // Línea separadora
  oled.drawLine(0, 14, 128, 14);

  // Información inferior
  oled.setFont(u8g2_font_5x7_tf);

  char buf[32];

  snprintf(buf, sizeof(buf), "T: %.2f C", temp);
  oled.drawStr(0, 26, buf);

  snprintf(buf, sizeof(buf), "H: %.2f %%", hum);
  oled.drawStr(0, 38, buf);

  snprintf(buf, sizeof(buf), "S: %.2f %%", soilValue);
  oled.drawStr(0, 50, buf);

  snprintf(buf, sizeof(buf), "pH: %.2f", phValue);
  oled.drawStr(0, 62, buf);

  // ITH destacado
  oled.setFont(u8g2_font_8x13B_tf);
  char bufITH[24];
  snprintf(bufITH, sizeof(bufITH), "ITH: %.2f", ITHValue);
  oled.drawStr(50, 50, bufITH);

  oled.sendBuffer();
}

// ======================
// SETUP
// ======================
void setup() {
  Serial.begin(115200);
  dht.begin();

  analogSetPinAttenuation(SOIL_PIN, ADC_11db);
  analogSetPinAttenuation(PH_PIN, ADC_11db);
  analogReadResolution(12);

  oled.begin();
  oled.clearBuffer();
  oled.setFont(u8g2_font_5x7_tf);
  oled.drawStr(0, 20, "Iniciando...");
  oled.sendBuffer();

  // Conexión WiFi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  delay(200);

  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 15000) {
    delay(200);
    Serial.print(".");
  }

  server.on("/", []() { server.send(200, "text/html", htmlPage); });
  server.begin();

  webSocket.begin();

  Serial.println("\nServidor iniciado");
}

// ======================
// LOOP
// ======================
void loop() {
  server.handleClient();
  webSocket.loop();

  float t = dht.readTemperature();
  float h = dht.readHumidity();
  float s = readSoil();
  float p = readPH();

  if (isnan(t) || isnan(h)) {
    Serial.println("Error DHT");
    return;
  }

        // === Fórmula corregida ===
  float ithRaw = (1.8f * t) + 32.0f - ((0.55f * h) * 2.0f) + 100.0f;

  temp = round(constrain(t, -40.0f, 125.0f) * 100) / 100.0f;
  hum  = round(constrain(h, 0.0f, 100.0f) * 100) / 100.0f;
  soilValue = round(constrain(s, 0.0f, 100.0f) * 100) / 100.0f;
  phValue   = round(constrain(p, 0.0f, 14.0f)   * 100) / 100.0f;
  ITHValue  = round(constrain(ithRaw, -1000.0f, 1000.0f) * 100) / 100.0f;

  updateOLED();

  // JSON
  String json = "{\"temp\":";
  json += String(temp, 2);
  json += ",\"hum\":";
  json += String(hum, 2);
  json += ",\"soil\":";
  json += String(soilValue, 2);
  json += ",\"ph\":";
  json += String(phValue, 2);
  json += ",\"ith\":";
  json += String(ITHValue, 2);

  if (WiFi.status() == WL_CONNECTED) {
    json += ",\"ip\":\"";
    json += WiFi.localIP().toString();
    json += "\"";
  }
  json += "}";

  webSocket.broadcastTXT(json);
  Serial.println(json);

  delay(1000);
}


// ======================
//     HTML (RESPONSIVE)
// ======================
String htmlPage = R"=====(
<!DOCTYPE html>
<html>
<head>
<meta charset="utf-8" />
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>ESP32 - Dashboard</title>

<script src="https://cdn.jsdelivr.net/npm/chart.js"></script>

<style>
body {
    font-family: Arial, sans-serif;
    margin: 0;
    padding: 0;
    background: #f2f6fa;
}
.container {
    display: flex;
    flex-direction: row;
    gap: 10px;
    padding: 10px;
    box-sizing: border-box;
    height: calc(100vh - 20px);
}
.left-box {
    flex: 0 0 320px;
    background: white;
    padding: 12px;
    border-radius: 10px;
    box-shadow: 0 0 8px rgba(0,0,0,0.08);
    overflow-y: auto;
}
.right-box {
    flex: 1;
    background: white;
    padding: 12px;
    border-radius: 10px;
    box-shadow: 0 0 8px rgba(0,0,0,0.08);
    overflow-y: auto;
}
.data-box {
    margin-bottom: 10px;
    padding: 10px;
    background: #eef3f8;
    border-radius: 8px;
    font-size: 0.95em;
    display: flex;
    justify-content: space-between;
    align-items: center;
}
.value-number {
    font-size: 1.4rem;
    color: #0b66a3;
    font-weight: 700;
}
.chart-row {
    display: flex;
    flex-direction: row;
    gap: 12px;
    flex-wrap: wrap;
    justify-content: flex-start;
}
/* AJUSTE para acomodar 5 gráficas */<!DOCTYPE html>
.chart-container {
    width: 19.8%; 
    min-width: 180px; 
    height: 170px;
    background: #fafafa;
    padding: 8px;
    border-radius: 8px;
    box-shadow: inset 0 0 0 1px rgba(0,0,0,0.02);
    text-align: center;
}
.chart-title {
    margin-top: 0;
    margin-bottom: 5px;
    font-size: 1.1em;
    color: #333; 
}
@media (max-width: 768px) {
    .container {
        flex-direction: column;
        height: auto;
    }
    .left-box {
        flex: none;
        width: 100%;
    }
    .chart-container {
        width: calc(100% - 30px);
        height: 150px;
        margin: 0 auto 10px auto;
    }
}
.header {
    text-align: center;
    padding-bottom: 8px;
    border-bottom: 1px solid #eee;
    margin-bottom: 10px;
}
</style>

</head>
<body>

<div class="container">

    <div class="left-box">
        <div class="header">
            <h3 style="margin:6px 0;">IOT-la Granja Paicol-SENA-CDATH</h3>
            <small id="ipInfo">IP: -</small>
        </div>

        <div class="data-box">
            <div>Temperatura</div>
            <div id="tempValue" class="value-number">-- °C</div>
        </div>

        <div class="data-box">
            <div>Humedad Ambiente</div>
            <div id="humValue" class="value-number">-- %</div>
        </div>

        <div class="data-box">
            <div>Humedad Suelo</div>
            <div id="soilValue" class="value-number">-- %</div>
        </div>
        
        <div class="data-box">
            <div>PH del Suelo/Agua</div>
            <div id="phValue" class="value-number">--</div>
        </div>
        <div class="data-box">
            <div>ITH</div>
            <div id="ithValue" class="value-number">--</div>
        </div>

        <div class="data-box">
            <div>WiFi</div>
            <div id="wifiStatus" class="value-number">--</div>
        </div>

        <div style="font-size:0.85em; color:#666; margin-top:8px;">
            Actualización: <strong>1s</strong> • Gráficas muestran últimas 20 muestras
        </div>
    </div>

    <div class="right-box">
        <h3 style="margin-top:0;">Gráficas</h3>

        <div class="chart-row">
            <div class="chart-container"><h4 class="chart-title">Temperatura</h4><canvas id="chartTemp"></canvas></div>
            <div class="chart-container"><h4 class="chart-title">Humedad del Aire</h4><canvas id="chartHum"></canvas></div>
            <div class="chart-container"><h4 class="chart-title">Humedad del Suelo</h4><canvas id="chartSoil"></canvas></div>
            
            <div class="chart-container"><h4 class="chart-title">PH</h4><canvas id="chartPH"></canvas></div>
            <div class="chart-container"><h4 class="chart-title">Índice de Temperatura y Humedad (ITH)</h4><canvas id="chartITH"></canvas></div>
        </div>
    </div>

</div>

<script>
(function(){
  // WebSocket to ESP32
  const ws = new WebSocket("ws://" + location.hostname + ":81/");

  // AÑADIR PH a variables de datos
  let labels = [];
  let tempData = [], humData = [], soilData = [], ithData = [], phData = []; 
  const MAX_POINTS = 20;

  function makeChart(canvasId, label, dataArr) {
    const ctx = document.getElementById(canvasId).getContext('2d');
    return new Chart(ctx, {
      type: 'line',
      data: {
        labels: labels,
        datasets: [{
          label: label,
          data: dataArr,
          borderWidth: 2,
          fill: false,
          pointRadius: 2,
          tension: 0.25
        }]
      },
      options: {
        responsive: true,
        maintainAspectRatio: false,
        animation: false,
        plugins: {
          legend: { display: false }
        },
        scales: {
          x: { display: false },
          y: {
            beginAtZero: false // PH y Temp pueden no empezar en cero
          }
        }
      }
    });
  }

  const chartTemp = makeChart("chartTemp", "Temperatura (°C)", tempData);
  const chartHum  = makeChart("chartHum",  "Humedad (%)",       humData);
  const chartSoil = makeChart("chartSoil", "Humedad Suelo (%)", soilData);
  const chartITH  = makeChart("chartITH",  "ITH",               ithData);
  // NUEVA GRÁFICA PH
  const chartPH   = makeChart("chartPH",   "PH",                phData); 

  const tempEl = document.getElementById("tempValue");
  const humEl  = document.getElementById("humValue");
  const soilEl = document.getElementById("soilValue");
  const ithEl  = document.getElementById("ithValue");
  const wifiEl = document.getElementById("wifiStatus");
  const ipInfo = document.getElementById("ipInfo");
  // NUEVO ELEMENTO PH
  const phEl   = document.getElementById("phValue"); 
<!DOCTYPE html>
  ws.onopen = function() {
    console.log("WS abierto");
    wifiEl.textContent = "Conectado";
  };
  ws.onclose = function() {
    console.log("WS cerrado");
    wifiEl.textContent = "Desconectado";
  };

  ws.onmessage = function(event) {
    try {
      const d = JSON.parse(event.data);
      const now = new Date().toLocaleTimeString();

      labels.push(now);
      tempData.push(d.temp);
      humData.push(d.hum);
      soilData.push(d.soil);
      ithData.push(d.ith);
      phData.push(d.ph); // AÑADIR DATO PH

      if (labels.length > MAX_POINTS) {
        labels.shift();
        tempData.shift();
        humData.shift();
        soilData.shift();
        ithData.shift();
        phData.shift(); // ELIMINAR DATO PH ANTIGUO
      }

      chartTemp.update();
      chartHum.update();
      chartSoil.update();
      chartITH.update();
      chartPH.update(); // ACTUALIZAR GRÁFICA PH

      tempEl.textContent = d.temp.toFixed(2) + " °C";
      humEl.textContent  = d.hum.toFixed(2)  + " %";
      soilEl.textContent = d.soil.toFixed(2) + " %";
      ithEl.textContent  = d.ith.toFixed(2);
      phEl.textContent   = d.ph.toFixed(2); // ACTUALIZAR TEXTO PH

      if (d.ip) ipInfo.textContent = "IP: " + d.ip;
    } catch (e) {
      console.error("JSON parse error", e);
    }
  };

})();
</script>

</body>
</html>
)=====";