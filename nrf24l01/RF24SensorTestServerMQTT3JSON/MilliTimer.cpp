 #include "MilliTimer.h"
#include <avr/sleep.h>
#include <util/atomic.h>



// ATtiny84 has BODS and BODSE for ATtiny84, revision B, and newer, even though
// the iotnx4.h header doesn't list it, so we *can* disable brown-out detection!
// See the ATtiny24/44/84 datasheet reference, section 7.2.1, page 34.
#if (defined(__AVR_ATtiny84__) || defined(__AVR_ATtiny44__)) && \
	!defined(BODSE) && !defined(BODS)
#define BODSE 2
#define BODS 7
#endif

// flag bits sent to the receiver
#define MODE_CHANGE 0x80 // a pin mode was changed
#define DIG_CHANGE 0x40 // a digital output was changed
#define PWM_CHANGE 0x30 // an analog (pwm) value was changed on port 2..3
#define ANA_MASK 0x0F // an analog read was requested on port 1..4


byte MilliTimer::poll(word ms) {
	byte ready = 0;
	if (armed) {
		unsigned long remain = next - millis();
		// since remain is unsigned, it will overflow to large values when
		// the timeout is reached, so this test works as long as poll() is
		// called no later than 5535 millisecs after the timer has expired
		//if (remain <= 60000)
		//	return 0;
		// return a value between 1 and 255, being msecs+1 past expiration
		// note: the actual return value is only reliable if poll() is
		// called no later than 255 millisecs after the timer has expired
		ready = -remain;
	}
	set(ms);
	return ready;
}

word MilliTimer::remaining() const {
	unsigned long remain = armed ? next - millis() : 0;
	return remain;
}

void MilliTimer::set(word ms) {
	armed = ms != 0;
	if (armed)
		next = millis() + ms - 1;
}





// ISR(WDT_vect) { Sleepy::watchdogEvent(); }

static volatile byte watchdogCounter;

void Sleepy::watchdogInterrupts(char mode) {
	// correct for the fact that WDP3 is *not* in bit position 3!
	if (mode & bit(3))
		mode ^= bit(3) | bit(WDP3);
	// pre-calculate the WDTCSR value, can't do it inside the timed sequence
	// we only generate interrupts, no reset
	byte wdtcsr = mode >= 0 ? bit(WDIE) | mode : 0;
	MCUSR &= ~(1 << WDRF);
	ATOMIC_BLOCK(ATOMIC_FORCEON) {
#ifndef WDTCSR
#define WDTCSR WDTCR
#endif
		WDTCSR |= (1 << WDCE) | (1 << WDE); // timed sequence
		WDTCSR = wdtcsr;
	}
}

/// @see http://www.nongnu.org/avr-libc/user-manual/group__avr__sleep.html
void Sleepy::powerDown() {
	byte adcsraSave = ADCSRA;
	ADCSRA &= ~bit(ADEN); // disable the ADC
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	ATOMIC_BLOCK(ATOMIC_FORCEON) {
		sleep_enable();
		// sleep_bod_disable(); // can't use this - not in my avr-libc version!
#ifdef BODSE
		MCUCR = MCUCR | bit(BODSE) | bit(BODS); // timed sequence
		MCUCR = (MCUCR & ~bit(BODSE)) | bit(BODS);
#endif
	}
	sleep_cpu();
	sleep_disable();
	// re-enable what we disabled
	ADCSRA = adcsraSave;
}

byte Sleepy::loseSomeTime(word msecs) {
	byte ok = 1;
	word msleft = msecs;
	// only slow down for periods longer than the watchdog granularity
	while (msleft >= 16) {
		char wdp = 0; // wdp 0..9 corresponds to roughly 16..8192 ms
		// calc wdp as log2(msleft/16), i.e. loop & inc while next value is ok
		for (word m = msleft; m >= 32; m >>= 1)
		if (++wdp >= 9)
			break;
		watchdogCounter = 0;
		watchdogInterrupts(wdp);
		powerDown();
		watchdogInterrupts(-1); // off
		// when interrupted, our best guess is that half the time has passed
		word halfms = 8 << wdp;
		msleft -= halfms;
		if (watchdogCounter == 0) {
			ok = 0; // lost some time, but got interrupted
			break;
		}
		msleft -= halfms;
	}
	// adjust the milli ticks, since we will have missed several
#if defined(__AVR_ATtiny84__) || defined(__AVR_ATtiny85__) || defined (__AVR_ATtiny44__) || defined (__AVR_ATtiny45__)
	extern volatile unsigned long millis_timer_millis;
	millis_timer_millis += msecs - msleft;
#else
	extern volatile unsigned long timer0_millis;
	timer0_millis += msecs - msleft;
#endif
	return ok; // true if we lost approx the time planned
}

void Sleepy::watchdogEvent() {
	++watchdogCounter;
}

Scheduler::Scheduler(byte size) : remaining(~0), maxTasks(size) {
	byte bytes = size * sizeof *tasks;
	tasks = (word*)malloc(bytes);
	memset(tasks, 0xFF, bytes);
}

Scheduler::Scheduler(word* buf, byte size) : tasks(buf), remaining(~0), maxTasks(size) {
	byte bytes = size * sizeof *tasks;
	memset(tasks, 0xFF, bytes);
}

char Scheduler::poll() {
	// all times in the tasks array are relative to the "remaining" value
	// i.e. only remaining counts down while waiting for the next timeout
	if (remaining == 0) {
		word lowest = ~0;
		for (byte i = 0; i < maxTasks; ++i) {
			if (tasks[i] == 0) {
				tasks[i] = ~0;
				return i;
			}
			if (tasks[i] < lowest)
				lowest = tasks[i];
		}
		if (lowest != ~0U) {
			for (byte i = 0; i < maxTasks; ++i) {
				if (tasks[i] != ~0U) {
					tasks[i] -= lowest;
				}
			}
		}
		else {
			// must turn off timer or it might overflow if its poll-method
			// is not called within 5535 ms, i.e. if no tasks are scheduled
			ms100.set(0);
		}
		remaining = lowest;
	}
	else if (remaining == ~0U) //remaining == ~0 means nothing running
		return -2;
	else if (ms100.poll(100))
		--remaining;
	return -1;
}

char Scheduler::pollWaiting() {
	if (remaining == ~0U) // Nothing running!
		return -2;
	// first wait until the remaining time we need to wait is less than 0.1s
	while (remaining > 0) {
		word step = remaining > 600 ? 600 : remaining;
		if (!Sleepy::loseSomeTime(100 * step)) // uses least amount of power
			return -1;
		remaining -= step;
	}
	// now lose some more time until that 0.1s mark
	if (!Sleepy::loseSomeTime(ms100.remaining()))
		return -1;
	// lastly, just ignore the 0..15 ms still left to go until the 0.1s mark
	return poll();
}

void Scheduler::timer(byte task, word tenths) {
	// if new timer will go off sooner than the rest, then adjust all entries
	if (tenths < remaining) {
		word diff = remaining - tenths;
		for (byte i = 0; i < maxTasks; ++i)
		if (tasks[i] != ~0U)
			tasks[i] += diff;
		remaining = tenths;
	}
	tasks[task] = tenths - remaining;
}

void Scheduler::cancel(byte task) {
	tasks[task] = ~0;
}
