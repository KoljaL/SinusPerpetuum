# Dokumentation: Kugel-Simulator auf sinusförmiger Bahn

Diese Dokumentation beschreibt den angenommenen physikalischen Versuchsaufbau einer Kugelrollbahn sowie die mathematischen und physikalischen Grundlagen, die für die digitale 2D-Simulation in JavaScript herangezogen werden. Die Simulation ist ein vereinfachtes Näherungsmodell und keine vollständige Kontakt- oder Mehrkörperberechnung.

---

## 1. Aufbau der Bahn in der realen Welt

Der mechanische Versuchsaufbau besteht aus einer Trägerkonstruktion, einem Führungsschienensystem und einem präzise definierten Rollkörper. Als Grundlage für diese Beschreibung dienen die Standardwerte (Default-Werte) des Systems.

### 1.1 Trägerbahn und Nut

- **Material des Trägers:** Massivholz.
- **Geometrische Breite:** 40 mm.
- **Konstruktionsmerkmal:** In der Mitte des Holzträgers befindet sich eine eingefräste Längsnut. Diese Nut dient ausschließlich als Freiraum (Freisparung), damit die Kugel während der Bewegung zu keinem Zeitpunkt den Holzboden berührt. Das Holz übernimmt somit eine reine Haltefunktion für die Schienen und übt keine Reibungskräfte auf den Rollkörper aus.

### 1.2 Schienensystem (Die Spur)

- **Material der Schienen:** Zwei parallel verlaufende Rundeisen (Rundstäbe aus Eisen).
- **Durchmesser der Rundstäbe:** 2 mm pro Stab.
- **Montage:** Die Schienen sind parallel im exakten Abstand auf dem Holzträger fixiert (verklebt).
- **Spurweite (Abstand $d$):** Der lichte Abstand zwischen den beiden Rundeisen beträgt **35 mm** ($d = 3{,}5\,	ext{cm}$).

### 1.3 Der Rollkörper (Die Kugel)

- **Material:** Geschliffener Wälzlagerstahl.
- **Masse ($m$):** 450 g ($0{,}45\,	ext{kg}$).
- **Äußerer Durchmesser ($D$):** 4{,}8 cm.
- **Äußerer Kugelradius ($R$):** 2{,}4 cm.

### 1.4 Kontaktgeometrie und Lagerung
Für das vereinfachte Modell wird angenommen, dass die Kugel zwischen den beiden Rundstäben liegt und an beiden Schienen geführt wird. Aus der lichten Spurweite allein folgen jedoch weder die exakte Anzahl der Kontaktpunkte noch deren Lage: Dafür müssten auch Rundstegdurchmesser, Toleranzen und die tatsächliche Geometrie des Aufbaus bekannt sein. Ein Kontakt mit Holz oder Nutgrund wird in der Simulation nicht modelliert, ist für den realen Aufbau aber durch eine Abstands- und Toleranzprüfung zu verifizieren.


## 2. Mathematische Beschreibung der Bahngeometrie

Die Schienenkonstruktion beschreibt im ungeneigten Zustand eine modifizierte Sinuskurve (Kosinusphase), die über eine horizontale Gesamtlänge von **100 cm** definiert ist.

### 2.1 Koordinatensystem

- Die horizontale Achse wird als $x$-Achse definiert, wobei $x$ von $0\,	ext{cm}$ (linker Rand) bis $100\,	ext{cm}$ (rechter Rand) verläuft.
- Die vertikale Achse wird als $y$-Achse definiert. Der Nullpunkt ($y = 0$) beschreibt die vertikale Mittellinie (Symmetrieachse) der Welle.

### 2.2 Die Wellenfunktion
Die vertikale Höhe $y$ an der horizontalen Position $x$ wird durch folgende Kosinusfunktion beschrieben:

$$
y(x) = A \cdot \cos(kx)
$$

Wobei:
- **$A$ (Amplitude):** $2{,}0\,	ext{cm}$ (maximale Auslenkung nach oben und unten).
- **$numPeaks$ (Wellenparameter im Code):** $7$. Im Code bestimmt dieser Wert die Anzahl der Halbwellensegmente: $numPeaks - 1 = 6$ Phasenintervalle von jeweils $\pi$. Dadurch beginnt die Bahn links ($x = 0$) mit einem Maximum und endet rechts ($x = 100$) wieder mit einem Maximum.
- **$k$ (Wellenzahl beziehungsweise räumliche Kreisfrequenz):**

