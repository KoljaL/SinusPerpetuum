# Review zu `animation.md`

Geprüft am 2026-09-08 gegen die Implementierung in [animation.html](animation.html) und die im Dokument angegebenen Formeln. Das Review trennt nachprüfbare Code-Abweichungen von physikalischen Annahmen, die ohne Vermessung des realen Aufbaus nicht bestätigt werden können.

## Ergebnis

Die mathematische Kosinusfunktion und die Zahlenwerte der angegebenen Standardparameter sind in sich weitgehend stimmig. Mehrere Aussagen beschreiben jedoch nicht das aktuelle Verhalten des Simulators oder setzen eine Kontaktgeometrie voraus, die aus der angegebenen lichten Spurweite allein nicht folgt.

## Feststellungen

### 1. Persistenz funktioniert aktuell nicht

**Befund:** `animation.md` behauptet, dass Einstellungen nach dem Neuladen erhalten bleiben. In `animation.html` wird jedoch beim Start unmittelbar `localStorage.clear()` aufgerufen. Dadurch werden die zuvor gespeicherten Werte und außerdem alle anderen Local-Storage-Einträge der Domain gelöscht, bevor `loadSettings()` sie lesen kann. Die `saveSettings()`-Aufrufe sind daher dauerhaft wirkungslos.

**Bewertung:** Sachlich falsch und zusätzlich destruktiv gegenüber fremden Local-Storage-Daten.

### 2. Startposition und effektiver Rollradius weichen vom Code ab

**Befund:** Die Dokumentation nennt $x = r_{\mathrm{eff}}$ und den Scheitelpunkt als Startposition. `resetSimulation()` setzt dagegen `ballX = radiusCm`. Beim Standarddurchmesser ist das $2{,}4\,\text{cm}$ statt des dokumentierten $r_{\mathrm{eff}} \approx 1{,}6424\,\text{cm}$. Außerdem liegt kein Scheitelpunkt bei $x = 2{,}4\,\text{cm}$; der erste Scheitelpunkt der Kosinusfunktion liegt bei $x = 0$.

**Bewertung:** Direkte Abweichung zwischen Dokumentation und Implementierung.

### 3. Zwei-Punkt-Auflage und $r_{\mathrm{eff}}$ sind im Simulator nicht implementiert

**Befund:** Die Simulation verwendet eine einzelne mathematische Bahn, zeichnet eine Kugel und setzt `radiusCm` als Abstand zur Bahn. Es gibt keine beiden Rundstäbe, keinen Schienendurchmesser und keine Kontaktberechnung. Der Wert `r_eff` kommt im Code nicht vor.

Die Rechnung

$$
r_{\mathrm{eff}} = \sqrt{R^2 - \left(\frac{d}{2}\right)^2}
$$

ist unter der zusätzlichen Annahme korrekt, dass $d/2$ der seitliche Abstand des Kugelmittelpunkts zu jedem Kontaktpunkt ist. Bei einer **lichten** Spurweite muss jedoch auch der Durchmesser der Rundstäbe und die tatsächliche Kontaktlage berücksichtigt werden. Die angegebene Rechnung ist deshalb keine allgemeingültige Folge aus den genannten Maßen.

**Bewertung:** Als vereinfachtes Modell kennzeichnen; nicht als exakt reale Kontaktgeometrie darstellen.

### 4. Die angekündigte Trägheitskorrektur fehlt, der Code verwendet weiterhin $5/7$

**Befund:** `animation.md` weist selbst darauf hin, dass die Anpassung für den kleineren Rollradius später implementiert werden soll. `animation.html` verwendet in der Beschleunigung weiterhin fest `5.0 / 7.0`. Damit simuliert der Code eine Vollkugel mit dem klassischen Abrollradius $R$, nicht das beschriebene Zwei-Schienen-Modell.

Unter dem vereinfachten Modell mit festem $r_{\mathrm{eff}}$ wäre der Faktor stattdessen

$$
\frac{1}{1 + I/(m r_{\mathrm{eff}}^2)}
= \frac{1}{1 + \frac{2}{5}(R/r_{\mathrm{eff}})^2}
\approx 0{,}539,
$$

also kleiner als $5/7 \approx 0{,}714$. Das Ergebnis gilt nur unter genau diesem kinematischen Modell und ersetzt keine vollständige Kontaktanalyse.

### 5. Die Randbedingung ist keine mechanisch geschlossene Senke

**Befund:** $y(0)=y(L)=+A$ beschreibt an beiden Enden einen Berg, keine Senke. Die Aussage, die Randbedingung halte die Kugel in einer geschlossenen Senke, folgt daher nicht aus der Kosinusfunktion. Im Simulator wird die Kugel bei `radiusCm` und `trackLengthCm - radiusCm` hart begrenzt und mit `-ballV * 0.4` zurückgeworfen.

