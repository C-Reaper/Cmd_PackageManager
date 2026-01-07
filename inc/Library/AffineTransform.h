#ifndef AFFINETRANSFORM_H
#define AFFINETRANSFORM_H

#include "./WindowEngine1.0.h"
#include "./TransformedView.h"
#include "./Math.h"

typedef M3x3 AffineTransform;

AffineTransform AffineTransform_New(){
	AffineTransform at = M3x3_Identity();
	return at;
}
void AffineTransform_Pos(AffineTransform* at,Vec2 p){
	M3x3 tl = M3x3_Trans(p.x,p.y,1.0f);
	*at = M3x3_M3x3_Mul(*at,tl);
}
void AffineTransform_Len(AffineTransform* at,Vec2 l){
	M3x3 tl = M3x3_Scale(l.x,l.y,1.0f);
	*at = M3x3_M3x3_Mul(*at,tl);
}
void AffineTransform_Rotate(AffineTransform* at,float angleR){
	M3x3 tl = M3x3_RotateZ(angleR);
	*at = M3x3_M3x3_Mul(*at,tl);
}
Vec2 AffineTransform_Calc(AffineTransform* at,Vec2 p){
	Vec3 v = M3x3_VecMul((Vec3){ p.x,p.y,1.0f },*at);
	return (Vec2){ v.x,v.y };
}
Vec2 AffineTransform_CalcBack(AffineTransform* at,Vec2 p){
	Vec3 v = M3x3_VecMul((Vec3){ p.x,p.y,1.0f },M3x3_Inverse(*at));
	return (Vec2){ v.x,v.y };
}

#endif //!AFFINETRANSFORM_H