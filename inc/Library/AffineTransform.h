#ifndef AFFINETRANSFORM_H
#define AFFINETRANSFORM_H

#include "./Math.h"
#include "./Rect.h"

typedef M3x3 AffineTransform;

AffineTransform AffineTransform_New(){
	AffineTransform at = M3x3_Identity();
	return at;
}
void AffineTransform_Identity(AffineTransform* at){
	*at = M3x3_Identity();
}
void AffineTransform_Trans(AffineTransform* at,Vec2 p){
	M3x3 tl = M3x3_Trans(p.x,p.y,1.0f);
	*at = M3x3_M3x3_Mul(*at,tl);
}
void AffineTransform_Scale(AffineTransform* at,Vec2 l){
	M3x3 tl = M3x3_Scale(l.x,l.y,1.0f);
	*at = M3x3_M3x3_Mul(*at,tl);
}
void AffineTransform_RotateX(AffineTransform* at,float angleR){
	M3x3 tl = M3x3_RotateX(angleR);
	*at = M3x3_M3x3_Mul(*at,tl);
}
void AffineTransform_RotateY(AffineTransform* at,float angleR){
	M3x3 tl = M3x3_RotateY(angleR);
	*at = M3x3_M3x3_Mul(*at,tl);
}
void AffineTransform_RotateZ(AffineTransform* at,float angleR){
	M3x3 tl = M3x3_RotateZ(angleR);
	*at = M3x3_M3x3_Mul(*at,tl);
}
void AffineTransform_Inverse(AffineTransform* at){
	*at = M3x3_Inverse(*at);
}
Vec2 AffineTransform_Calc(AffineTransform* at,Vec2 p){
	Vec3 v = M3x3_VecMul((Vec3){ p.x,p.y,1.0f },*at);
	return (Vec2){ v.x,v.y };
}
Rect AffineTransform_Rect(AffineTransform* at,Rect r){
	const Vec2 tl = r.p;
	const Vec2 tr = Vec2_Add(r.p,(Vec2){ r.d.x,0.0f });
	const Vec2 bl = Vec2_Add(r.p,(Vec2){ 0.0f,r.d.y });
	const Vec2 br = Vec2_Add(r.p,r.d);

	const Vec2 t_tl = AffineTransform_Calc(at,tl);
	const Vec2 t_tr = AffineTransform_Calc(at,tr);
	const Vec2 t_bl = AffineTransform_Calc(at,bl);
	const Vec2 t_br = AffineTransform_Calc(at,br);

	const F32 xmin = F32_Min(F32_Min(t_tl.x,t_tr.x),F32_Min(t_bl.x,t_br.x));
	const F32 xmax = F32_Max(F32_Max(t_tl.x,t_tr.x),F32_Max(t_bl.x,t_br.x));
	const F32 ymin = F32_Min(F32_Min(t_tl.y,t_tr.y),F32_Min(t_bl.y,t_br.y));
	const F32 ymax = F32_Max(F32_Max(t_tl.y,t_tr.y),F32_Max(t_bl.y,t_br.y));
	return Rect_New((Vec2){ xmin,ymin },(Vec2){ xmax - xmin,ymax - ymin });
}

#endif //!AFFINETRANSFORM_H