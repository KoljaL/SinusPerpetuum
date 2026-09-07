# Dokumentation: Kugel-Simulator auf sinusförmiger Bahn

Diese Dokumentation beschreibt den realen, physikalischen Versuchsaufbau einer Kugelrollbahn sowie die mathematischen und physikalischen Grundlagen, die für die digitale 2D-Simulation in JavaScript herangezogen werden.

---

## 1. Aufbau der Bahn in der realen Welt

Der mechanische Versuchsaufbau besteht aus einer Trägerkonstruktion, einem Führungsschienensystem und einem präzise definierten Rollkörper. Als Grundlage für diese Beschreibung dienen die Standardwerte (Default-Werte) des Systems.

### 1.1 Trägerbahn und Nut
* **Material des Trägers:** Massivholz.
* **Geometrische Breite:** 40 mm.
* **Konstruktionsmerkmal:** In der Mitte des Holzträgers befindet sich eine eingefräste Längsnut. Diese Nut dient ausschließlich als Freiraum (Freisparung), damit die Kugel während der Bewegung zu keinem Zeitpunkt den Holzboden berührt. Das Holz übernimmt somit eine reine Haltefunktion für die Schienen und übt keine Reibungskräfte auf den Rollkörper aus.

### 1.2 Schienensystem (Die Spur)
* **Material der Schienen:** Zwei parallel verlaufende Rundeisen (Rundstäbe aus Eisen).
* **Durchmesser der Rundstäbe:** 2 mm pro Stab.
* **Montage:** Die Schienen sind parallel im exakten Abstand auf dem Holzträger fixiert (verklebt).
* **Spurweite (Abstand $d$):** Der lichte Abstand zwischen den beiden Rundeisen beträgt **35 mm** ($d = 3{,}5\text{ cm}$).

### 1.3 Der Rollkörper (Die Kugel)
* **Material:** Geschliffener Wälzlagerstahl.
* **Masse ($m$):** 450 g ($0{,}45\text{ kg}$).
* **Äußerer Durchmesser ($D$):** 4{,}8 cm.
* **Äußerer Kugelradius ($R$):** 2{,}4 cm.

### 1.4 Kontaktgeometrie und Lagerung
Da der lichte Schienenabstand ($35\text{ mm}$) kleiner ist als der Kugeldurchmesser ($48\text{ mm}$), sinkt die Kugel ein Stück weit zwischen die beiden Eisenrundstäbe ein. Sie hat exakt **zwei Berührungspunkte** – jeweils einen Kontaktpunkt pro Rundeisen. Ein Kontakt mit dem Holzträger oder dem Nutgrund ist ausgeschlossen. Die Kugel lagert und rollt ausschließlich auf den beiden Flanken der Eisenstäbe (Materialpaarung: Stahl auf Eisen).


## 2. Mathematische Beschreibung der Bahngeometrie

Die Schienenkonstruktion beschreibt im ungeneigten Zustand eine modifizierte Sinuskurve (Kosinusphase), die über eine horizontale Gesamtlänge von **100 cm** definiert ist.

### 2.1 Koordinatensystem
* Die horizontale Achse wird als $x$-Achse definiert, wobei $x$ von $0\text{ cm}$ (linker Rand) bis $100\text{ cm}$ (rechter Rand) verläuft.
* Die vertikale Achse wird als $y$-Achse definiert. Der Nullpunkt ($y = 0$) beschreibt die vertikale Mittellinie (Symmetrieachse) der Welle.

### 2.2 Die Wellenfunktion
Die vertikale Höhe $y$ an der horizontalen Position $x$ wird durch folgende Kosinusfunktion beschrieben:

```math
y(x) = A * cos(k * x)
```


