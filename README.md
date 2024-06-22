# noisecommander3dscompanion
Companion app for Noise Commander 3DS

## What and why?
This is a Qt-based app which can display the screen of a Nintendo 3DS running Noise Commander via TCP/IP connection over wifi.

While **Noise Commander 3DS** is closed source, I am providing the companion app source code to allow it be built on other platforms than linux.

There are currently no other dependencies than **Qt5** and it uses the QMake build system. I hope this should make it easy to port. I believe you only need to open the .pro file in **QtCreator** and should be ready to go.

Noise Commander renders 8x8 pixel font-tiles like a console/text mode and only the changed blocks are transmitted to reduce the required bandwidth. The upper screen can show 50x30 tiles and the lower 40x30.

A "pixel"-tile only consists of a two bytes: An ascii character and a color-byte with the foreground/background colors using 3 bits each.

It uses a very simplistic made up protocol which may change with future versions of Noise Commander and I have not set up any version-handling yet. The current NC version is 0.0.5.

Apologies for the mess but I hope it works for you and brings you joy.

I am hoping to make Noise Commander good enough for people to record jams with this for Youtube. Wish me luck :D

---

External links:

[Patreon](https://www.patreon.com/NoiseCommander3DS)

[YouTube](https://www.youtube.com/@NoiseCommander3DS)
