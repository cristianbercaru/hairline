# hairline

A simple gstreamer application, which uses the webcam as source and the screen as sink.

## Build - Ubuntu 24.04

This application has been built and tested on **Ubuntu 24.04 Linux** using **gstreamer-1.0** and **Gtk+3.0** libraries. <br>
Build and run on other operating system versions at your own risk.

| Makefile target   | Function                                                 |
| ----------------- | -------------------------------------------------------- |
| `make install`    | Install all the necessary packets for the build          |
| `make`            | Build `hairline` application                             |
| `make clean`      | Remove all build artifacts including `hairline`          |
| `make again`      | Remove all artifacts and rebuild `hairline` from scratch |

## Runtime controls

Run the following command to launch the application
```
./hairline
```
### Buttons
**⇆** Flip image horizontally

**⇵** Flip image vertically

**⭮** Rotate image clockwise

**⭯** Rotate image counter-clockwise

**◑** Invert colors

## Implementation

You can run this command directly into your shell. I implemented the equivalent gstreamer pipeline in C++.
```
gst-launch-1.0 v4l2src ! videoflip method=4 ! videoconvert ! coloreffects preset=3 ! \
               videoconvert ! openh264enc ! openh264dec ! autovideosink
```

`v4l2src` is the generic Linux module which provides the camera input.

`videoflip` is used to flip and rotate the image by changing its `method` property:
```
none                 (0) – Identity (no rotation)
clockwise            (1) – Rotate clockwise 90 degrees
rotate-180           (2) – Rotate 180 degrees
counterclockwise     (3) – Rotate counter-clockwise 90 degrees
horizontal-flip      (4) – Flip horizontally
vertical-flip        (5) – Flip vertically
upper-left-diagonal  (6) – Flip across upper left/lower right diagonal
upper-right-diagonal (7) – Flip across upper right/lower left diagonal
```

`coloreffects` inverts colors by switching beetween `preset` 0 and 3:
```
none  (0) – Do nothing preset
heat  (1) – Fake heat camera toning
sepia (2) – Sepia toning
xray  (3) – Invert and slightly shade to blue
xpro  (4) – Cross processing toning
```

## Documentation

<https://gstreamer.freedesktop.org/documentation>