Wobei:
* **$A$ (Amplitude):** $2{,}0\text{ cm}$ (maximale Auslenkung nach oben und unten).
* **$numPeaks$ (Wellenberge):** $7$. Dieser Wert ist so gewählt, dass die Bahn links ($x = 0$) mit einem Maximum beginnt und rechts ($x = 100$) mit einem Maximum endet.
* **$k$ (Kreisfrequenz bezogen auf die Bahnstrecke):** 
```math
k = ((numPeaks - 1) * pi) / L = (6 * pi) / 100 = 0,1885 cm^-1
```
Hierbei ist $L = 100\text{ cm}$ die horizontale Gesamtlänge.

### 2.3 Geometrische Ausprägung bei 7 Wellenbergen
Durch das Einsetzen von $numPeaks = 7$ in die Kosinusfunktion ergibt sich exakt folgende Abfolge auf den 100 cm Schienenweg:
1. **Bei $x = 0\text{ cm}$:** Ein lokales Maximum ($y = +2\text{ cm}$), welches den **ersten halben Berg** links bildet.
2. **Im Verlauf:** Es folgen insgesamt **3 vollständige Wellentäler** ($y = -2\text{ cm}$) und **2 vollständige, dazwischenliegende Wellenberge** ($y = +2\text{ cm}$).
3. **Bei $x = 100\text{ cm}$:** Die Funktion endet exakt wieder auf einem lokalen Maximum ($y = +2\text{ cm}$), was den **letzten halben Berg** rechts abbildet.

Diese mathematische Randbedingung stellt sicher, dass die Bahn an beiden äußeren Enden mechanisch geschlossen ist und die Kugel in einer geschlossenen Senke gefangen bleibt.


# Physik der Rollbewegung & Der effektive Rollradius

## 3. Physikalische Beschreibung der Rollbewegung

In der Simulation werden die Bewegungsgleichungen numerisch gelöst. Um das reale Verhalten der Kugel exakt abzubilden, müssen zwei signifikante Korrekturen an der klassischen Punktmassen-Physik vorgenommen werden: der **effektive Rollradius** und das **Trägheitsmoment**.

### 3.1 Der effektive Rollradius ($r_{\text{eff}}$)
Da die Kugel nicht auf ihrem tiefsten Punkt aufliegt, sondern links und rechts an den Schienenflanken im Abstand $d = 35\text{ mm}$ einhakt, rotieren die tatsächlichen Kontaktpunkte auf einem kleineren Kreisradius als der geometrische Außenradius ($R = 24\text{ mm}$) der Kugel. 

Dieser effektive Rollradius $r_{\text{eff}}$ berechnet sich über den Satz des Pythagoras aus dem Außenradius und der halben Spurweite:

```math
r_eff = sqrt( R^2 - (d / 2)^2 )
r_eff = sqrt( 2,4^2 - 1,75^2 ) = sqrt( 5,76 - 3,0625 ) = sqrt( 2,6975 ) ≈ 1,6424 cm
```


* **Auswirkung auf die Dynamik:** Bei einer linearen Vorwärtsbewegung der Kugel um eine Strecke $\Delta x$ muss sich die Kugel aufgrund des kleineren Abrollradius $r_{\text{eff}}$ **schneller um ihre eigene Achse drehen** (höhere Winkelgeschwindigkeit $\omega = v / r_{\text{eff}}$), als wenn sie auf einer flachen Ebene abrollen würde.

### 3.2 Das Trägheitsmoment und die 5/7-Korrektur
Eine rollende Kugel besitzt nicht nur translationale kinetische Energie (Vorschub), sondern auch rotatorische kinetische Energie (Drehung). Für eine homogene Vollkugel gilt das Trägheitsmoment:

```math
I = (2 / 5) * m * R^2
```
Wenn eine Kugel auf einer flachen Ebene *ohne Rutschen* rollt, führt die Aufteilung der potenziellen Energie in Translation und Rotation dazu, dass die effektive Beschleunigung auf einer Schräge gegenüber einem reinen reibungsfreien Gleiter um den Faktor **5/7 (ca. 71,4 %)** reduziert ist:

