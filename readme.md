# ♾️ SinusPerpetuum: Die elektromagnetische Endlos-Kugelbahn

```text
____ _ _  _ _  _ ____ ___  ____ ____ ___  ____ ___ _  _ _  _ _  _
[__  | |\ | |  | [__  |__] |___ |__/ |__] |___  |  |  | |  | |\/|
___] | | \| |__| ___] |    |___ |  \ |    |___  |  |__| |__| |  | 
              ~ Die ewige Sinuswelle ~
```

> **Ein physikalisches Kugelbahn-Projekt mit Kondensator-Pufferung.** 
> Dieses Repository enthält die vollständige Dokumentation und die korrigierten technischen Spezifikationen für den Bau einer interaktiven, elektromagnetisch beschleunigten Kugelbahn. Eine schwere 50-mm-Stahlkugel wird durch im Holz verborgene Elektromagnete kontaktlos die Hügel hinaufgezogen, um im perfekten Loop endlos zu rollen.

---


# Gauss-Rennbahn: Physikalisches Kugelbahn-Projekt mit Kondensator-Pufferung

Dieses Repository enthält die vollständige Dokumentation und die korrigierten technischen Spezifikationen für den Bau einer interaktiven, elektromagnetisch beschleunigten Kugelbahn. Eine schwere ferromagnetische Stahlkugel wird durch im Holz verborgene Elektromagnete kontaktlos die Hügel hinaufgezogen. Dank einer integrierten Kondensatorbank reicht ein kompaktes Standard-Steckernetzteil für den Betrieb aus.

---

## 📐 1. Maße und Geometrie der Holzbahn

* **Gesamtlänge Holzblock:** 1000 mm
* **Breite / Höhe Holzblock:** 80 mm / **80 mm** *(angepasst an das Hügelprofil, da der Scheitelpunkt bei 70 mm liegt)*
* **Material:** Massives Hartholz wie Buche oder Eiche (*Wichtig: Kein weiches Nadelholz wie Fichte/Kiefer verwenden, da die dünnen Wandstärken über den Magneten sonst brechen können.*)
* **Hügelprofil:** Sinuswelle mit 20 mm Gesamthöhe (Scheitelpunkt bei 70 mm ab Unterkante des Blocks, Talsohle bei 50 mm ab Unterkante).
* **Profil-Aufteilung (von links nach rechts):**
  * **0,0 – 166,6 mm:** 1. halber Hügel (fällt ab von 70 mm auf 50 mm / Tal)
  * **166,6 – 500,0 mm:** 1. ganzer Hügel (Tal ➔ **Gipfel 1 bei 333,3 mm** ➔ Tal)
  * **500,0 – 833,3 mm:** 2. ganzer Hügel (Tal ➔ **Gipfel 2 bei 666,6 mm** ➔ Tal)
  * **833,3 – 1000,0 mm:** 2. halber Hügel (steigt an von 50 mm auf 70 mm)
* **Kugel:** Ferromagnetische Stahlkugel, Durchmesser **50 mm**, Gewicht ca. **514 g** (aus Chrom- oder Lagerstahl; kein Edelstahl!).
* **Laufbahn:** Leicht konkave Führungsnut (ca. **52–55 mm breit**) auf der Oberseite, um der 50-mm-Kugel stabilen Halt zu bieten.

---

## 🧲 2. Technische Daten des Kurven-Magneten (2 Stück benötigt)

* **Einbauart:** Waagerecht unter dem ansteigenden Hügelteil, leicht bananenförmig (flache U-Form) an den Hügelverlauf angepasst. Von unten so tief ausfräsen, dass die beiden Eisenspitzen exakt **1 mm Restwandstärke** unterhalb der Laufbahnoberfläche stehen lassen.
* **Kern:** Weicheisen / unlegierter Baustahl (z. B. **S235JR**). Kein Edelstahl! Länge: 120 mm, Durchmesser: 12–15 mm. Enden auf 15 mm Länge kegelförmig zugespitzt (flache Spitze mit 3 mm Restdurchmesser).
* **Wicklung:** **Ca. 1050 Windungen** pro Magnet aus **0,50 mm Kupferlackdraht** (gleichmäßig verteilt auf 80 mm Wickelbreite), um auf die geforderte Gesamtlänge zu kommen.
* **Drahtbedarf:** Exakt **50 Meter** (ca. 88 Gramm) pro Magnet. Eine standardmäßige 250g-Rolle (~140 m) ist für beide Magnete absolut ausreichend.
* **Spulen-Gesamtdurchmesser:** Ca. 22 mm (aufgrund der höheren Windungszahl).

---

## 🔌 3. Spannungsversorgung und Elko-Pufferung

Um das System trotz der schweren 514g-Kugel mit einem sehr kleinen, günstigen Netzteil betreiben zu können, wird eine Kondensatorbank als Zwischenspeicher genutzt.

* **Netzteil:** Günstiges Steckernetzteil mit **24 V Gleichspannung (DC) und 1 Ampere** (Leistung: 24 Watt).
* **Ladewiderstand:** Ein **33 Ohm Zementwiderstand mit mindestens 25W** Belastbarkeit. Er wird in Reihe zwischen Netzteil und Kondensatoren geschaltet, um den Einschaltstrom auf ca. 0,73 A zu begrenzen und das Netzteil vor Überlastung beim Laden zu schützen.
* **Kondensatorbank (Elkos):** Gesamtkapazität von **ca. 14.100 µF**. Realisiert durch die Parallelschaltung von **3 Stück Elektrolytkondensatoren mit je 4.700 µF / 50 V** (*Wichtig: 50V-Typen wählen, um ausreichende Spannungsfestigkeit gegen Schaltpeaks zu gewährleisten*).
* **Sicherheits-Entladung (Bleeder):** Ein **4,7 kOhm (0,25W) Widerstand** wird fest parallel zu den Kondensatoren geschaltet. Er entlädt die Elko-Bank nach dem Trennen vom Stromnetz innerhalb von ca. einer Minute selbstständig.

