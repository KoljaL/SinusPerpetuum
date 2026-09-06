#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>

// Pin-Definitionen
const int TASTER_PIN = 4;
const int MOS_PIN = 5;
const int LED_PIN = 2;

// WLAN Zugangsdaten
const char *ssid = WIFI_SSID;
const char *password = WIFI_PASSWORD;

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
unsigned long gesamtLaufzeit = 0; // In Sekunden

// Taster-Entprellung
bool letzterTasterZustand = HIGH;
bool stabilerTasterZustand = HIGH;
unsigned long letzteDebounceZeit = 0;
const unsigned long DEBOUNCE_DELAY = 50;

// HTML/CSS/JS Weboberfläche
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="de">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Magnet Controller & Log</title>
    <link rel="icon" href="data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 64 64'%3E%3Crect width='64' height='64' rx='12' fill='%230d1117'/%3E%3Cpath d='M18 14v23a14 14 0 0 0 28 0V14h-9v23a5 5 0 0 1-10 0V14z' fill='%23da3633'/%3E%3Cpath d='M18 14h9v12h-9zm19 0h9v12h-9z' fill='%2358a6ff'/%3E%3C/svg%3E">
    <style>
        :root { color-scheme: dark; --bg: #0d1117; --surface: #161b22; --surface-alt: #21262d; --border: #30363d; --text: #e6edf3; --muted: #8b949e; --accent: #58a6ff; --success: #238636; --danger: #da3633; --info: #1f6feb; }
        * { box-sizing: border-box; }
        body { font-family: Arial, sans-serif; text-align: center; background: var(--bg); color: var(--text); padding: 20px; }
        .container { max-width: 500px; margin: 0 auto; }
        .card { background: var(--surface); padding: 20px; border: 1px solid var(--border); border-radius: 10px; box-shadow: 0 4px 14px rgba(0,0,0,0.35); margin-bottom: 20px; }
        .btn { display: inline-block; padding: 12px 24px; font-size: 16px; color: var(--text); border: 1px solid transparent; border-radius: 5px; cursor: pointer; margin: 10px 0; width: 80%; font-weight: bold; }
        .btn-start { background: var(--success); }
        .btn-stop { background: var(--danger); }
        .btn-log { background: var(--info); width: auto; padding: 8px 15px; margin-left: 10px; }
        .btn-session-csv { background: var(--success); font-size: 12px; padding: 4px 10px; border-radius: 3px; border: none; color: var(--text); cursor: pointer; margin-right: 5px; }
        .btn-session-del { background: var(--danger); font-size: 12px; padding: 4px 10px; border-radius: 3px; border: none; color: var(--text); cursor: pointer; }
        .slider-container { margin: 20px 0; text-align: left; }
        .slider { width: 100%; margin-top: 8px; accent-color: var(--accent); }
        .val { font-weight: bold; color: var(--accent); float: right; }
        .grid-status { display: grid; grid-template-columns: 1fr 1fr; gap: 10px; background: var(--surface-alt); padding: 12px; border: 1px solid var(--border); border-radius: 5px; margin-top: 15px; font-size: 15px; }
        .input-group { margin-top: 15px; display: flex; justify-content: center; align-items: center; }
        .input-temp { padding: 8px; width: 80px; font-size: 16px; background: var(--surface-alt); color: var(--text); border: 1px solid var(--border); border-radius: 5px; text-align: center; }
        .input-temp::placeholder { color: var(--muted); }
        .session { margin-top: 25px; text-align: left; background: var(--surface-alt); padding: 15px; border-radius: 8px; border: 1px solid var(--border); }
        .session-header { display: flex; justify-content: space-between; align-items: center; border-bottom: 2px solid var(--accent); padding-bottom: 5px; margin-bottom: 8px; gap: 10px; }
        .session h4 { margin: 0; color: var(--accent); font-size: 16px; }
        table { width: 100%; border-collapse: collapse; margin-top: 5px; font-size: 13px; }
        th, td { border: 1px solid var(--border); padding: 6px; text-align: center; }
        th { background-color: var(--surface); color: var(--text); }
        tr:nth-child(even){background-color: #1b222c;}
        @media (max-width: 520px) {
            .input-group { flex-wrap: wrap; }
            .session-header { align-items: flex-start; flex-direction: column; }
            table { font-size: 11px; }
            th, td { padding: 4px; }
        }
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
                const gespeicherteDatenParsed = gespeicherteDaten ? JSON.parse(gespeicherteDaten) : [];
                if (!Array.isArray(gespeicherteDatenParsed) || gespeicherteDatenParsed.length === 0) {
                    return [];
                }

                if (Array.isArray(gespeicherteDatenParsed[0].eintraege)) {
                    return gespeicherteDatenParsed;
                }

                // Alte Einträge ohne Spannung bleiben in einer Migrations-Session erhalten.
                return [{
                    spannung: 'unbekannt',
                    timestamp: gespeicherteDatenParsed[0].timestamp || Date.now(),
                    eintraege: gespeicherteDatenParsed
                }];
            } catch (fehler) {
                console.warn('Temperatur-Log konnte nicht geladen werden:', fehler);
                return [];
            }
        }

        function speichereTemperaturLog() {
            try {
                localStorage.setItem(TEMPERATUR_LOG_KEY, JSON.stringify(temperaturSessions));
            } catch (fehler) {
                console.error('Temperatur-Log konnte nicht gespeichert werden:', fehler);
                alert('Die Messung konnte im Browser nicht gespeichert werden.');
            }
        }

        function formatiereZeitstempel(timestamp) {
            return new Intl.DateTimeFormat('de-DE', {
                dateStyle: 'short',
                timeStyle: 'medium'
            }).format(new Date(timestamp));
        }

        function rendereTemperaturLog() {
            const container = document.getElementById('sessionContainer');
            container.innerHTML = '';

            if (temperaturSessions.length === 0) {
                container.innerHTML = '<p style="color:#888; margin-top:20px;">Keine Messreihen vorhanden.</p>';
                return;
            }

            temperaturSessions.forEach((session, sessionIndex) => {
                const bereich = document.createElement('section');
                bereich.className = 'session';

                const kopf = document.createElement('div');
                kopf.className = 'session-header';

                const titel = document.createElement('h4');
                titel.innerText = `Session: ${session.spannung} V`;
                kopf.appendChild(titel);

                const aktionen = document.createElement('div');
                const csvButton = document.createElement('button');
                csvButton.className = 'btn-session-csv';
                csvButton.innerText = 'CSV';
                csvButton.onclick = () => exportiereSessionCSV(sessionIndex);

                const loeschButton = document.createElement('button');
                loeschButton.className = 'btn-session-del';
                loeschButton.innerText = 'Löschen';
                loeschButton.onclick = () => loescheSession(sessionIndex);

                aktionen.appendChild(csvButton);
                aktionen.appendChild(loeschButton);
                kopf.appendChild(aktionen);
                bereich.appendChild(kopf);

                const tabelle = document.createElement('table');
                tabelle.innerHTML = `<thead><tr>
                    <th>Zeitpunkt</th>
                    <th>AN/AUS (ms)</th>
                    <th>Duty</th>
                    <th>Laufzeit</th>
                    <th>Temp</th>
                </tr></thead><tbody></tbody>`;
                const tbody = tabelle.getElementsByTagName('tbody')[0];
                session.eintraege.forEach(eintrag => {
                    const zeile = tbody.insertRow();
                    zeile.insertCell(0).innerText = formatiereZeitstempel(eintrag.timestamp);
                    zeile.insertCell(1).innerText = `${eintrag.an} / ${eintrag.aus}`;
                    zeile.insertCell(2).innerText = `${eintrag.dc} %`;
                    zeile.insertCell(3).innerText = formatiereZeit(eintrag.laufzeit);
                    zeile.insertCell(4).innerText = `${eintrag.temperatur} °C`;
                });
                bereich.appendChild(tabelle);
                container.appendChild(bereich);
            });
        }

        function toggleSystem() {
            fetch('/toggle');
        }

        function updateZeit(typ, wert) {
            document.getElementById(typ.toLowerCase() + 'Val').innerText = wert;
            fetch(`/${typ}?wert=${wert}`);
        }

        function formatiereZeit(sek) {
            let m = Math.floor(sek / 60).toString().padStart(2, '0');
            let s = (sek % 60).toString().padStart(2, '0');
            return `${m}:${s}`;
        }

        function logDaten() {
            const voltageVal = Number.parseFloat(document.getElementById('voltageInput').value);
            const tempVal = document.getElementById('tempInput').value;
            if (!Number.isFinite(voltageVal) || voltageVal <= 0) {
                return alert("Bitte zuerst eine gültige Spannung eingeben!");
            }
            if(!tempVal) return alert("Bitte zuerst eine Temperatur eingeben!");

            const spannung = voltageVal.toFixed(1);
            
            fetch('/status').then(response => response.json()).then(data => {
                let session = temperaturSessions.find(eintrag => eintrag.spannung === spannung);
                if (!session) {
                    session = {
                        spannung: spannung,
                        timestamp: Date.now(),
                        eintraege: []
                    };
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
                
                document.getElementById('tempInput').value = ''; // Input leeren
            });
        }

        function loescheSession(sessionIndex) {
            const session = temperaturSessions[sessionIndex];
            if (!session || !confirm(`Möchtest du die Session für ${session.spannung} V wirklich löschen?`)) {
                return;
            }

            temperaturSessions.splice(sessionIndex, 1);
            speichereTemperaturLog();
            rendereTemperaturLog();
        }

        function exportiereSessionCSV(sessionIndex) {
            const session = temperaturSessions[sessionIndex];
            if (!session) {
                return;
            }

            const kopf = 'Spannung(V);Zeitpunkt;AN(ms);AUS(ms);DutyCycle(%);Laufzeit(s);Temperatur(C)';
            const zeilen = session.eintraege.map(eintrag => [
                session.spannung,
                formatiereZeitstempel(eintrag.timestamp),
                eintrag.an,
                eintrag.aus,
                String(eintrag.dc).replace('.', ','),
                eintrag.laufzeit,
                String(eintrag.temperatur).replace('.', ',')
            ].map(wert => `"${String(wert).replaceAll('"', '""')}"`).join(';'));
            const csv = `\uFEFF${[kopf, ...zeilen].join('\n')}`;
            const link = document.createElement('a');
            link.href = `data:text/csv;charset=utf-8,${encodeURIComponent(csv)}`;
            link.download = `messung_${session.spannung}V_${Date.now()}.csv`;
            document.body.appendChild(link);
            link.click();
            document.body.removeChild(link);
        }

        rendereTemperaturLog();

        // Regelmäßiger Datenabgleich mit dem ESP32 (Intervall 500ms für genaue Stoppuhr)
        setInterval(() => {
            fetch('/status').then(response => response.json()).then(data => {
                // Button und Zustand syncen
                const btn = document.getElementById('toggleBtn');
                systemAktiv = data.aktiv;
                if(systemAktiv) {
                    btn.innerText = "STOPP";
                    btn.className = "btn btn-stop";
                } else {
                    btn.innerText = "START";
                    btn.className = "btn btn-start";
                }
                
                // Slider und Werte syncen (nur wenn nicht gerade gezogen wird)
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
    </script>
</body>
</html>
)rawliteral";

String holeDutyCycle()
{
    float gesamt = anZeit + ausZeit;
    if (gesamt == 0)
    {
        Serial.println("[DUTY] Ungueltige Gesamtzeit: 0 ms");
        return "0.0";
    }
    float dc = (anZeit / gesamt) * 100.0;
    static unsigned long letzterAnZeit = 0;
    static unsigned long letzterAusZeit = 0;
    if (anZeit != letzterAnZeit || ausZeit != letzterAusZeit)
    {
        Serial.printf("[DUTY] AN=%lu ms, AUS=%lu ms, Duty Cycle=%.1f %%\n",
                      anZeit, ausZeit, dc);
        letzterAnZeit = anZeit;
        letzterAusZeit = ausZeit;
    }
    return String(dc, 1);
}

// Generated by Copilot
void setSystemAktiv(bool aktiv)
{
    if (systemAktiv == aktiv)
    {
        return;
    }

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
    Serial.println("[SETUP] Pins konfigurieren");
    pinMode(TASTER_PIN, INPUT_PULLUP);
    pinMode(MOS_PIN, OUTPUT);
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(MOS_PIN, LOW);
    digitalWrite(LED_PIN, LOW);

    // 1. WLAN-Verbindung starten
    Serial.print("Verbinde mit WLAN: ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);

    // Warten bis verbunden (LED blinkt währenddessen)
    while (WiFi.status() != WL_CONNECTED)
    {
        digitalWrite(LED_PIN, HIGH);
        delay(150);
        digitalWrite(LED_PIN, LOW);
        delay(150);
        Serial.print(".");
    }

    // Erfolgreich verbunden -> LED bleibt an
    digitalWrite(LED_PIN, HIGH);
    Serial.println("\nErfolgreich mit WLAN verbunden!");
    Serial.print("IP-Adresse des ESP32: http://");
    Serial.println(WiFi.localIP()); // <-- Das ist die IP für deinen Browser!

    // Serial.println("[SETUP] WiFi konfigurieren");
    // bool apGestartet = WiFi.softAP(ssid, password);
    // IPAddress IP = WiFi.softAPIP();
    // Serial.printf("[WIFI] SoftAP %s\n", apGestartet ? "gestartet" : "fehlgeschlagen");
    // Serial.printf("[WIFI] SSID: %s\n", ssid);
    // Serial.printf("[WIFI] IP-Adresse: %s\n", IP.toString().c_str());
    // Serial.printf("[WIFI] Kanal: %d\n", WiFi.channel());

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(200, "text/html", index_html); });

    server.on("/toggle", HTTP_GET, [](AsyncWebServerRequest *request)
              {
        setSystemAktiv(!systemAktiv);
        request->send(200, "text/plain", String(systemAktiv)); });

    server.on("/AN", HTTP_GET, [](AsyncWebServerRequest *request)
              {
        if (!request->hasParam("wert")) {
            request->send(400, "text/plain", "Parameter wert fehlt");
            return;
        }
        long wert = request->getParam("wert")->value().toInt();
        if (wert < 100 || wert > 1500) {
            request->send(400, "text/plain", "AN-Zeit muss zwischen 100 und 1500 ms liegen");
            return;
        }
        anZeit = static_cast<unsigned long>(wert);
        request->send(200, "text/plain", "OK"); });

    server.on("/AUS", HTTP_GET, [](AsyncWebServerRequest *request)
              {
        if (!request->hasParam("wert")) {
            request->send(400, "text/plain", "Parameter wert fehlt");
            return;
        }
        long wert = request->getParam("wert")->value().toInt();
        if (wert < 500 || wert > 2500) {
            request->send(400, "text/plain", "AUS-Zeit muss zwischen 500 und 2500 ms liegen");
            return;
        }
        ausZeit = static_cast<unsigned long>(wert);
        request->send(200, "text/plain", "OK"); });

    server.on("/status", HTTP_GET, [](AsyncWebServerRequest *request)
              {
        unsigned long aktuelleLaufzeit = gesamtLaufzeit;
        if (systemAktiv) {
            aktuelleLaufzeit = (millis() - startZeit) / 1000; // In Sekunden umrechnen
        }
        
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
    // Hardware-Taster abfragen
    unsigned long aktuelleZeit = millis();
    int lesung = digitalRead(TASTER_PIN);
    if (lesung != letzterTasterZustand)
    {
        letzteDebounceZeit = aktuelleZeit;
    }
    if ((aktuelleZeit - letzteDebounceZeit) > DEBOUNCE_DELAY &&
        lesung != stabilerTasterZustand)
    {
        stabilerTasterZustand = lesung;
        if (stabilerTasterZustand == LOW)
        {
            setSystemAktiv(!systemAktiv);
        }
    }
    letzterTasterZustand = lesung;
    // Taktung des Magneten
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
        if (magnetAn)
        {
            digitalWrite(MOS_PIN, LOW);
            digitalWrite(LED_PIN, LOW);
            magnetAn = false;
        }
    }
}