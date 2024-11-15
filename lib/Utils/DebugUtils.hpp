#pragma once

#ifdef DEBUG
#define debug(msg) Serial.print((String)msg + "\n")
#else
#define debug(msg) // empty, simply replace with nothing
#endif