$$
k = rac{(numPeaks - 1)\pi}{L}
	= rac{6\pi}{100\,	ext{cm}}
	pprox 0{,}1885\,	ext{cm}^{-1}
$$

Hierbei ist $L = 100	ext{ cm}$ die horizontale Gesamtlänge.

### 2.3 Geometrische Ausprägung bei $numPeaks = 7$
Durch das Einsetzen von $numPeaks = 7$ in die Kosinusfunktion ergibt sich auf der horizontalen Ausdehnung von 100 cm folgende Folge von Extremstellen:
1. **Bei $x = 0\,	ext{cm}$:** Ein lokales Maximum ($y = +2\,	ext{cm}$), welches den **ersten halben Berg** links bildet.
2. **Im Verlauf:** Es folgen drei lokale Minima beziehungsweise Wellentäler ($y = -2\,	ext{cm}$) und zwei innere lokale Maxima beziehungsweise Wellenberge ($y = +2\,	ext{cm}$).
3. **Bei $x = 100\,	ext{cm}$:** Die Funktion endet exakt wieder auf einem lokalen Maximum ($y = +2\,	ext{cm}$), was den **letzten halben Berg** rechts abbildet.

Diese mathematische Randbedingung bildet keine mechanisch geschlossene Senke. Der nutzbare Bahnbereich für den Kugelmittelpunkt ist $R \leq x \leq L-R$. Wird dieser Bereich überschritten, verlässt die Kugel die offene Bahn; es gibt weder eine künstliche Umkehr noch einen Energieverlust durch eine numerische Randwand.


## 3. Physik der Rollbewegung und der effektive Rollradius

### 3.1 Bahnkoordinate und lokale Geometrie

Die Dynamik wird in der **Bahnbogenlänge** $s$ formuliert. Die horizontale Koordinate $x$ ist nur noch eine Darstellungs- und Geometriekoordinate. Für

$$
y(x) = A\cos(kx),
\qquad
k = rac{(numPeaks-1)\pi}{L}
$$

gelten

$$
y'(x) = -Ak\sin(kx),
\qquad
y''(x) = -Ak^2\cos(kx).
$$

Die Bogenlänge wird durch

