EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L oximeter-modules:Arduino_Pro_Mini-oximeter-modules U1
U 1 1 5FE79CB4
P 7750 3050
F 0 "U1" H 7700 3100 50  0000 R CNN
F 1 "Arduino_Pro_Mini" H 8000 3000 50  0000 R CNN
F 2 "" H 7750 3000 50  0001 C CNN
F 3 "" H 7750 3000 50  0001 C CNN
	1    7750 3050
	1    0    0    -1  
$EndComp
$Comp
L oximeter-modules:Module_SSD1306-oximeter-modules U2
U 1 1 5FE7F08E
P 4150 4350
F 0 "U2" H 4100 4300 50  0000 L CNN
F 1 "Module_SSD1306" H 3850 4200 50  0000 L CNN
F 2 "" H 4150 4350 50  0001 C CNN
F 3 "" H 4150 4350 50  0001 C CNN
	1    4150 4350
	1    0    0    -1  
$EndComp
Wire Wire Line
	4200 3700 4200 3450
$Comp
L Device:Battery_Cell BT1
U 1 1 5FE9404B
P 7400 4150
F 0 "BT1" V 7650 4200 50  0000 C CNN
F 1 "Battery" V 7550 4200 50  0000 C CNN
F 2 "" V 7400 4210 50  0001 C CNN
F 3 "~" V 7400 4210 50  0001 C CNN
	1    7400 4150
	0    1    1    0   
$EndComp
$Comp
L Switch:SW_SPST SW1
U 1 1 5FE9993B
P 7950 4150
F 0 "SW1" H 7950 3900 50  0000 C CNN
F 1 "Switch" H 7950 4000 50  0000 C CNN
F 2 "" H 7950 4150 50  0001 C CNN
F 3 "~" H 7950 4150 50  0001 C CNN
	1    7950 4150
	1    0    0    -1  
$EndComp
Wire Wire Line
	7600 4150 7750 4150
Wire Wire Line
	8150 4150 8250 4150
$Comp
L oximeter-modules:MH-ET_MAX30102 U3
U 1 1 5FE7B364
P 4250 3000
F 0 "U3" H 4300 3150 50  0000 R CNN
F 1 "MH-ET_MAX30102" H 4550 3050 50  0000 R CNN
F 2 "" H 4400 3050 50  0001 C CNN
F 3 "" H 4400 3050 50  0001 C CNN
	1    4250 3000
	1    0    0    -1  
$EndComp
Text Notes 4950 3250 0    50   ~ 0
The SSD1306 module already\nprovides power to the IIC line,\nthat's why the voltage selection\nis not shorted
Wire Wire Line
	4100 3450 4100 3600
Wire Wire Line
	4100 3600 4300 3600
Wire Wire Line
	4300 4000 4300 3600
Connection ~ 4300 3600
Wire Wire Line
	4200 3700 4200 4000
Connection ~ 4200 3700
Wire Wire Line
	4000 3800 4000 3450
Wire Wire Line
	4000 3800 4100 3800
Wire Wire Line
	4100 3800 4100 4000
Wire Wire Line
	4000 3900 4000 4000
Wire Wire Line
	7650 3600 7650 3450
Wire Wire Line
	4300 3600 7650 3600
Wire Wire Line
	7750 3450 7750 3700
Wire Wire Line
	4200 3700 7750 3700
Wire Wire Line
	7950 3500 7950 3800
Wire Wire Line
	7950 3800 4100 3800
Connection ~ 4100 3800
Wire Wire Line
	8150 3500 8150 3900
Wire Wire Line
	8150 3900 7300 3900
Wire Wire Line
	7300 4150 7300 3900
Connection ~ 7300 3900
Wire Wire Line
	8250 4150 8250 3500
Wire Notes Line
	6200 3300 4700 3300
Wire Notes Line
	4700 3300 4700 2900
Wire Notes Line
	4700 2900 6200 2900
Wire Notes Line
	6200 2900 6200 3300
Wire Wire Line
	4000 3900 4400 3900
Wire Wire Line
	4300 3450 4400 3450
Wire Wire Line
	4400 3450 4400 3900
Connection ~ 4400 3900
Wire Wire Line
	4400 3900 7300 3900
Text Notes 3800 4400 2    50   ~ 0
Address: 0x3C (0x78/0x79)
Text Notes 2800 3050 0    50   ~ 0
Address: 0x57 (0xAE/0xAF)
Text Notes 6350 4350 0    50   ~ 0
Power: 5 to 16 V,\naccording to Arduino\nPro Mini datasheet
Wire Notes Line
	7700 4000 7700 4500
Wire Notes Line
	7700 4500 6300 4500
Wire Notes Line
	6300 4000 7700 4000
Wire Notes Line
	6300 4000 6300 4500
$EndSCHEMATC
