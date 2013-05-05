#include "../main.h"

void deprecated_place_object(tExpression Evaluator)
	{
	place_object(&Evaluator);
	}

void place_object(tExpression* pEvaluator) {
	double tx, ty, tz; // Position
	double rx, ry, rz; // Rotation
	double sx, sy, sz; // Scale

	// Insert the default section values into the size equation
	insertSectionVariables(pEvaluator);
	
	// Evaluate the equations
	pEvaluator->err = exprEval(pEvaluator->o, &pEvaluator->result);

	// Check for errors
	if(pEvaluator->err != EXPR_ERROR_NOERROR)
		section_error("place_object: Expression evaluation Error (%d): %s", pEvaluator->err, pEvaluator->equation);
	
	exprValListGet(pEvaluator->v, "tx", &tx);
	exprValListGet(pEvaluator->v, "ty", &ty);
	exprValListGet(pEvaluator->v, "tz", &tz);

	exprValListGet(pEvaluator->v, "rx", &rx);
	exprValListGet(pEvaluator->v, "ry", &ry);
	exprValListGet(pEvaluator->v, "rz", &rz);
	
	exprValListGet(pEvaluator->v, "sx", &sx);
	exprValListGet(pEvaluator->v, "sy", &sy);
	exprValListGet(pEvaluator->v, "sz", &sz);
	
	glTranslated (tx, ty, tz );
	glRotated    (rx, 1, 0, 0);
	glRotated    (ry, 0, 1, 0);
	glRotated    (rz, 0, 0, 1);
	glScaled     (sx, sy, sz );
}