#ifndef CALLSTACK_H
#define CALLSTACK_H

#include "../Container/Vector.h"
#include "CStr.h"
#include "String.h"
#include "Files.h"
#include "AlxParser.h"
#include "AlxScope.h"
#include "AlxShutingYard.h"
#include "AlxExternFunctions.h"



#define FUNCTIONRT_NONE         0
#define FUNCTIONRT_CALL         1
#define FUNCTIONRT_ARG0         2
#define FUNCTIONRT_UNFINISHED   3
#define FUNCTIONRT_JMP          4
#define FUNCTIONRT_FULLRETURN   5

typedef unsigned char FunctionRT;

#define CALLPOSITION_INVALID    -1
#define FUNCTION_CALLED         -2

typedef struct CallPosition {
    TokenMap tm;
    CStr fname;
    TT_Iter pos;
    int range;
    TT_Type type;
    FunctionRT sy;
} CallPosition;

CallPosition CallPosition_New(TT_Type tt,TT_Iter it) {
    CallPosition cp;
    cp.tm = TokenMap_Null();
    cp.type = tt;
    cp.pos = it;
    cp.fname = NULL;
    cp.sy = FUNCTIONRT_NONE;
    return cp;
}
CallPosition CallPosition_New_N(TT_Type tt,TT_Iter it,CStr name) {
    CallPosition cp;
    cp.tm = TokenMap_Null();
    cp.type = tt;
    cp.pos = it;
    cp.fname = CStr_Cpy(name);
    cp.sy = FUNCTIONRT_NONE;
    return cp;
}
CallPosition CallPosition_Make(TokenMap tm,TT_Type type,TT_Iter pos,CStr fname,int range) {
    CallPosition cp;
    cp.tm = tm;
    cp.type = type;
    cp.pos = pos;
    cp.fname = CStr_Cpy(fname);
    cp.range = range;
    cp.sy = FUNCTIONRT_NONE;
    return cp;
}
CallPosition CallPosition_Null() {
    CallPosition cp;
    cp.tm = TokenMap_Null();
    cp.type = TOKEN_NONE;
    cp.pos = CALLPOSITION_INVALID;
    cp.fname = NULL;
    cp.sy = FUNCTIONRT_NONE;
    return cp;
}
void CallPosition_Free(CallPosition* cp) {
    TokenMap_Free(&cp->tm);
    CStr_Free(&cp->fname);
}
void CallPosition_Set(CallPosition* dst,CallPosition* src) {
    CallPosition_Free(dst);
    *dst = *src;
}
void CallPosition_Print(CallPosition* cp) {
    printf("--- CallPosition ---\n");
    printf("%d: Pos: %ld: to %s | ",cp->type,cp->pos,cp->fname);
    TokenMap_Print_S(&cp->tm);
    printf("\n");
    printf("-----------------\n");
}
void CallPosition_Print_S(CallPosition* cp) {
    printf("%d: Pos: %ld: to %s | ",cp->type,cp->pos,cp->fname);
    TokenMap_Print_S(&cp->tm);
}

typedef Vector CallStack;

