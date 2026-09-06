Hier sind deine Projektdateien direkt als strukturierter Code zum schnellen Kopieren vorbereitet. Damit du keine Code-Anzeigefehler mehr hast, ist das Projekt sauber in drei Dateien aufgeteilt, die du direkt in deine PlatformIO-Ordnerstruktur einfügen kannst.
------------------------------
## 1. platformio.ini (Im Hauptverzeichnis deines Projekts)
Ersetze den Inhalt deiner vorhandenen Konfigurationsdatei vollständig. Der wichtigste Befehl hierbei ist die letzte Zeile (board_build.embed_txtfiles), welche die HTML-Webseite automatisch in den Flash-Speicher deines ESP32 komprimiert.

[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino
monitor_speed = 115200
monitor_rts = 0
monitor_dtr = 0
upload_targets = upload, monitor

lib_deps =
    https://github.com
    https://github.com

; Bindet die index.html Datei beim Kompilieren direkt in den Flash ein
board_build.embed_txtfiles = src/index.html

------------------------------
## 2. index.html (Im Ordner src/)
Erstelle eine neue Datei mit dem Namen index.html exakt im Ordner src/. Durch die Trennung bleibt das JavaScript für die Session-Verwaltung und die Einzel-Exporte völlig fehlerfrei.

<!DOCTYPE html>
<html lang="de">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Magnet Controller & Log</title>
    <style>
        body { font-family: Arial, sans-serif; text-align: center; background: #f4f4f9; color: #333; padding: 20px; }
        .container { max-width: 500px; margin: 0 auto; }
        .card { background: white; padding: 20px; border-radius: 10px; box-shadow: 0 4px 8px rgba(0,0,0,0.1); margin-bottom: 20px; }
        .btn { display: inline-block; padding: 12px 24px; font-size: 16px; color: white; border: none; border-radius: 5px; cursor: pointer; margin: 10px 0; width: 80%; font-weight: bold; }
        .btn-start { background: #28a745; }
        .btn-stop { background: #dc3545; }
        .btn-log { background: #17a2b8; width: auto; padding: 8px 15px; margin-left: 10px; }
        .btn-session-csv { background: #28a745; font-size: 12px; padding: 4px 10px; border-radius: 3px; border: none; color: white; cursor: pointer; margin-right: 5px;}
        .btn-session-del { background: #dc3545; font-size: 12px; padding: 4px 10px; border-radius: 3px; border: none; color: white; cursor: pointer;}
        .slider-container { margin: 20px 0; text-align: left; }
        .slider { width: 100%; margin-top: 8px; }
        .val { font-weight: bold; color: #007bff; float: right; }
        .grid-status { display: grid; grid-template-columns: 1fr 1fr; gap: 10px; background: #e9ecef; padding: 12px; border-radius: 5px; margin-top: 15px; font-size: 15px; }
        .input-group { margin-top: 15px; display: flex; justify-content: center; align-items: center; }
        .input-temp { padding: 8px; width: 80px; font-size: 16px; border: 1px solid #ccc; border-radius: 5px; text-align: center; }
        .session { margin-top: 25px; text-align: left; background: #fff; padding: 15px; border-radius: 8px; border: 1px solid #ddd; }
        .session-header { display: flex; justify-content: space-between; align-items: center; border-bottom: 2px solid #007bff; padding-bottom: 5px; margin-bottom: 8px; }
        .session h4 { margin: 0; color: #007bff; font-size: 16px; }
        table { width: 100%; border-collapse: collapse; margin-top: 5px; font-size: 13px; }
        th, td { border: 1px solid #ddd; padding: 6px; text-align: center; }
        th { background-color: #f8f9fa; color: #333; }
        tr:nth-child(even){background-color: #f9f9f9;}
    </style>
</head>
<body>
    <div class="container">
        <div class="card">
            <h2>🧲 Magnet Prüfstand</h2>
            <button id="toggleBtn" class="btn btn-start" onclick="toggleSystem()">START</button>
            
            <div class="slider-container">
                <label>AN-Zeit: <span id="anVal" class="val">500</span> ms</label>
                <input type="range" min="100" max="1500" step="50" value="500" class="slider" id="anSlider" oninput="updateZeit('AN', this.value)">
            </div>

            <div class="slider-container">
                <label>AUS-Zeit: <span id="ausVal" class="val">1500</span> ms</label>
                <input type="range" min="500" max="2500" step="50" value="1500" class="slider" id="ausSlider" oninput="updateZeit('AUS', this.value)">
            </div>

            <div class="grid-status">
                <div>Laufzeit: <span id="timerVal" style="font-weight:bold;">00:00</span></div>
                <div>Duty Cycle: <span id="dcVal" style="font-weight:bold;">25.0</span> %</div>
            </div>
        </div>

        <div class="card">
            <h3>🌡️ Temperatur loggen</h3>
            <div class="input-group">
                <input type="number" step="0.1" min="0.1" id="voltageInput" class="input-temp" placeholder="24.0">
                <span style="font-size:18px; margin-left:5px; margin-right:10px;">V</span>
                <input type="number" step="0.1" id="tempInput" class="input-temp" placeholder="25.0">
                <span style="font-size:18px; margin-left:5px; margin-right:10px;">°C</span>
                <button class="btn btn-log" onclick="logDaten()">Speichern</button>
            </div>
            <div id="sessionContainer"></div>
        </div>
    </div>

    <script>
        let systemAktiv = false;
        const TEMPERATUR_LOG_KEY = 'magnetkugelbahnTemperaturLog';
        let temperaturSessions = ladeTemperaturSessions();

        function ladeTemperaturSessions() {
            try {
                const gespeicherteDaten = localStorage.getItem(TEMPERATUR_LOG_KEY);
                return gespeicherteDaten ? JSON.parse(gespeicherteDaten) : [];
            } catch (fehler) {
                console.warn('Log-Daten fehlerhaft:', fehler);
                return [];
            }
        }

        function speichereTemperaturLog() {
            localStorage.setItem(TEMPERATUR_LOG_KEY, JSON.stringify(temperaturSessions));
        }

        function formatiereZeitstempel(timestamp) {
            return new Intl.DateTimeFormat('de-DE', {
                hour: '2-digit', minute: '2-digit', second: '2-digit'
            }).format(new Date(timestamp));
        }

        function rendereTemperaturLog() {
            const container = document.getElementById('sessionContainer');
            container.innerHTML = '';

            if (temperaturSessions.length === 0) {
                container.innerHTML = '<p style="color:#888; margin-top:20px;">Keine Messreihen vorhanden.</p>';
                return;
            }

            temperaturSessions.forEach((session, sIdx) => {
                const bereich = document.createElement('section');
                bereich.className = 'session';

                const header = document.createElement('div');
                header.className = 'session-header';

                const titel = document.createElement('h4');
                titel.innerText = '🔋 Session: ' + session.spannung + ' V';
                header.appendChild(titel);

                const btnGroup = document.createElement('div');
                
                const csvBtn = document.createElement('button');
                csvBtn.className = 'btn-session-csv';
                csvBtn.innerText = '📥 CSV';
                csvBtn.onclick = () => exportiereSessionCSV(sIdx);
                
                const delBtn = document.createElement('button');
                delBtn.className = 'btn-session-del';
                delBtn.innerText = '🗑️ Löschen';
                delBtn.onclick = () => loescheSession(sIdx);

                btnGroup.appendChild(csvBtn);
                btnGroup.appendChild(delBtn);
                header.appendChild(btnGroup);
                bereich.appendChild(header);

                const tabelle = document.createElement('table');
                tabelle.innerHTML = '<thead><tr><th>Uhrzeit</th><th>AN/AUS</th><th>Duty</th><th>Laufzeit</th><th>Temp</th></tr></thead><tbody></tbody>';
                
                const tbody = tabelle.getElementsByTagName('tbody')[0];
                session.eintraege.forEach(eintrag => {
                    const zeile = tbody.insertRow();
                    zeile.insertCell(0).innerText = formatiereZeitstempel(eintrag.timestamp);
                    zeile.insertCell(1).innerText = eintrag.an + '/' + eintrag.aus;
                    zeile.insertCell(2).innerText = eintrag.dc + '%';
                    zeile.insertCell(3).innerText = formatiereZeit(eintrag.laufzeit);
                    zeile.insertCell(4).innerText = eintrag.temperatur + ' °C';
                });
                
                bereich.appendChild(tabelle);
                container.appendChild(bereich);
            });
        }

        function toggleSystem() { fetch('/toggle'); }
        function updateZeit(typ, wert) { document.getElementById(typ.toLowerCase() + 'Val').innerText = wert; fetch('/' + typ + '?wert=' + wert); }
        function formatiereZeit(sek) { let m = Math.floor(sek / 60).toString().padStart(2, '0'); let s = (sek % 60).toString().padStart(2, '0'); return m + ':' + s; }

        function logDaten() {
            const voltageVal = Number.parseFloat(document.getElementById('voltageInput').value);
            const tempVal = document.getElementById('tempInput').value;
            if (!Number.isFinite(voltageVal) || voltageVal <= 0) return alert("Bitte gültige Spannung eingeben!");
            if (!tempVal) return alert("Bitte Temperatur eingeben!");

            const spannung = voltageVal.toFixed(1);
            
            fetch('/status').then(response => response.json()).then(data => {
                let session = temperaturSessions.find(e => e.spannung === spannung);
                if (!session) {
                    session = { spannung: spannung, timestamp: Date.now(), eintraege: [] };
                    temperaturSessions.unshift(session);
                }

                session.eintraege.unshift({
                    timestamp: Date.now(),
                    an: data.an,
                    aus: data.aus,
                    dc: data.dc,
                    laufzeit: data.laufzeit,
                    temperatur: tempVal
                });
                speichereTemperaturLog();
                rendereTemperaturLog();
                document.getElementById('tempInput').value = '';
            });
        }

        function loescheSession(idx) {
            if(confirm('Möchtest du die Session für ' + temperaturSessions[idx].spannung + ' V wirklich löschen?')) {
                temperaturSessions.splice(idx, 1);
                speichereTemperaturLog();
                rendereTemperaturLog();
            }
        }

        function exportiereSessionCSV(idx) {
            const session = temperaturSessions[idx];
            let csv = "data:text/csv;charset=utf-8,Spannung(V);Uhrzeit;AN(ms);AUS(ms);DutyCycle(%);Laufzeit(s);Temperatur(C)\n";
            
            session.eintraege.forEach(e => {

csv += session.spannung + ';' + '"' + formatiereZeitstempel(e.timestamp) + '";' + e.an + ';' + e.aus + ';' + e.dc.replace('.',',') + ';' + e.laufzeit + ';' + e.temperatur.replace('.',',') + '\n';
});
const encodedUri = encodeURI(csv);
const link = document.createElement("a");
link.setAttribute("href", encodedUri);
link.setAttribute("download", 'messung_' + session.spannung + 'V_' + Date.now() + '.csv');
document.body.appendChild(link);
link.click();
document.body.removeChild(link);
}
rendereTemperaturLog();
setInterval(() => {
fetch('/status').then(response => response.json()).then(data => {
const btn = document.getElementById('toggleBtn');
systemAktiv = data.aktiv;
if(systemAktiv) { btn.innerText = "STOPP"; btn.className = "btn btn-stop"; }
else { btn.innerText = "START"; btn.className = "btn btn-start"; }
if(document.activeElement !== document.getElementById('anSlider')) {
document.getElementById('anVal').innerText = data.an;
document.getElementById('anSlider').value = data.an;
}
if(document.activeElement !== document.getElementById('ausSlider')) {
document.getElementById('ausVal').innerText = data.aus;
document.getElementById('ausSlider').value = data.aus;
}
document.getElementById('dcVal').innerText = data.dc;
document.getElementById('timerVal').innerText = formatiereZeit(data.laufzeit);
});
}, 500);



---

### 3. `main.cpp` (Im Ordner `src/`)
Ersetze den Inhalt deiner vorhandenen `main.cpp`. Der C++ Code extrahiert nun die HTML-Webseite über einen Low-Level-Assemblerbefehl direkt aus dem Flash-Speicher des Mikrocontrollers (`index_html_start`).

```cpp
#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>

// Pin-Definitionen
const int TASTER_PIN = 4;
const int MOS_PIN = 5;
const int LED_PIN = 2;

// WLAN Zugangsdaten
const char *ssid = "MIA_2";
const char *password = "12345mia12345";

AsyncWebServer server(80);

// Standard-Intervalle
unsigned long anZeit = 500;
unsigned long ausZeit = 1500;

// Variablen für die Zustandsmaschine
bool systemAktiv = false;
bool magnetAn = false;
unsigned long letzteWechselZeit = 0;

// Variablen für die Stoppuhr
unsigned long startZeit = 0;
unsigned long gesamtLaufzeit = 0; 

// Taster-Entprellung
bool letzterTasterZustand = HIGH;
bool stabilerTasterZustand = HIGH;
unsigned long letzteDebounceZeit = 0;
const unsigned long DEBOUNCE_DELAY = 50;

// Trick für PlatformIO: Bindet die index.html Datei direkt in den Flash-Speicher ein
extern const uint8_t index_html_start[] ASM_WEAK_ATTR __asm__("_binary_src_index_html_start");
extern const uint8_t index_html_end[] ASM_WEAK_ATTR __asm__("_binary_src_index_html_end");

String holeDutyCycle()
{
    float gesamt = anZeit + ausZeit;
    if (gesamt == 0) return "0.0";
    float dc = (anZeit / gesamt) * 100.0;
    return String(dc, 1);
}

void setSystemAktiv(bool aktiv)
{
    if (systemAktiv == aktiv) return;

    unsigned long aktuelleZeit = millis();
    systemAktiv = aktiv;

    if (systemAktiv)
    {
        startZeit = aktuelleZeit;
        letzteWechselZeit = aktuelleZeit;
        magnetAn = true;
        digitalWrite(MOS_PIN, HIGH);
        digitalWrite(LED_PIN, HIGH);
    }
    else
    {
        gesamtLaufzeit = (aktuelleZeit - startZeit) / 1000;
        magnetAn = false;
        digitalWrite(MOS_PIN, LOW);
        digitalWrite(LED_PIN, LOW);
    }
}

void setup()
{
    Serial.begin(115200);
    pinMode(TASTER_PIN, INPUT_PULLUP);
    pinMode(MOS_PIN, OUTPUT);
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(MOS_PIN, LOW);
    digitalWrite(LED_PIN, LOW);

    Serial.print("Verbinde mit WLAN: ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        digitalWrite(LED_PIN, HIGH); delay(150);
        digitalWrite(LED_PIN, LOW); delay(150);
        Serial.print(".");
    }

    digitalWrite(LED_PIN, HIGH);
    Serial.println("\nErfolgreich verbunden!");
    Serial.print("IP-Adresse des ESP32: http://");
    Serial.println(WiFi.localIP());

    // Sendet die index.html Datei ressourcenschonend direkt aus dem Flash
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              {
                  size_t html_len = index_html_end - index_html_start;
                  request->send_P(200, "text/html", index_html_start, html_len);
              });

    server.on("/toggle", HTTP_GET, [](AsyncWebServerRequest *request)
              {
        setSystemAktiv(!systemAktiv);
        request->send(200, "text/plain", String(systemAktiv)); });

    server.on("/AN", HTTP_GET, [](AsyncWebServerRequest *request)
              {
        if (!request->hasParam("wert")) return request->send(400, "text/plain", "Fehler");
        long wert = request->getParam("wert")->value().toInt();
        if (wert < 100 || wert > 1500) return request->send(400, "text/plain", "Fehler");
        anZeit = static_cast<unsigned long>(wert);
        request->send(200, "text/plain", "OK"); });

    server.on("/AUS", HTTP_GET, [](AsyncWebServerRequest *request)
              {
        if (!request->hasParam("wert")) return request->send(400, "text/plain", "Fehler");
        long wert = request->getParam("wert")->value().toInt();
        if (wert < 500 || wert > 2500) return request->send(400, "text/plain", "Fehler");
        ausZeit = static_cast<unsigned long>(wert);
        request->send(200, "text/plain", "OK"); });

    server.on("/status", HTTP_GET, [](AsyncWebServerRequest *request)
              {
        unsigned long aktuelleLaufzeit = gesamtLaufzeit;
        if (systemAktiv) aktuelleLaufzeit = (millis() - startZeit) / 1000;
        
        String json = "{\"aktiv\":" + String(systemAktiv ? "true" : "false") + 
                      ",\"an\":" + String(anZeit) + 
                      ",\"aus\":" + String(ausZeit) + 
                      ",\"dc\":\"" + holeDutyCycle() + "\"" +
                      ",\"laufzeit\":" + String(aktuelleLaufzeit) + "}";
        request->send(200, "application/json", json); });

    server.begin();
}

void loop()
{
    unsigned long aktuelleZeit = millis();
    int lesung = digitalRead(TASTER_PIN);
    if (lesung != letzterTasterZustand) { letzteDebounceZeit = aktuelleZeit; }
    
    if ((aktuelleZeit - letzteDebounceZeit) > DEBOUNCE_DELAY && lesung != stabilerTasterZustand)
    {
        stabilerTasterZustand = lesung;
        if (stabilerTasterZustand == LOW) setSystemAktiv(!systemAktiv);
    }
    letzterTasterZustand = lesung;

    if (systemAktiv)
    {
        if (magnetAn && (aktuelleZeit - letzteWechselZeit >= anZeit))
        {
            digitalWrite(MOS_PIN, LOW);
            digitalWrite(LED_PIN, LOW);
            magnetAn = false;
            letzteWechselZeit = aktuelleZeit;
        }
        else if (!magnetAn && (aktuelleZeit - letzteWechselZeit >= ausZeit))
        {
            digitalWrite(MOS_PIN, HIGH);
            digitalWrite(LED_PIN, HIGH);
            magnetAn = true;
            letzteWechselZeit = aktuelleZeit;
        }
    }
    else
    {
        if (magnetAn) { digitalWrite(MOS_PIN, LOW); digitalWrite(LED_PIN, LOW); magnetAn = false; }
    }
}

Wenn du diese Aufteilung nutzt, kannst du das Projekt ganz regulär kompilieren und hochladen. PlatformIO regelt das HTML-Streaming im Hintergrund selbstständig.
Funktioniert der Kompiliervorgang und der automatische Monitor-Reset nun wie gewünscht?

