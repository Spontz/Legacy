/*
	exprevaldriver.c: driver to expreval library
*/

#include <float.h>

#include "exprevaldriver.h"
#include "gldriver.h"
#include "../dkernel.h"

int removeSpaces(char* Str)
	{
	unsigned int i = 0;
	unsigned int p = 0;

	if (Str==0)
		return 0;

	for (; i < strlen(Str); ++i)
		if (Str[i] != ' ')
			Str[p++] = Str[i];
	Str[p] = '\0';

	return i-p;
	}

EXPRTYPE saturate(EXPRTYPE in)
	{
	return (in < 0) ? 0 : ( (in > 1) ? 1 : in );
	}

EXPR_FUNCTIONSOLVER(expr_smoothstep)
	{
	int err;
	EXPRTYPE v0, v1, t, x;

	EXPR_REQUIRECOUNT(3);

	EXPR_EVALNODE(0, v0);
	EXPR_EVALNODE(1, v1);
	EXPR_EVALNODE(2, t);

	if (fabs(v1 - v0) < FLT_EPSILON)
		EXPR_RETURNSOFTERR(EXPR_ERROR_OUTOFRANGE);

	EXPR_CLEARMATHERR();

	x = saturate((t - v0) / (v1 - v0)); // the division by 0 has been already controlled
	*val = x*x*(3 - 2*x);

	EXPR_CHECKMATHERR();

	return EXPR_ERROR_NOERROR;
	}

EXPR_FUNCTIONSOLVER(expr_hermite_interpolation)
	{
	int err;
	EXPRTYPE v0, v1, t;

	EXPR_REQUIRECOUNT(3);

	EXPR_EVALNODE(0, v0);
	EXPR_EVALNODE(1, v1);
	EXPR_EVALNODE(2, t);

	if (fabs(v1 - v0) < FLT_EPSILON)
		EXPR_RETURNSOFTERR(EXPR_ERROR_OUTOFRANGE);

	EXPR_CLEARMATHERR();

	*val = v0 + t*t*(3 - 2*t)*(v1-v0);

	EXPR_CHECKMATHERR();

	return EXPR_ERROR_NOERROR;
	}

void initExpression(tExpression *myExpression)
	{
	static int	count = 0;

	int			removedSpacesCount;
	char		num[1024];

	myExpression->pOwner = mySection;

	removedSpacesCount = removeSpaces(myExpression->equation);
	
	dkernel_trace("exprevaldriver: initExpression: [%s] (%d spaces removed)", myExpression->equation, removedSpacesCount);
	
	sprintf(num, "%d", count++);
	//if (count>141)
	//MessageBox(0, myExpression->equation, num, 0);

	myExpression->err = exprFuncListCreate(&myExpression->f);
	if(myExpression->err != EXPR_ERROR_NOERROR)
		{
		dkernel_error("Function List Creation Error %d\n", myExpression->err);
		return;
		}
	
	/* Initialize internal functions */
	myExpression->err = exprFuncListInit(myExpression->f);
	if(myExpression->err != EXPR_ERROR_NOERROR)
		{
		dkernel_error("Function List Initialization Error %d\n", myExpression->err);
		dkernel_error("Some internal functions may not be usable\n");
		return;
		}
	
	/* Create variable list */
	myExpression->err = exprValListCreate(&myExpression->v);
	if(myExpression->err != EXPR_ERROR_NOERROR)
		{
		dkernel_error("Variable List Creation Error %d\n", myExpression->err);
		exprFuncListFree(myExpression->f);
		return;
		}
	
	/* Create the constant list */
	myExpression->err = exprValListCreate(&myExpression->c);
	if(myExpression->err != EXPR_ERROR_NOERROR)
		{
		dkernel_error("Constant List Creation Error %d\n", myExpression->err);
		exprFuncListFree(myExpression->f);
		exprValListFree(myExpression->v);
		return;
		}
	
	/* Initialize internal constants */
	myExpression->err = exprValListInit(myExpression->c);
	if(myExpression->err != EXPR_ERROR_NOERROR)
		{
		dkernel_error("Constant List Initialization Error %d\n", myExpression->err);
		dkernel_error("Some internal constants may not be usable\n");
		return;
		}
	
	/* Add any application defined functions, constants, or variables to the lists here or down below */
	
	myExpression->err = exprCreate(&myExpression->o, myExpression->f, myExpression->v, myExpression->c, NULL, 0, NULL);
	if(myExpression->err != EXPR_ERROR_NOERROR)
		{
		dkernel_error("Expression Object Creation Error %d\n", myExpression->err);
		exprFuncListFree(myExpression->f); //-V525
		exprValListFree(myExpression->c);
		exprValListFree(myExpression->v);
		return;
		}
	
	/* Add any application defined functions, constants, or variables to the lists here or down below.
		This is the last time you can for the functions or constants. */


	// Add custom functions
	exprFuncListAdd(myExpression->f, expr_smoothstep, "ss", 3, 3, 0, 0);
	exprFuncListAdd(myExpression->f, expr_hermite_interpolation, "hi", 3, 3, 0, 0);

	/* Functions and constants may be added or changed here */
	
	myExpression->err = exprParse(myExpression->o, myExpression->equation);
	if(myExpression->err != EXPR_ERROR_NOERROR)
		{
		dkernel_error
			(
			"%s : Error %d parsing section %s expression (layer:%d, start:%.5g, end:%.5g)\n\n%s",
			myExpression->pOwner->DataSource ? myExpression->pOwner->DataSource : "Unknown data source",
			myExpression->err,
			myExpression->pOwner->identifier,
			myExpression->pOwner->layer,
			myExpression->pOwner->startTime,
			myExpression->pOwner->endTime,
			myExpression->equation ? myExpression->equation : "Equation String is NULL"
			);

		/* Free objects and return */
		exprFree(myExpression->o);
		
		/* Free lists */
		}
}

