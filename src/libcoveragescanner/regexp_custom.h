/* invisible customization header for regexp implementation */

#include <stdlib.h>
#include <string.h>


/* undefine this to remove some extra checks */
#define REGEXP_EXTRA_CHECKS
    
/* internal error handler -- you need to provide an implementation.
 * By default, this does nothing, but you may want to send stuff to
 * the debug log or something. */
void re_report(const char* error);

/* unicode handlers for translating stuff.  Provide a sane implementation
 * for your character set.  A default implementation is provided and
 * it kind of sucks.
 *
 * If REGEXP_UNICODE is not defined, those functions won't be used. */
#ifdef REGEXP_UNICODE
CHAR_TYPE* re_ansi_to_unicode(const char* s);
char* re_unicode_to_ansi(const CHAR_TYPE* s);
#endif

