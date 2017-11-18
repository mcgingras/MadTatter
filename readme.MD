<h1 align="center">The Mad Tatter</h1>

<div align="center">
<img src="app/static/t20.svg" width="50%" />
</div>

<div align="center">
  <strong>CAD Machine designed to give tattoos.</strong>
</div>
<div align="center">
  Site + Software.
</div>

<br />

<div align="center">
  <sub>An INFO 4320 project @ Cornell University. Built by
  <a href="https://github.com/mcgingras">Michael Gingras</a>
  <a href="#">Andrew Grossfeld</a> and
  <a href="#">Anna Kamphampaty</a>
</div>

## Table of Contents
- [Features](#features)
- [Example](#example)
- [Philosophy](#philosophy)
- [Site](#site)
- [Ardunio](#arduino)
- [FAQ](#faq)
- [Installation](#installation)

## Features
- __svg2gcode:__ convert svg files to gcode to run on the machine.
- __draw:__ draw your own svgs
- __live:__ see a live stream of the machine
- __tcp:__ TCP support for GCODE instructions with arduino
- __ink box:__ currently the tattoos are done with inkbox gel. REAL to come later.

## Example
```
show some examples
```


## Philosophy
The goal of this project is to create a machine capable of tattooing a human arm. In the interest of this project, we are limiting our machine’s tattooing capabilities to drawing with a pen. We hope our project will initiate discussion about the role of machines in artistic, creative work. Our aim is to make (temporary) tattooing more accessible for artists who don't have time to apprentice under someone, and to let a user receive a temporary tattoo of their choice through a machine.

To initiate this discussion and explore the role of machines in art, we hope to answer the following questions: Will the user’s experience with a machine be better or worse than if they went to a human tattoo artist? Can machines replace humans in the realm of art and tattoos? We trust machines to carry out highly complex tasks — will we trust them with something that is artistic in nature?


## Site
The site is built in Flask. Main components are svg2gcode, gcode d3 visualizer, and the TCP to arduino capabilities. Each of these functionalities is in its own module for use outside of this project.



## Arduino
The software responsible for controlling the Mad Tatter is written in the arduino `.ino` language.


## FAQ
### Why mad tatter?
_ask andrew_

### Will this work with real tattoos?
The initial project was proposed with real tattoos in mind. For safety reasons, the course staff would not let us make a machine capable of actually tattooing a human arm, but Michael plans on continuing the project to add support for real tattoos.

### How can I make the machine myself?
Schematics available upon request.

### I am from inkbox and think this is super cool.
Please send us more gel!!


## Installation
To run the flask web app...
```
python run.py
```