---

## ⚡ 4. Schaltlogik und Magnetfeld-Abschaltung

* **Elektrische Werte pro Impuls:** Spulenwiderstand: **4,6 Ohm** (für die 50m Kupferlackdraht), Stromaufnahme beim Schalten: **ca. 5,2 Ampere**, Peak-Leistung: **ca. 125 Watt**. Diese massive Spitzenenergie wird komplett aus der gepufferten Kondensatorbank bereitgestellt.
* **Schaltelement:** Ein leistungsstarker N-Kanal MOSFET (empfohlen: **IRLB3034** oder **IRF3205**), der über den Arduino Nano angesteuert wird. *Hinweis: Aufgrund der schweren Kugel und längeren Einschaltzeiten wird ein kleiner Kühlkörper auf dem MOSFET empfohlen.*
* **Sensorik:** **2 analoge Hall-Sensoren (SS49E)**, jeweils gepaart mit einem winzigen Neodym-Magneten (z. B. 5x2 mm). 
  * *Einbau-Hinweis:* Die Sensoren werden **ca. 15–20 mm versetzt vor dem Beginn des Magnetkerns** in den tiefen Talsohlen verbaut. Dadurch wird verhindert, dass der Sensor permanent durch das Eigenfeld des ruhenden Eisenkerns übersteuert wird. Der Sensor triggert den zeitgesteuerten Impuls, sobald die Kugel das Tal passiert.
* **Schaltlogik:** Der Arduino Nano schaltet die Spule ein, sobald die Kugel die Steigung hochrollt. Kurz vor dem Gipfel (Scheitelpunkt) schaltet er sie komplett ab, damit die Kugel durch Eigenschwung über die Kuppe rollt.
* **Blitzschnelle Feldzerstörung (TVS-Schutzdiode):** Um das Magnetfeld am Gipfel schlagartig zusammenbrechen zu lassen (damit die Spule die schwere Kugel beim Verlassen nicht zurückzieht), wird parallel zu jeder Spule eine Schutzbeschaltung gesetzt:
  * **1× Standard-Diode 1N4007** in Reihe (antiseriell) mit **1× TVS-Diode (Typ: P6KE33A)**. Beim Abschalten begrenzt die TVS-Diode die Induktionsspannung zuverlässig auf ca. 33V. Zusammen mit der Betriebsspannung von 24V treten maximal ca. 57V Schaltspannung auf, was für den IRLB3034 (40V) zu viel wäre – daher wird hier zwingend der **IRF3205** (Sperrspannung 55-60V) oder eine TVS-Diode mit geringerer Spannung (z.B. **P6KE24A**) in Kombination mit einem spannungsfesteren MOSFET genutzt.

---

## 🛒 Einkaufsliste

### 🪵 Mechanik & Bahn
* [ ] **1x Hartholzblock** (Buche oder Eiche), Maße: 1000 x 80 x 80 mm
* [ ] **1x Stahlkugel** (aus Lagerstahl / Chromstahl), Durchmesser: **50 mm** (Gewicht: ca. 514 g)
* [ ] **2x Weicheisenbolzen / Rundstahl** (für die Magnetkerne), Durchmesser 12 mm oder 15 mm, Länge min. 120 mm. Material: Unlegierter Baustahl (z. B. S235JR).

### 🔋 Stromversorgung & Pufferung
* [ ] **1x Steckernetzteil 24V DC / 1A** (24 Watt) mit Standard-Hohlstecker (z. B. 5,5 mm / 2,1 mm)
* [ ] **1x Hohlstecker-Einbaubuchse** (passend zum Netzteil), zum Einlassen ins Holz
* [ ] **1x Hochlast-Zementwiderstand 33 Ohm / 25 Watt**
* [ ] **3x Elektrolytkondensator (Elko) 4.700 µF / 50V** (Bauform: Radial)
* [ ] **1x Widerstand 4,7 kOhm** (0,25 Watt Metallfilm) als Entladewiderstand (Bleeder)

### 🧲 Magnetspulen & Sensorik
* [ ] **1x Rolle Kupferlackdraht (Cul) 0,50 mm** (250g-Rolle, entspricht ca. 140 Metern)
* [ ] **2x Analoger Hall-Sensor** (Typ: SS49E)
* [ ] **2x Neodym-Dauermagnete** (z. B. Scheibenmagnete 5x2 mm)

### 🧠 Steuerung & Elektronik (Schaltkreis)
* [ ] **1x Arduino Nano (V3.0)** inkl. Stiftleisten
* [ ] **2x N-Kanal MOSFET** (Typ: **IRF3205**)
* [ ] **2x Standard-Diode** (Typ: 1N4007)
* [ ] **2x TVS-Suppressordiode 24V** (Typ: P6KE24A, unidirektional)
* [ ] **2x Widerstand 150 Ohm** (0,25 Watt Metallfilm) als Gate-Vorwiderstand
* [ ] **2x Widerstand 10 kOhm** (0,25 Watt Metallfilm) als Gate-Pull-Down-Widerstand

### 🛠️ Hilfsmaterialien
* [ ] **1x Lochrasterplatine** (Streifenraster) zur Montage der Bauteile
* [ ] **1x 2-Komponenten-Epoxidharz-Kleber** (z. B. UHU Plus Schnellfest)
* [ ] **Dünne Litzenkabel** (0,25 mm² oder 0,5 mm²) in verschiedenen Farben
