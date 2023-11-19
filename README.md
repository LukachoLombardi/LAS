# LAS
Lukacho's Amazing Scheduler! (for Arduino)

LAS is a very simple and easy to use cooperative scheduler for any board that can be programmed using Arduino.h and Arduino's flavour of C++.

## What can LAS do?
LAS can:
- create basic, function or method based, Tasks with a variety of parameters
- now also create tasks from contextual objects extending the "Callable" class
- execute timed code
- execute repeated code
- allow you to manipulate Tasks from within their included functions for maximum control
- have an accuracy of 50ms> most of the time
Most importantly, LAS can easily help you get rid of the pitfalls that can arise from using delays or basic millis() based timing when combined with recurring tasks.

## Basic instructions
all LAS functions and fields can be accessed through its namespace. LAS is fully accessible and allows you to manipulate most of its parts at runtime.
- LAS::scheduleFunction: schedule a paramterless void function as an internal task with a variety of timing and repeat options
- LAS::scheduleIn: A handy shortcut for simple scheduling of a function in x milliseconds
- LAS::scheduleRepeated: shortcut for creating a simple repeated task
- LAS::schedulerInit: Start LAS. You can pass an instance of arduLogger (or a wrapper for another logger following arduLoggers definition) or let LAS create one internally, if you prefer to use another logger for the rest of your project.

- ALL OF THE ABOVE FUNCTIONS ARE NOW AVAILABLE TO BE USED WITH CALLABLES AS WELL.
- Simply derive a class from "Callable" and override the run method with your function, then pass an object as previously done as a pointer. You can also include environment variables or return pointers for your void function.

## Why did I even make this?
mostly for fun, secondly for an Arduino based robot project. 
