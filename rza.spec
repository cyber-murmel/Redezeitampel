MCU: MSP430G2553

Pins:

#	Gruppe	Funktion
1	VCC	+3,3V
2	P1.0	Leuchte grün
3	P1.1	Leuchte gelb
4	P1.2	Leuchte rot
5	P1.3	Wecker
6	A4	Redezeiteinstellung
7	P1.5	Redezeitrücksetzung
8	P2.0	nicht verbunden
9	P2.1	nicht verbunden
10	P2.2	nicht verbunden
11	P2.3	nicht verbunden
12	P2.4	nicht verbunden
13	P2.5	nicht verbunden
14	P1.6	Bereitschaft
15	P1.7	nicht verbunden
16	RST
17	TEST
18	XOUT	Uhrenquarz 32768 Hz
19	XIN	Uhrenquarz 32768 Hz
20	GND	Masse

Konfiguration:

Reg.	Adr.	Wert	Anmerkungen

P1OUT		0x21	0x6#	zweites Nibble für Begriff der Ampel
P1DIR		0x22	0x0f	Ampel auf OUT, alles andere auf IN
P1REN		0x27	0xe0	Schalter wird auf 1 gezogen, Pin 15 nicht verbunden
BCSCTL1