CallStack CallStack_New() {
    CallStack cs = Vector_New(sizeof(CallPosition));
    return cs;
}
void CallStack_Call(CallStack* cs,TokenMap* ptm,TT_Type type,TT_Iter pos,CStr name,int range) {
    TokenMap tm = TokenMap_Cpy(ptm);
    CallPosition cp = CallPosition_Make(tm,type,pos,name,range);
    Vector_Push(cs,&cp);
}
CallPosition* CallStack_FuncPeek(CallStack* cs) {
    for(int i = cs->size-1;i>=0;i--){
        CallPosition* cp = (CallPosition*)Vector_Get(cs,i);
        if(cp->type==TOKEN_FUNCTION){
            return cp;
        }
    }
    return NULL;
}
TokenMap CallStack_FuncPop(CallStack* cs,int* range) {
    for(int i = cs->size-1;i>=0;i--){
        CallPosition* cp = (CallPosition*)Vector_Get(cs,i);

        if(cp->type==TOKEN_FUNCTION){
            if(range) *range = cp->range;
            
            TokenMap cpy = TokenMap_Cpy(&cp->tm);
            for(int j = cs->size-1;j>=i;j--){
                CallPosition* cpfree = (CallPosition*)Vector_Get(cs,j);
                CallPosition_Free(cpfree);
                Vector_PopTop(cs);
            }
            return cpy;
        }
    }
    return TokenMap_Null();
}
CallPosition CallStack_FuncReturn(CallStack* cs,int* range) {
    for(int i = cs->size-1;i>=0;i--){
        CallPosition* cp = (CallPosition*)Vector_Get(cs,i);
        if(cp->type==TOKEN_FUNCTION){
            if(range) *range = cp->range;
            
            for(int j = cs->size-1;j>i;j++){
                CallPosition* cpfree = (CallPosition*)Vector_Get(cs,j);
                CallPosition_Free(cpfree);
                Vector_PopTop(cs);
            }
            CallPosition* ret = (CallPosition*)Vector_Get(cs,cs->size-1);
            CallPosition tokm = *ret;
            return tokm;
        }
    }
    return CallPosition_Null();
}
CallPosition* CallStack_Peek_T(CallStack* cs,TT_Type tt) {
    for(int i = cs->size-1;i>=0;i--){
        CallPosition* cp = (CallPosition*)Vector_Get(cs,i);
        if(cp->type==tt){
            return cp;
        }
    }
    return NULL;
}
CallPosition* CallStack_Peek(CallStack* cs) {
    if(cs->size>0){
        CallPosition* cp = (CallPosition*)Vector_Get(cs,cs->size-1);
        return cp;
    }
    return NULL;
}
CallPosition* CallStack_PrePeek(CallStack* cs) {
    if(cs->size>1){
        CallPosition* cp = (CallPosition*)Vector_Get(cs,cs->size-2);
        return cp;
    }
    return NULL;
}
TT_Type CallStack_Back(CallStack* cs) {
    if(cs->size>0){
        CallPosition* cp = (CallPosition*)Vector_Get(cs,cs->size-1);
        return cp->type;
    }
    return TOKEN_NONE;
}
TT_Type CallStack_PreBack(CallStack* cs) {
    if(cs->size>1){
        CallPosition* cp = (CallPosition*)Vector_Get(cs,cs->size-2);
        return cp->type;
    }
    return TOKEN_NONE;
}
void CallStack_Push(CallStack* cs,CallPosition* cp) {
    Vector_Push(cs,cp);
}
void CallStack_Pop(CallStack* cs) {
    if(cs->size>0){
        CallPosition* cpfree = (CallPosition*)Vector_Get(cs,cs->size-1);
        CallPosition_Free(cpfree);
        Vector_PopTop(cs);
    }
}
void CallStack_Add(CallStack* cs,CallPosition* cp,int i) {
    Vector_Add(cs,cp,i);
}
void CallStack_Remove(CallStack* cs,int i) {
    if(i>=0 && i<cs->size){
        CallPosition* cpfree = (CallPosition*)Vector_Get(cs,i);
        CallPosition_Free(cpfree);
        Vector_Remove(cs,i);
    }
}
void CallStack_GoBack(CallStack* cs,TT_Iter* it) {
    if(cs->size>0){
        CallPosition* cp = (CallPosition*)Vector_Get(cs,cs->size-1);
        *it = cp->pos;
    }
}
void CallStack_Return(CallStack* cs,TT_Type tt) {
    for(int i = cs->size - 1;i>=0;i--){
        CallPosition* cp = (CallPosition*)Vector_Get(cs,i);
        TT_Type t = cp->type;
        CallStack_Pop(cs);

        if(t==tt) break;
    }
}
void CallStack_Free(CallStack* cs) {
    for(int i = 0;i<cs->size;i++){
        CallPosition* cp = (CallPosition*)Vector_Get(cs,i);
        CallPosition_Free(cp);
    }
    Vector_Free(cs);
}
void CallStack_Print(CallStack* cs) {
    printf("--- CallStack ---\n");
    for(int i = 0;i<cs->size;i++){
        CallPosition* tm = (CallPosition*)Vector_Get(cs,i);
        printf("%d: ",i);
        CallPosition_Print_S(tm);
        printf("\n");
    }
    printf("-----------------\n");
}




typedef struct Function {
    TT_Iter pos;
    Boolean access;
    CStr name;
    CStr rettype;
    Vector params;// Vector<Member>
    String text;
} Function;

