#ifndef __POSITIONING_H
#define __POSITIONING_H

#ifdef __cplusplus
extern "C" {
#endif

// ******************************************************************

void deprecated_place_object (tExpression Evaluator);  // << Isaac: this function passes expression by copy and has been replaced by the pointer version
void place_object(tExpression* pEvaluator);

// ******************************************************************

#ifdef __cplusplus
}
#endif

#endif
