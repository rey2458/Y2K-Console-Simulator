# Console UI launcher simulator
I wanted to make something that would look like an early designs of XBOX or PlayStation UI but wanted to combine it with the modern point of view on the Y2K style and on late 90s early 2000s aesthetic. So here it is, my magnum opus so far

<img width="800" height="450" alt="mmgif" src="https://github.com/user-attachments/assets/907b7bef-87fa-4b5c-850e-84694bfb7afc" />

[Watch the full video walkthrough on YouTube](https://youtu.be/tA7MHWJbMgM)

## Tools:
* C++17
* Qt
* Blender

## Features:
* Navigation using the keyboard (not fully, "prev", "pause" and "next" buttons in the musicplayer are still need to be pressed by lmb)
* Users profile customisation
* Hardcoded map-based playlist system
* Background and low-poly model are rendered in Blender by me

## How to run:
You need to open the .pro file in Qt Creator, build it and run

**Important!** .mp3 and .jpg file paths are hardcoded at the moment, change them to your paths before compiling


##  TO-DO:
* add lyrics parsing for songs
* make normal data base instead of hardcoding
* add network access
* add volume control by keyboard