Function Function_New(TT_Iter pos,CStr name) {
    Function cp;
    cp.pos = pos;
    cp.name = CStr_Cpy(name);
    cp.rettype = NULL;
    cp.params = Vector_New(sizeof(Member));
    cp.text = String_New();
    return cp;
}
Function Function_Make(TT_Iter pos,CStr name,CStr rettype,Member* params) {
    Function cp;
    cp.pos = pos;
    cp.name = CStr_Cpy(name);
    cp.rettype = CStr_Cpy(rettype);
    cp.params = Vector_New(sizeof(Member));
    cp.text = String_New();

    for(int i = 0;params[i].name!=NULL;i++){
        Vector_Push(&cp.params,&params[i]);
    }

    return cp;
}
Function Function_MakeX(TT_Iter pos,Boolean access,CStr name,CStr rettype,Member* params) {
    Function cp;
    cp.pos = pos;
    cp.access = access;
    cp.name = CStr_Cpy(name);
    cp.rettype = CStr_Cpy(rettype);
    cp.params = Vector_New(sizeof(Member));
    cp.text = String_New();

    for(int i = 0;params[i].name!=NULL;i++){
        Vector_Push(&cp.params,&params[i]);
    }

    return cp;
}
Function Function_Null() {
    Function cp;
    cp.pos = CALLPOSITION_INVALID;
    cp.name = NULL;
    cp.rettype = NULL;
    cp.params = Vector_Null();
    cp.text = String_Null();
    return cp;
}
void Function_Free(Function* cp) {
    for(int i = 0;i<cp->params.size;i++){
        Member* m = (Member*)Vector_Get(&cp->params,i);
        Member_Free(m);
    }
    Vector_Free(&cp->params);
    String_Free(&cp->text);
    CStr_Free(&cp->name);
    cp->pos = CALLPOSITION_INVALID;
}
void Function_Print(Function* cp) {
    printf("--- Function ---\n");
    printf("Name: %s\n",cp->name);
    printf("Pos: %ld\n",cp->pos);
    printf("Ret: %s\n",cp->rettype);
    printf("Parameters:\n");
    
    for (int i = 0;i<cp->params.size;i++){
        Member* m = (Member*)Vector_Get(&cp->params,i);
        printf("%s: %s\n",m->name,m->type);
    }
    
    printf("-----------------\n");
}
void Function_Print_S(Function* cp) {
    printf("%s (%ld): %s { ",cp->name,cp->pos,cp->rettype);
    for (int i = 0;i<cp->params.size;i++){
        Member* m = (Member*)Vector_Get(&cp->params,i);
        printf("%s: %s",m->name,m->type);
        if(i<cp->params.size-1) printf(", ");
    }
    printf(" }");
}

typedef Vector FunctionMap;

FunctionMap FunctionMap_New() {
    FunctionMap cs = Vector_New(sizeof(Function));
    return cs;
}
TT_Iter FunctionMap_Find(FunctionMap* cs,CStr name) {
    for(int i = 0;i<cs->size;i++){
        Function* f = (Function*)Vector_Get(cs,i);
        if(CStr_Cmp(f->name,name)) return i;
    }
    return -1;
}
Function* FunctionMap_FindF(FunctionMap* cs,CStr name) {
    for(int i = 0;i<cs->size;i++){
        Function* f = (Function*)Vector_Get(cs,i);
        if(CStr_Cmp(f->name,name)) return f;
    }
    return NULL;
}
Function* FunctionMap_Get(FunctionMap* cs,CStr name) {
    for(int i = 0;i<cs->size;i++){
        Function* f = (Function*)Vector_Get(cs,i);
        if(CStr_Cmp(f->name,name)) return f;
    }
    return NULL;
}
void FunctionMap_Free(FunctionMap* cs) {
    for(int i = 0;i<cs->size;i++){
        Function* cp = (Function*)Vector_Get(cs,i);
        Function_Free(cp);
    }
    Vector_Free(cs);
}
void FunctionMap_Print(FunctionMap* cs) {
    printf("--- FunctionMap ---\n");
    for(int i = 0;i<cs->size;i++){
        Function* tm = (Function*)Vector_Get(cs,i);
        printf("%d: ",i);
        Function_Print_S(tm);
        printf("\n");
    }
    printf("-------------------\n");
}



typedef struct KeywordExecuter {
    TT_Type tt;
    Boolean (*func)(void*,TokenMap*);
} KeywordExecuter;

