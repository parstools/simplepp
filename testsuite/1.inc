#ifdef var1
var1
#else
not var1
#endif

#include "2.inc"

#if var2<100
var2<100
 #elif var2<1000
var2 100..1000
 #elif var2>2000
var2 >2000
 #else 
var2 : 1000.2000
#endif