```math
a = (5 / 7) * g * sin(alpha)
```
* **Wichtige Besonderheit bei Zwei-Punkt-Auflage:** Da der Abrollradius $r_{\text{eff}}$ durch die 35-mm-Spurweite kleiner ist als der Massenradius $R$, verschiebt sich dieses Verhältnis in der Realität noch weiter zulasten der Translation. Die Kugel muss mehr Energie in die Rotation stecken, wodurch sie mathematisch noch träger beschleunigt als der klassische 5/7-Standardfaktor einer flach rollenden Kugel. *(Hinweis: Diese vertiefte Anpassung wird in einer späteren Code-Version implementiert).*
* **Zusammenfassung:** Der effektive Rollradius $r_{\text{eff}}$ und das Trägheitsmoment führen dazu, dass die Kugel auf der Zwei-Punkt-Auflagebahn langsamer beschleunigt als eine idealisierte flach rollende Kugel. Diese Effekte müssen in der Simulation berücksichtigt werden, um realistische Bewegungen zu erzeugen.

## 4. Beschreibung der Simulationsparameter und Dämpfung

Um die Bewegung der Kugel realistisch zu begrenzen und ein unendliches Pendeln (Perpetuum Mobile) zu verhindern, berücksichtigt das System zwei physikalische Dämpfungsmechanismen, die über Schieberegler skaliert werden können.

### 4.1 Die Rollreibung ($\mu_R$)
* **Physikalische Ursache:** Mikroskopische Verformungen der Stahlkugel und der beiden Eisenrundstäbe am unmittelbaren Kontaktpunkt.
* **Mittelwert (Default):** $\mu_R = 0{,}0020$. Dies entspricht dem realistischen Literaturwert für eine hochfeste, geschliffene Materialpaarung aus Stahl und Eisen/Stahl.
* **Berechnung:** Die Bremskraft ist proportional zur Normalkraft $F_N$, welche wiederum vom lokalen Bahnneigungswinkel $\alpha$ abhängt: $F_{\text{Roll}} = \mu_R \cdot m \cdot g \cdot \cos(\alpha)$.
* **Regler-Bereich:** $-50\%$ bis $+50\%$ ($0{,}0010$ bis $0{,}0030$).

### 4.2 Der Luftwiderstand ($c_w$)
* **Physikalische Ursache:** Der Strömungswiderstand, den die Kugel beim Verdrängen der Umgebungsluft erfährt. Da die Kugel im Versuch sehr hohe Geschwindigkeiten aufbauen kann, gewinnt diese Kraft quadratisch an Bedeutung.
* **Mittelwert (Default):** $c_w = 0{,}470$. Dies ist der aerodynamische Standardbeiwert für eine geometrisch perfekte Kugelkonstruktion.
* **Berechnung:** Die Kraft berechnet sich aus der Stirnfläche der Kugel ($A = \pi \cdot R^2$), der Luftdichte ($\rho \approx 1{,}2\text{ mg/cm}^3$) und der aktuellen Geschwindigkeit im Quadrat: $F_{\text{Luft}} = 0{,}5 \cdot \rho \cdot A \cdot c_w \cdot v^2$.
* **Regler-Bereich:** $-50\%$ bis $+50\%$ ($0{,}235$ bis $0{,}705$).

### 4.3 Startbedingung und Speicherung
* **Startverhalten:** Um reproduzierbare Messergebnisse zu gewährleisten, wird die Kugel bei jedem Reset exakt am linken Rand ($x = r_{\text{eff}}$) auf dem Scheitelpunkt des ersten halben Berges im Stillstand ($v = 0$) platziert.
* **Persistenz:** Alle Modifikationen an den Reibungskoeffizienten, Massen oder Geometrien werden über die `LocalStorage`-API des Webbrowsers dauerhaft gesichert, sodass die Versuchskonfiguration auch nach einem Neuladen der Webseite unverändert aktiv bleibt.
