#ifndef __EXPREVALDRIVER_H
#define __EXPREVALDRIVER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <math.h>
#include <errno.h>
#include "../../expreval/expreval.h"
#include "../dkernel.h"

typedef struct
	{
	// Equation and source of the equation
	char*			equation;
	tDemoSection*	pOwner;			// demo section that owns this expression, use 0 if not aplicable
	
	// Variables for the expression evaluation library
	exprFuncList*	f;				// Functions list
	exprValList*	v;				// Values list
	exprValList*	c;				// Constants list
	exprObj*		o;				// The expression object
	EXPRTYPE		result;			// The result object
	int				err;			// Contains an error (if there was an error)
	} tExpression;

void insertSectionVariables(tExpression *myExpression);
void initExpression(tExpression *myExpression);

#ifdef __cplusplus
}
#endif

#endif
