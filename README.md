# Alarm-clock
Alarm clock wit 7segment LED Display and STM32F042.

![obrázok](https://user-images.githubusercontent.com/18489229/224020670-61b3f72f-d508-4ec6-a274-42f23987e6f4.png)

Description:
The alarm clock is powered by an external source of 9V DC/1A. It has four control buttons and a four 7-segments display. There is a reset button from the back.

Operation:

Functions of the Buttons from left to right:
1. Mode/Setup button
2. button + to increase the set value
3. button - to reduce the set value
4. Alarm button to turn on or off the alarm

When connecting the alarm clock to the power voltage, the current time is lit up on the display. Re -pressing the mode/setting button can be moved in the alarm menu. The menu consists of the following parts:

    Showing time
    Showing day of the week
    Showing date
    Showing year
    Showing temperature
    Showing brightness
    Showing calibration of the time
    Showing alarm time
    Showing alarm day

If the mode/setting button is kept for two seconds, the value on the display starts to flash and with the buttons  + and - it can be changed. If these buttons + and - are pressed for more than one second, the value on the display changes faster.
By simply holding the alarm button, the lowered alarm turns off. When this button is pressed for two seconds, the alarm is activated or deactivated. If the alarm is activated, the LED in the upper right corner is lit.

Construction:

The alarm clock is folded with three PCBs:

Alarm clock: https://oshwlab.com/bobobo007/hodiny_v01-001

Buttons: https://oshwlab.com/bobobo007/buttons_v01-001

Display: https://oshwlab.com/bobobo007/display_v01-001

After fitting the boards, the alarm clock and display plate should be flown at a 90 ° angle so that the display is connected to the alarm clock. To install the buttons, use two M3x15mm spacers and four M3x6mm screws. After printing the box in the 3D printer, attach the plate with four screws 2.9x6.5 to the bottom box. Attach the top box with a screw of 2.4x30mm. The stl files are attached.

I ordered a red filter here: https://www.tme.eu/en/details/km-35-filtr/skatulky-stost-prislusnosť/maszczyk/fi-0035-rd/

Firties can be ordered here: https://www.tme.eu/en/details/tact-2brrd/microspinace-tact/ninigi/

Also via aliexpress: https://www.aliexpress.com/item/328900666634.html?spm=A2G0O.ORDER_LIST.List.List_main.117.22321802vydsym

The attachment includes complete STM32Cubeide software. It is also possible to record software with STM32 ST-LINK Utility. BIN files are also attached for different crystal frequencies.