KeywordExecuter KeywordExecuter_New(TT_Type tt,Boolean (*func)(void*,TokenMap*)) {
    KeywordExecuter cp;
    cp.tt = tt;
    cp.func = func;
    return cp;
}
KeywordExecuter KeywordExecuter_Null() {
    KeywordExecuter cp;
    cp.tt = TOKEN_NONE;
    cp.func = NULL;
    return cp;
}
#define KEYWORDEXECUTER_END KeywordExecuter_Null()

typedef Vector KeywordExecuterMap;

KeywordExecuterMap KeywordExecuterMap_New() {
    KeywordExecuterMap cs = Vector_New(sizeof(KeywordExecuter));
    return cs;
}
KeywordExecuterMap KeywordExecuterMap_Make(KeywordExecuter* initlist) {
    KeywordExecuterMap cs = Vector_New(sizeof(KeywordExecuter));

    for(int i = 0;initlist[i].tt!=TOKEN_NONE && initlist[i].func!=NULL;i++){
        Vector_Push(&cs,&initlist[i]);
    }

    return cs;
}
TT_Iter KeywordExecuterMap_Find(KeywordExecuterMap* cs,TT_Type tt) {
    for(int i = cs->size-1;i>=0;i--){
        KeywordExecuter* ke = (KeywordExecuter*)Vector_Get(cs,i);
        if(ke->tt==tt) return i;
    }
    return -1;
}
Vector KeywordExecuterMap_FindAll(KeywordExecuterMap* cs,TT_Type tt) {
    Vector iters = Vector_New(sizeof(int));
    for(int i = 0;i<cs->size;i++){
        KeywordExecuter* ke = (KeywordExecuter*)Vector_Get(cs,i);
        if(ke->tt==tt) Vector_Push(&iters,(int[]){ i });
    }
    return iters;
}
void KeywordExecuterMap_Free(KeywordExecuterMap* cs) {
    Vector_Free(cs);
}
void KeywordExecuterMap_Print(KeywordExecuterMap* cs) {
    printf("--- KeywordExecuterMap ---\n");
    for(int i = 0;i<cs->size;i++){
        KeywordExecuter* ke = (KeywordExecuter*)Vector_Get(cs,i);
        printf("%d: %d -> %p\n",i,ke->tt,ke->func);
    }
    printf("--------------------------\n");
}



typedef struct RangeChanger {
    TT_Type tt;
    int rangechange;
} RangeChanger;

RangeChanger RangeChanger_New(TT_Type tt,int rangechange) {
    RangeChanger cp;
    cp.tt = tt;
    cp.rangechange = rangechange;
    return cp;
}
RangeChanger RangeChanger_Null() {
    RangeChanger cp;
    cp.tt = TOKEN_NONE;
    cp.rangechange = 0;
    return cp;
}
#define RANGECHANGER_END RangeChanger_Null()

typedef Vector RangeChangerMap;

RangeChangerMap RangeChangerMap_New() {
    RangeChangerMap cs = Vector_New(sizeof(RangeChanger));
    return cs;
}
RangeChangerMap RangeChangerMap_Make(RangeChanger* initlist) {
    RangeChangerMap cs = Vector_New(sizeof(RangeChanger));

    for(int i = 0;initlist[i].tt!=TOKEN_NONE;i++){
        Vector_Push(&cs,&initlist[i]);
    }

    return cs;
}
TT_Iter RangeChangerMap_Find(RangeChangerMap* cs,TT_Type tt) {
    for(int i = cs->size-1;i>=0;i--){
        RangeChanger* ke = (RangeChanger*)Vector_Get(cs,i);
        if(ke->tt==tt) return i;
    }
    return -1;
}
RangeChanger* RangeChangerMap_FindG(RangeChangerMap* cs,TT_Type tt) {
    for(int i = cs->size-1;i>=0;i--){
        RangeChanger* ke = (RangeChanger*)Vector_Get(cs,i);
        if(ke->tt==tt) return ke;
    }
    return NULL;
}
void RangeChangerMap_Free(RangeChangerMap* cs) {
    Vector_Free(cs);
}
void RangeChangerMap_Print(RangeChangerMap* cs) {
    printf("--- RangeChangerMap ---\n");
    for(int i = 0;i<cs->size;i++){
        RangeChanger* ke = (RangeChanger*)Vector_Get(cs,i);
        printf("%d: %d -> %d\n",i,ke->tt,ke->tt);
    }
    printf("--------------------------\n");
}

#endif