**Bewertung:** Die Begrenzung ist eine numerische Stoßregel mit zusätzlichem Energieverlust, keine aus der Bahngeometrie entstehende mechanische Endbegrenzung. Sie ist außerdem ein dritter Dämpfungsmechanismus neben Rollreibung und Luftwiderstand.

### 6. `numPeaks = 7` ist terminologisch missverständlich

Für $y=A\cos(6\pi x/L)$ gibt es im Intervall $[0,L]$ vier Maxima bei $0$, $L/3$, $2L/3$ und $L$ sowie drei Minima. Die Beschreibung der drei Täler, zwei inneren vollständigen Berge und zwei Randhälften ist als Segmentzählung konsistent. `numPeaks` bezeichnet im Code aber nicht sieben lokale Wellenberge, sondern bestimmt sieben Halbperioden beziehungsweise sechs $\pi$-Phasenintervalle.

**Bewertung:** Formel korrekt, Bezeichnung sollte präzisiert werden, etwa als `numHalfWaves` oder als Anzahl der Extremabschnitte.

### 7. $k$ ist eine Wellenzahl, keine Kreisfrequenz

Die Einheit $\text{cm}^{-1}$ und die Verwendung in $\cos(kx)$ sind korrekt. Da $k$ keine Zeitabhängigkeit besitzt, ist die Bezeichnung „Kreisfrequenz“ fachlich unpassend. „Wellenzahl“ oder „räumliche Kreisfrequenz“ ist konsistenter.

Außerdem beschreibt $L=100\,\text{cm}$ im Code die horizontale $x$-Ausdehnung. Die tatsächliche gekrümmte Bahnlänge ist geringfügig größer; „Schienenweg“ sollte daher nicht mit dieser horizontalen Länge gleichgesetzt werden.

### 8. Die Bewegungsgleichung behandelt die Koordinate nicht konsistent

`ballV` wird durch `ballX += ballV * dt` als Geschwindigkeit in horizontaler $x$-Richtung verwendet. `accGravity` wird aber aus der tangentialen Hangabtriebskomponente mit $\sin(\alpha)$ gebildet. Für eine gekrümmte Bahn müssten Tangentialgeschwindigkeit, Umrechnung auf $x$ und der geschwindigkeitsabhängige Krümmungsterm zusammen behandelt werden. Dasselbe betrifft Rollreibung und Luftwiderstand, die `ballV` als Bahngeschwindigkeit verwenden.

**Bewertung:** Die Simulation ist ein vereinfachtes Näherungsmodell; die Formulierung „exakt“ beziehungsweise „realistisch“ ist dafür zu stark.

### 9. Rollreibung ist als Modellannahme zu kennzeichnen

Die verwendete Beziehung $F_{\text{Roll}}=\mu_Rmg\cos(\alpha)$ ist eine plausible Näherung für eine einfache geneigte Bahn. Auf der gekrümmten Bahn hängt die Normalkraft zusätzlich von der Krümmung und der Geschwindigkeit ab. Bei Stillstand setzt der Code die Reibungsbeschleunigung wegen `Math.sign(ballV) === 0` auf null; eine statische Reibung oder ein explizites Haftkriterium wird nicht modelliert.

### 10. Luftwiderstand: Einheiten stimmen, Gültigkeitsbereich ist nicht belegt

Die im Code verwendete Luftdichte `0.0012 g/cm^3` entspricht $1{,}2\,\text{mg/cm}^3$ beziehungsweise ungefähr $1{,}2\,\text{kg/m}^3$. Auch die Stirnfläche $A=\pi R^2$ und die quadratische Geschwindigkeitsabhängigkeit sind formal konsistent.

Der Wert $c_w=0{,}47$ ist aber kein universell konstanter Materialwert. Er hängt unter anderem von Reynolds-Zahl, Oberflächenzustand und Strömungsbereich ab. Die Behauptung sehr hoher Versuchsgeschwindigkeiten ist aus dem Dokument und dem Code nicht ableitbar.

## Nicht aus dem Repository verifizierbare Angaben

Material, reale Maße, Klebeverbindung, Nutgeometrie, tatsächliche Kontaktzahl und der Literaturwert für die Rollreibung sind im Repository nicht durch Messprotokolle, Zeichnungen oder Quellen belegt. Diese Angaben sollten als Versuchsaufbau-Annahmen, Messwerte oder externe Quellen gekennzeichnet werden. Besonders der Ausschluss eines Kontakts mit Holz oder Nutgrund benötigt eine Toleranz- und Abstandsprüfung des realen Aufbaus.
