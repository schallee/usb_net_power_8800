#ifndef cc_h
#define cc_h

#ifdef __GNUC__
#       define CC_UNUSED_VAR          __attribute__((__unused__))
#       define CC_PRINTF(fmt,first)   __attribute__((format(printf, fmt, first)))
#       define CC_NO_RETURN           __attribute__((noreturn))
#else
#       define CC_UNUSED_VAR  
#       define CC_PRINTF(fmt,first)
#       define CC_NO_RETURN
#endif

#endif