$$
rac{ds}{dx} = q(x) = \sqrt{1 + y'(x)^2}
$$

definiert. Der Code berechnet $s(x)$ numerisch in einer Arc-Length-Tabelle und invertiert diese Tabelle für die Darstellung zu $x(s)$. Dadurch werden Geschwindigkeit und Ortsänderung nicht mehr in verschiedenen Koordinatensystemen vermischt.

Der lokale Bahnwinkel im geneigten Aufbau ist

$$
lpha(x) = rctan(y'(x)) + 	heta,
$$

wobei $	heta$ die einstellbare Gesamtneigung der Bahn ist. Der Tangentialeinheitsvektor lautet

$$
\mathbf t = (\coslpha,\,\sinlpha).
$$

### 3.2 Der effektive Rollradius ($r_{\mathrm{eff}}$)

Unter der Modellannahme, dass die halbe lichte Spurweite dem seitlichen Abstand der Kontaktlage entspricht, rotieren die Kontaktpunkte auf einem kleineren Kreisradius als der geometrische Außenradius ($R = 24\,	ext{mm}$) der Kugel.

$$
r_{\mathrm{eff}} = \sqrt{R^2 - \left(rac{d}{2}ight)^2}
$$

Für die Standardwerte ergibt sich

$$
r_{\mathrm{eff}}
= \sqrt{2{,}4^2 - 1{,}75^2}\,	ext{cm}
= \sqrt{2{,}6975}\,	ext{cm}
pprox 1{,}6424\,	ext{cm}.
$$

Die Formel ist eine Modellannahme und keine allgemeine Kontaktberechnung für eine lichte Spurweite. Ist die Kugel nicht größer als die halbe Spurweite, verwendet der Code ersatzweise den Außenradius.

Die Kugelposition wird für die Darstellung um $r_{\mathrm{eff}}$ in Normalenrichtung vom Bahnpunkt versetzt. Die Rollkinematik verwendet dagegen direkt die Bahnbogenlänge:

$$
\omega = rac{v}{r_{\mathrm{eff}}},
\qquad v = \dot{s}.
$$

### 3.3 Trägheitsmoment und tangentiale Bewegungsgleichung

Für eine homogene Vollkugel gilt

$$
I = rac{2}{5}mR^2.
$$

Der effektive Trägheitsfaktor des vereinfachten Rollmodells ist

$$
f_{\mathrm{acc}}
= rac{1}{1 + I/(m r_{\mathrm{eff}}^2)}
= rac{1}{1 + 0{,}4(R/r_{\mathrm{eff}})^2}.
$$

Die tangentiale Gewichtskraft und die daraus resultierende Beschleunigung sind

$$
F_{g,t} = -mg\sin(lpha),
\qquad
a_s = f_{\mathrm{acc}}rac{F_{t,\mathrm{gesamt}}}{m}.
$$

Alle Kräfte $F_{t,\mathrm{gesamt}}$ werden entlang derselben Tangente addiert. Der Code verwendet eine Euler-Cromer-Integration mit festem Zeitschritt $\Delta t = 1/240\,	ext{s}$:

$$
v_{n+1} = v_n + a_s(s_n,v_n)\Delta t,
\qquad
s_{n+1} = s_n + v_{n+1}\Delta t.
$$

Erst danach wird $s_{n+1}$ über die Arc-Length-Tabelle nach $x$ umgerechnet und zusammen mit $y(x)$, dem Normalenversatz und $	heta$ in Bildschirmkoordinaten transformiert.

### 3.4 Krümmung und dynamische Normalkraft

Die signierte Krümmung der Graphkurve ist

$$
\kappa(x) = rac{y''(x)}{\left(1+y'(x)^2ight)^{3/2}}.
$$

Der zugehörige **signierte** Krümmungsradius ist

$$
R_k = rac{1}{\kappa}.
$$

Für $\kappa=0$ wird der Krümmungsbeitrag null gesetzt. Mit dem Vorzeichen der Krümmung lautet die Normalkraft des Schienenkontakts

$$
F_N = mg\cos(lpha) + rac{mv^2}{R_k}
= mg\cos(lpha) + mv^2\kappa.
$$

In einem Tal ist der Krümmungsbeitrag positiv und erhöht die Normalkraft. Auf einem Berg ist er negativ und kann den Kontakt bei hoher Geschwindigkeit aufheben. Gilt $F_N \leq 0$, verlässt die Kugel die Schiene und wird als freier Flug weitergerechnet.


## 4. Reibung, Haftung, Luftwiderstand und offene Bahnenden

### 4.1 Rollreibung und Haftreibung ($\mu_R$, $\mu_H$)

- **Rollreibung:** Bei $v
eq0$ wird die dynamische Rollreibung mit der aktuellen Normalkraft berechnet:

	$$
	F_R = -\operatorname{sgn}(v)\,\mu_R F_N.
	$$

	Im Code ist $\mu_R$ der Reglerwert, standardmäßig $0{,}0020$.
- **Haftreibung:** Der feste Modellwert ist $\mu_H=0{,}004$. Bei nahezu verschwindender Geschwindigkeit wird die Kugel exakt angehalten und bleibt stehen, solange

	$$
	|F_{g,t}| \leq \mu_H F_N.
	$$

	Überschreitet die Hangabtriebskraft diese Grenze, startet die Kugel wieder. Damit entsteht kein künstliches Kriechen durch numerisches Rauschen.
- **Kontaktbedingung:** Das Haft- und Rollreibungsmodell wird nur bei $F_N>0$ angewandt. Bei $F_N\leq0$ ist kein positiver Schienenkontakt mehr vorhanden.

### 4.2 Luftwiderstand ($c_w$)

Der Luftwiderstand wirkt ebenfalls entlang der Tangente und berücksichtigt das Vorzeichen der Bahngeschwindigkeit:

$$
F_{\mathrm{Luft}}
= -rac12ho A c_w\,v|v|,
\qquad
A = \pi R^2.
$$

Der Standardwert ist $c_w=0{,}470$; die Luftdichte bleibt $hopprox1{,}2\,	ext{mg/cm}^3$. Der Reglerbereich für $c_w$ reicht von $0{,}235$ bis $0{,}705$. Zusammen mit Gewichtskraft und Rollreibung ergibt sich

$$
F_{t,\mathrm{gesamt}}
= F_{g,t} + F_R + F_{\mathrm{Luft}}.
$$

### 4.3 Offene Bahnenden und freier Flug

Der gültige Bereich für den Kugelmittelpunkt ist

$$
R \leq x \leq L-R.
$$

Beim Überschreiten eines dieser offenen Enden wird die Position auf den jeweiligen Endpunkt gesetzt und die Kugel verlässt die Bahn mit ihrer momentanen Tangentialgeschwindigkeit. Es gibt keinen Abprallfaktor und keinen künstlichen Energieverlust. Im freien Flug werden Position und Geschwindigkeit in den geneigten Weltkoordinaten mit

$$
\dot{v}_x = 0,
\qquad
\dot{v}_y = -g
$$

weiterintegriert. Dasselbe Freiflugmodell wird verwendet, wenn die dynamische Normalkraft auf einem Berg $F_N\leq0$ wird.

### 4.4 Startbedingung, Frequenzanzeige und Speicherung

- **Startverhalten:** Beim Reset startet die Kugel im Stillstand ($v=0$) bei $x=R$, also am nutzbaren linken Randbereich. Die Startposition wird intern als $s=s(x=R)$ gespeichert.
- **Frequenzanzeige:** Die räumliche Wellenlänge wird entlang der Bahn aus der Gesamtbogenlänge $S$ bestimmt:

	$$
	\lambda_s = rac{2S}{numPeaks-1},
	\qquad
	f = rac{|v|}{\lambda_s}.
	$$

	Während des freien Flugs zeigt die Anzeige die Geschwindigkeit des freien Körpers; eine Bahnfrequenz ist dann physikalisch nicht mehr definiert.
- **Persistenz:** Änderungen an Reibungskoeffizienten, Masse oder Geometrie werden über die `LocalStorage`-API dauerhaft gespeichert. Der Haftreibungskoeffizient $\mu_H$ ist ein fester Modellparameter und besitzt keinen Schieberegler.

Die konkreten Materialien, Maße und Literaturwerte des realen Versuchsaufbaus sind im Repository nicht durch Messprotokolle oder externe Quellen belegt. Sie sind daher als Versuchsaufbau-Annahmen beziehungsweise externe Werte zu verstehen und müssen für eine reale Validierung separat geprüft werden.


## 5. Elektromagnetisches Booster-System (Coils & Hallsensoren)

In die beiden mittleren Wellenberge ist ein berührungsloses Beschleunigungssystem integriert, welches auf der Fokussierung gesättigter Magnetfelder basiert.

### 5.1 Feldcharakteristik der spitz zulaufenden Eisenkerne
Die integrierten Elektromagnete (12 mm Eisenkern, 1.018 Windungen bei 12 V Betriebsspannung) arbeiten tief im Bereich der magnetischen Sättigung ($B_{	ext{sat}} pprox 1{,}8\,	ext{Tesla}$). Durch die spitz zulaufenden Überstände wird der magnetische Fluss extrem konzentriert. Die resultierende Anziehungskraft auf die Stahlkugel folgt einer aggressiven Kelvin-Gradientengleichung:

$$
F_{	ext{Mag}}(x) = F_{	ext{max}} \cdot rac{r_0^4}{(x + r_0)^4}
$$

Diese Charakteristik ist empirisch auf eine effective Reichweite von $4\,	ext{cm}$ auf der waagerechten Ebene kalibriert ($F pprox 0{,}05\,	ext{N}$ bei $x = 4\,	ext{cm}$), schnellt im unmittelbaren Nahbereich vor der Spitze ($x 	o 0$) jedoch auf bis zu $22{,}5\,	ext{Newton}$ hoch.

### 5.2 Wired-OR Sensorsteuerung und Software-Timer
Das Einschalten der Spulen erfolgt ortsabhängig über vorgespannte Hallsensoren, das Ausschalten zeitsynchron über einen Software-Sperrkreis:
1. **Einschalt-Trigger:** Passiert die Kugel die 4-cm-Zone vor der jeweiligen Kernspitze, schaltet der entsprechende Hallsensor durch. Beide Schrägen-Sensoren sind hardwareseitig als *Wired-OR* gekoppelt und nutzen denselben Mikrocontroller-Pin. Der Magnet schaltet augenblicklich ein.
2. **Zeitgesteuertes Ausschalten:** Gleichzeitig mit der Aktivierung startet ein interner Timer mit einer festen Laufzeit von $\Delta t = 40\,	ext{ms}$. Nach Ablauf dieser Zeitspanne wird der Stromfluss gekappt. Die Kugel nutzt ihren kinetischen Impuls, um den Scheitelpunkt zu überwinden, ohne beim Verlassen des Hügels rückwärts gebremst zu werden.
3. **Software-Blindschleife:** Um eine Fehlauslösung der Hallsensoren durch das massive Magnetfeld der eigenen Spule zu verhindern, blockiert das Programm die Sensoreingänge während der Spulenlaufzeit plus einer Abklingphase von weiteren $20\,	ext{ms}$ vollständig.