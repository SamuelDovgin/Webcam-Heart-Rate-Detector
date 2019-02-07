# OpenFrameworks Webcam Heart Rate Tracker 
By: Samuel Dovgin
Completed: 12/20/2018

This project focuses around creating an application that takes in a live webcam feed of a users face
and through the use of signal processing Fourier Transforms calculate the heart rate of the individual.
This works by detecting minute changes in skin color normally invisible to the naked eye.
The periodic change in skin tone is a result of blood rushing under the skin that is in sync with every 
heartbeat. This is then calculated into standard beats per minute.

An essential part of this project is first recognizing and tracking where human faces are located in a 
video feed. This will allow a patch of skin, such as the forehead, to be tracked specifically.

# Background Links
https://web.stanford.edu/class/cs231a/prev_projects_2016/finalReport.pdf
This project is where much of the inspiration was sourced from. It also served as the refrence for
technical equations to use. However this project utilized fully pre-recorded videos and I wanted to 
make a fully live version.

https://people.csail.mit.edu/mrub/vidmag/
This is where much of the Video Magnification research is available on the internet.

http://people.csail.mit.edu/mrub/papers/vidmag.pdf
Research paper explaining the findings and theory behind Eulerian Video Magnification

# Framework
OpenFrameworks - "open source C++ toolkit designed to assist the creative process by providing 
a simple and intuitive framework for experimentation". I implemented simple opensource graphic libraries
to display graphs, live video stream, and pixel color flutuation.

# Libraries
FFTW - this will be the main library used to process signal streams of pixel data into data of a
single heartrate per second data point. This optimized version of Fourier Transforms were neccessary
to achieve live speed.

# Progress Plan
1. Learn about the uses of OpenCV.
2. Become more familiar with the Eulerian Video Magnification research.
3. Test several sample programs that utilizes the webcam. (OpenFrameworks has example programs)
4. Setup and become familiar with FFTW library and its various approximations and uses.
5. Use webcam feeds and apply it to Eulerian Video Magnification and FFTW algorithms.
6. Tune the program until there is consistency and accuracy with the calulated heart rates.
7. Create a real time OpenFrameworks application that displays the video feed and calulcated
   heartrate.
8. If time remains, implement extensions to increase applicability of the project.

# Other Extensions to Consider
1. Turn this into an application that stores heart rate data for medical tracking.
	a. Keep a record of heart rate fluctuations (daily) to keep track of health trends/goals.
2. Track multiple faces at once and return heart rates for each individual.
3. Allow consistent tracking even with slight disturbances in the video stream.
	a. This would make the project more accessable to smartphone users.
4. Find and play music that would be in tune with heart rate for workout purposes.
