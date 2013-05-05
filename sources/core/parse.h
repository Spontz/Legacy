#ifndef __PARSE_H
#define __PARSE_H

#ifdef __cplusplus
extern "C" {
#endif

// ******************************************************************

void              getKeyValue    (char *line, char  *key,    char *value                     );
int               getFloatVector (char *line, float *vector, int max                         );
unsigned long int getNextLine    (const char *pText, char *pResult, unsigned long int startPosition );

// ******************************************************************

#ifdef __cplusplus
}
#endif

#endif
