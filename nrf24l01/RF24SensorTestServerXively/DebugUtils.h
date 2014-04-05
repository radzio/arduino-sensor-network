#ifdef DEBUG
#define DEBUG_PRINT(x) Serial.print (x);
#define DEBUG_PRINTF(x) Serial.print (F(x));
#define DEBUG_PRINTFLN(x) Serial.println (F(x));
#define DEBUG_PRINTDEC(x) Serial.print (x, DEC);
#define DEBUG_PRINTLN(x) Serial.println (x);
#else
#define DEBUG_PRINT(x)
#define DEBUG_PRINTF(x)
#define DEBUG_PRINTDEC(x)
#define DEBUG_PRINTLN(x) 
#define DEBUG_PRINTFLN(x);
#endif 



#define DEBUG_PRINT_DETAILS(str)        \
	Serial.print(millis());     \
	Serial.print(": ");         \
	Serial.print(__PRETTY_FUNCTION__); \
	Serial.print(' ');          \
	Serial.print(__FILE__);     \
	Serial.print(':');          \
	Serial.print(__LINE__);     \
	Serial.print(' ');          \
	Serial.println(str);