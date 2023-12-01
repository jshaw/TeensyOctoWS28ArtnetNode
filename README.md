# Teensy OctoWS28 Artnet Node
**Version: V0.2.1**

Studio Jordan Shaw // http://jordanshaw.com

This is a Artnet node built with Teensy 4.1 + OctoWS28
* Teensy Board 4.1: https://www.pjrc.com/store/teensy41.html 
* OctoWS28 Adaptor: https://www.pjrc.com/store/octo28_adaptor.html
* Ethernet Kit for Teensy 4.1: https://www.pjrc.com/store/ethernet_kit.html
* OctoWS28 Teensy Library: https://www.pjrc.com/teensy/td_libs_OctoWS2811.html 
* FastLED: https://github.com/FastLED/FastLED
* Artnet Library: https://github.com/natcl/Artnet

**Note:** for Teensy 4.1, Teensyduino and the Artnet library, the following artnet.h source needs to be modified to work with 4.1.
Modify this file to use NativeEthernet `/Users/[USER]/Library/Arduino15/packages/teensy/hardware/avr/1.58.1/libraries/Artnet/Artnet.h`

Teensy 4.1 requires `NativeEthernet.h` and `NativeEthernetUdp.h`.
To avoid library conflicts, it's best to uninstall the Artnet library available from within the Arduino library manager and only use the modified version included with Teensyduino. Around line ~30 in Artnet.h update the below... [the PJRC thread.](https://forum.pjrc.com/index.php?threads/does-the-artnet-library-work-with-the-native-ethernet-library.70064/)

```
#if defined(ARDUINO_SAMD_ZERO)
    #include <WiFi101.h>
    #include <WiFiUdp.h>
#else
    // #include <Ethernet.h>
    // #include <EthernetUdp.h>
    #include <NativeEthernet.h>
    #include <NativeEthernetUdp.h>
#endif
```

## Sources from around the web that inspired and facilitated all this working

Original Artnet example from Paul Stoffregen's OctoWS2811 library
https://github.com/PaulStoffregen/OctoWS2811/blob/master/examples/Artnet/Artnet.ino

Inspired by CaseyJScalf's Teensy 4.1 Artnet Node w/ Madmapper
https://github.com/CaseyJScalf/Teensy-4.1-as-ArtNet-Node-for-5v-WS2812-LED

The implimentation of the FastLED with OctoWS28 + Pin selection
https://blinkylights.blog/2021/02/03/using-teensy-4-1-with-fastled/

## Raspberry Pi
To run on a Pi, I used a [USB ethernet adapter](https://www.amazon.ca/BENFEI-Ethernet-Compatible-Notebook-Windows7/dp/B08KWC7D78/ref=asc_df_B08KWC7D78/) for the artnet networking.
I used the adapter IP for the artnet IP

## Artnet Server / Processing App
This project was put together in tandum with a custom Processing.org app to generate the LED visuals.
The gotchya with this is that the artnet library compatible with the Teensy/Octo setup is not the library in the Processing library manager.
YOU MUST download and manually installed from Florian's [Artnet4j - Art-Net DMX over IP library for Java and Processing repo](https://github.com/cansik/artnet4j).
