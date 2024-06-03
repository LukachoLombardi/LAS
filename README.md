# LAS
Lukacho's Amazing Scheduler! (for Arduino)

LAS is a very simple and easy to use cooperative scheduler for any board that can be programmed using Arduino.h and Arduino's flavour of C++.

Depends on [arduLogger](https://github.com/LukachoLombardi/arduLogger)

## What can LAS do?
LAS can:
- create basic, function or method based, Tasks with a variety of parameters
- **now also create tasks from contextual objects extending the "Callable" class**
- execute timed code
- execute repeated code
- allow you to manipulate Tasks from within their included functions for maximum control
- have an accuracy of less than a ms while doing so

Most importantly, LAS can easily help you get rid of the pitfalls that can arise from using delays or basic millis() based timing when combined with recurring tasks.

## Basic instructions
~~all LAS functions and fields can be accessed through its namespace. LAS is fully accessible and allows you to manipulate most of its parts at runtime.~~ LAS is now an object type you can create, just as any sane person would expect it to be.
- scheduleFunction: schedule a paramterless void function as an internal task with a variety of timing and repeat options
- scheduleIn: A handy shortcut for simple scheduling of a function in x milliseconds
- scheduleRepeated: shortcut for creating a simple repeated task
- schedulerInit: <s>Start LAS</s>. Initialize LAS. Requires an arduLogger and an instance of LASConfig.
- schedulerStart: actually starts LAS after successful initialization. Place first schedulings inbetween init and start.

- ALL OF THE ABOVE FUNCTIONS ARE NOW AVAILABLE TO BE USED WITH CALLABLES AS WELL.
- Simply derive a class from "Callable" and override the run method with your function, then pass an object as previously done as a pointer. You can also include environment variables or return pointers for your void function.
- new: there's an overridable "onFinish" method now. No "onCreate" for now though, 
as actually running it on creation seems kind of pointless, and I don't really want to add another Task field to count ececutions done.

What to expect in the future:
- The abovementioned "onCreate" method will probably come sometime in the future
- also I'm planning to optimize LAS a bit by cutting down on unused fields on non-repeating tasks.

## Why did I even make this and not just use an existing project?
mostly for fun! This project really helped me in understanding some of C++'s low level concepts better. 
Also I prefer working with lightweight implementations I fully understand instead of immediately adapting an existing framework with lots of overhead.

This is also actively used for an Arduino based robot project (still very WIP): 
See [Philipp Räuchle](https://github.com/LukachoLombardi/PhilippRaeuchle) for more.
