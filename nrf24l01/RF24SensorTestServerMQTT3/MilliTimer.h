// 2009-02-13 <jc@wippler.nl> http://opensource.org/licenses/mit-license.php

#ifndef MilliTimer_H
#define MilliTimer_H
#endif

/// @file
/// Ports library definitions.

#if ARDUINO >= 100
#include <Arduino.h> // Arduino 1.0
#else
#include <WProgram.h> // Arduino 0022
#endif
#include <stdint.h>
#include <avr/pgmspace.h>
//#include <util/delay.h>

// tweak this to switch ATtiny84 etc to new Arduino 1.0+ conventions
// see http://arduino.cc/forum/index.php/topic,51984.msg371307.html#msg371307
// and http://forum.jeelabs.net/node/1567
#if ARDUINO >= 100
#define WRITE_RESULT size_t
#else
#define WRITE_RESULT void
#endif

class MilliTimer {
	word next;
	byte armed;
public:
	MilliTimer() : armed(0) {}

	/// poll until the timer fires
	/// @param ms Periodic repeat rate of the time, omit for a one-shot timer.
	byte poll(word ms = 0);
	/// Return the number of milliseconds before the timer will fire
	word remaining() const;
	/// Returns true if the timer is not armed
	byte idle() const { return !armed; }
	/// set the one-shot timeout value
	/// @param ms Timeout value. Timer stops once the timer has fired.
	void set(word ms);
};







class Scheduler {
	word* tasks;
	word remaining;
	byte maxTasks;
	MilliTimer ms100;
public:
	/// initialize for a specified maximum number of tasks
	Scheduler(byte max);
	Scheduler(word* buf, byte max);

	/// Return next task to run, -1 if there are none ready to run, but there
	/// are tasks waiting, or -2 if there are no tasks waiting (i.e. all idle)
	char poll();
	/// same as poll, but wait for event in power-down mode.
	/// Uses Sleepy::loseSomeTime() - see comments there re requiring the
	/// watchdog timer.
	char pollWaiting();

	/// set a task timer, in tenths of seconds
	void timer(byte task, word tenths);
	/// cancel a task timer
	void cancel(byte task);

	/// return true if a task timer is not running
	byte idle(byte task) { return tasks[task] == ~0U; }
};

/// Low-power utility code using the Watchdog Timer (WDT). Requires a WDT
/// interrupt handler, e.g. EMPTY_INTERRUPT(WDT_vect);
class Sleepy {
public:
	/// start the watchdog timer (or disable it if mode < 0)
	/// @param mode Enable watchdog trigger after "16 << mode" milliseconds
	/// (mode 0..9), or disable it (mode < 0).
	/// @note If you use this function, you MUST included a definition of a WDT
	/// interrupt handler in your code. The simplest is to include this line:
	///
	/// ISR(WDT_vect) { Sleepy::watchdogEvent(); }
	///
	/// This will get called when the watchdog fires.
	static void watchdogInterrupts(char mode);

	/// enter low-power mode, wake up with watchdog, INT0/1, or pin-change
	static void powerDown();

	/// Spend some time in low-power mode, the timing is only approximate.
	/// @param msecs Number of milliseconds to sleep, in range 0..65535.
	/// @returns 1 if all went normally, or 0 if some other interrupt occurred
	/// @note If you use this function, you MUST included a definition of a WDT
	/// interrupt handler in your code. The simplest is to include this line:
	///
	/// ISR(WDT_vect) { Sleepy::watchdogEvent(); }
	///
	/// This will get called when the watchdog fires.
	static byte loseSomeTime(word msecs);

	/// This must be called from your watchdog interrupt code.
	static void watchdogEvent();
};