void insertSectionVariables(tExpression *expression)
	{
	// These are the variables that can be used in all formulas
	exprValListAdd(expression->v ,    "t", (EXPRTYPE)mySection->runTime );
	exprValListAdd(expression->v , "tend", (EXPRTYPE)mySection->duration);
	exprValListAdd(expression->v , "beat", (EXPRTYPE)demoSystem.beat    );
	exprValListAdd(expression->v ,  "fps", (EXPRTYPE)demoSystem.fps     );

	// Camera values
	if (demoSystem.camera != NULL) {
		exprValListAdd(expression->v , "cam_eye_x", (EXPRTYPE)demoSystem.camera->eye.x);
		exprValListAdd(expression->v , "cam_eye_y", (EXPRTYPE)demoSystem.camera->eye.y);
		exprValListAdd(expression->v , "cam_eye_z", (EXPRTYPE)demoSystem.camera->eye.z);
		
		exprValListAdd(expression->v , "cam_target_x", (EXPRTYPE)demoSystem.camera->target.x);
		exprValListAdd(expression->v , "cam_target_y", (EXPRTYPE)demoSystem.camera->target.y);
		exprValListAdd(expression->v , "cam_target_z", (EXPRTYPE)demoSystem.camera->target.z);

		exprValListAdd(expression->v , "cam_angle_x", (EXPRTYPE)demoSystem.camera->angle.x);
		exprValListAdd(expression->v , "cam_angle_y", (EXPRTYPE)demoSystem.camera->angle.y);
		exprValListAdd(expression->v , "cam_angle_z", (EXPRTYPE)demoSystem.camera->angle.z);
	} else {
		exprValListAdd(expression->v , "cam_eye_x", 0);
		exprValListAdd(expression->v , "cam_eye_y", 0);
		exprValListAdd(expression->v , "cam_eye_z", 0);

		exprValListAdd(expression->v , "cam_target_x", 0);
		exprValListAdd(expression->v , "cam_target_y", 0);
		exprValListAdd(expression->v , "cam_target_z", 100);

		exprValListAdd(expression->v , "cam_angle_x", 0);
		exprValListAdd(expression->v , "cam_angle_y", 0);
		exprValListAdd(expression->v , "cam_angle_z", 0);
		}
	
	// Graphic constants
	exprValListAdd(expression->v , "vpWidth"    , (EXPRTYPE)glDriver.vpWidth    );
	exprValListAdd(expression->v , "vpHeight"   , (EXPRTYPE)glDriver.vpHeight   );

	exprValListAdd(expression->v , "aspectRatio", (EXPRTYPE)gldrv_get_aspect_ratio());
	}
