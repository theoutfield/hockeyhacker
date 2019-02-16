# hockeyhacker
Electronic Hockey Net Targets.  Kind of like the ones used in the NHL skills competition.


Project: Hockey Shot Electronic Net Target
Author : Tony Fountaine
Year   : 2019
License: MIT  
Desc   : My son was watching the NHL Skills competition and wanted 
         the light up targets used in the sharp shooter competition
         I wanted to buy some for him but the price was ridiculous.
         I won't name the company but the price was in the thousands.
         
         The design is really simple.  Just an accelerometer and an
         STM32 Blue Pill with Arduino firmware.  There is a WS2812
         LED strip that allows the target to change colors when hit.
         The current design uses an MPU6050 3Axis Accel and 3 Axis
         Gyro. I use the 3Axis accel readings to compute the mag.
         This is currently saturating so I'm planning on adding
         a version with the ST H3LIS331DL 400g. The original design was  
         chosen to create the cheapest possible version.  The blue
         pill is about $2-3 dollars and the MPU6050 is about $2.
         The LED Strips cost about $30 for 5meters maybe less. I cut
         up the strips and soldered them to the back of a 3/8" HDPE
         puck board.  The HDPE board is white and does a nice job
         of diffusing the LED lights.  You could go cheap and just use
         a single LED.....or something.  All in all you can make 4 targets
         for less than $100.  They work great and are very durable.  I
         believe HDPE is the same material that hockey rinks are made of.
         
         This project is kind of interesting in the sense that some
         young hockey players may take an interest in electronics and
         coding in an attempt to create something fun for themselves.
         That's my hope anyway.  There may be some old guys who want 
         to give it a shot as well.  It's a cool little project that
         I think anyone could put together.  You need a soldering iron
         to put the electronics together and a saw to cut the HDPE.   
         I actually repurposed the old foam hockey target as the casing
         for the HDPE.  You can buy them for about $15-$20.  I used some
         L brackets to hold the HDPE in.  Just drilled a hole in the side
         and screwed it in.
           
         The targets will currently work independently.  I am working 
         on a Raspberry Pi 3+ main controller to sequence the targets.
         Each target will connect to the Pi over USB.  This will allow 
         all sorts of game modes to be created.  
         I was originally going to write this in C++ but I'm leaning 
         towards using Node.js and Javascript so that it is easier for 
         someone to learn and modify.  I am testing it to see if it 
         performs well enough.  It will have a webpage to read the stats
         and start/restart the game modes from a Phone/Tablet.  
         All of the data will be saved in an SQLite database.
           
         Have fun!  I'm sure there are lots of improvements and ideas
         to take this project in many directions.  I added a lot of 
         code comments that I wouldn't normally do just in case someone
         is learning how to code.

