# Troubleshooting

## No serial port

The validated board appeared as `USB-SERIAL CH340`. Use a data-capable cable and install the CH340 driver if no COM/serial device appears.

## Screen stays blank

Confirm that the hardware is the single-USB ILI9341/XPT2046 ESP32-2432S028R and that the correct build environment was flashed. Other CYD variants use different pinouts or controllers.

## Profile cannot be reached

Check Wi-Fi and use a public profile URL or username. The alpha provider reports reachability only; it intentionally keeps aggregate metrics hidden until their public mapping is verified.

## Setup portal unavailable

Restart the device and check the display for the `MakerWorldAnalytics-XXXX` network and local address. Do not share the setup password publicly.
