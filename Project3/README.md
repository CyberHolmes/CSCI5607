# CSci 5607 Project3

Project3a - Ray Tracing

- Hailin Archer - deak0007@umn.edu

## Description

This ray tracer program takes a scene text file from commandline as input. It renders the scene on computer screen using openGL. It also saves the image on disk as specified by the scene file.
It allows user to control the camera's position and view angle by keyboard input.

## How to compile the code
```
$ make clean
$ make
```
## How to run the program
```
$ ./ray ./Scenes/<FILENAME>.txt
```
## User Controls
```
'a' - control camera x axis by reducing x component
'd' - control camera x axis by adding x component
's' - control camera y axis by reducing x component
'w' - control camera y axis by adding x component
'e' - control camera z axis by reducing x component
'q' - control camera z axis by adding x component

'j' - control camera forward direction's x component by reducing x component
'l' - control camera forward direction's x component by adding x component
'k' - control camera forward direction's y component by reducing y component
'i' - control camera forward direction's y component by adding y component
'u' - control camera forward direction's z component by reducing z component
'o' - control camera forward direction's z component by adding z component

'b' - save current rendering result on screen to a file with time stamp (won't delete the image saved at the beginning of the rendering)

'escape' - quit program <implemented in starter code>

'f' - full screen <implemented in starter code>
