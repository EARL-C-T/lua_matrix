/* lua matrix
 * EARL T ET
 *
 * a lua module for 2d matrix math
 */



#include <lua5.5/lauxlib.h>
#include <lua5.5/lua.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct matrix{
    lua_Integer r_cnt;
    lua_Integer c_cnt;
    lua_Number v[];
}mtrx;




static int lua_matrix_mk(lua_State *L){
    if(2!=lua_gettop(L)){
        lua_pushstring(L,"must supply row count and colum count");
        lua_error(L);
    }
    lua_Integer r_cnt = luaL_checkinteger(L,1);
    lua_Integer c_cnt = luaL_checkinteger(L,2);
    lua_Integer t=r_cnt*c_cnt;

    mtrx* M=(mtrx*)lua_newuserdata(L,sizeof(mtrx)+sizeof(lua_Number)*t);
    M->c_cnt=c_cnt;
    M->r_cnt=r_cnt;
    for(int i=1;i<=t;i++){
        M->v[i]=0.0;
    }
   luaL_getmetatable(L,"mtrxmeta" );
   lua_setmetatable(L,-2 );
   return 1;
}

static int lua_matrix_mkfull(lua_State *L){
    if(3!=lua_gettop(L)){
        lua_pushstring(L,"must supply row count and colum count");
        lua_error(L);
    }
    lua_Integer r_cnt = luaL_checkinteger(L,1);
    lua_Integer c_cnt = luaL_checkinteger(L,2);
    lua_Integer t=r_cnt*c_cnt;
    if (!lua_istable(L, 3)||t!=lua_rawlen(L,3 )){
        lua_pushstring(L,"third arg must be a table with all values supplied");
        lua_error(L);
    }
    mtrx *M =(mtrx*)lua_newuserdata(L,sizeof(mtrx)+sizeof(lua_Number)*t);

    M->c_cnt=c_cnt;
    M->r_cnt=r_cnt;
    for(int i=1;i<=t;i++){
        lua_rawgeti(L,3,i);

        M->v[i]=luaL_checknumber(L,-1 );
        printf("%f\n",M->v[i]);
       lua_pop(L,1);
    }
    luaL_getmetatable(L,"mtrxmeta" );
    lua_setmetatable(L,-2 );
   return 1;
}

static int lua_matrix_mkrand(lua_State *L){
    lua_Integer r_cnt , c_cnt, t;
    if(!lua_isinteger(L,1 ) || !lua_isinteger(L,2)){
        r_cnt =(lua_Integer)100/rand();
        c_cnt=(lua_Integer)100/rand();
        printf("dimensions where not supplyed so the will be randomized \n %lld  X  %lld \n",r_cnt,c_cnt);
    }else{
        r_cnt=luaL_checkinteger(L,1 );
        c_cnt=luaL_checkinteger(L,2 );
    }
    t=r_cnt*c_cnt;


    mtrx *m=(mtrx*)lua_newuserdata(L,sizeof(mtrx)+sizeof(lua_Number)*t );
    m->r_cnt=r_cnt;
    m->c_cnt=c_cnt;
    for(int i=1;i<=t;i++){
        m->v[i]=1.0/rand();
    }
    luaL_getmetatable(L,"mtrxmeta" );
    lua_setmetatable(L,-2 );
    return 1;

}
static const struct luaL_Reg matrix_func[]={
    {"mk",lua_matrix_mk},
    {"mk_full",lua_matrix_mkfull},
    {"mk_rand",lua_matrix_mkrand},
    {NULL,NULL}
};

static int lua_matrix_show(lua_State *L){
    mtrx *m=(mtrx*)luaL_checkudata(L,1 ,"mtrxmeta" );
    lua_Integer t=m->r_cnt*m->c_cnt ;
    int cc=0;
    for (int i=1;i<=t;i++){
        cc++;
        printf(" %f ",m->v[i]);
        if (cc==m->c_cnt){
            printf("\n");
            cc=0;
        }
    }
    return 0;
}
static int lua_matrix_show_octave(lua_State *L){
    mtrx *m=(mtrx*)luaL_checkudata(L,1 ,"mtrxmeta" );
    lua_Integer t=m->r_cnt*m->c_cnt ;
    int cc=0;
    printf("[ ");
    for (int i=1;i<=t;i++){
        cc++;
        if(cc!=m->c_cnt && i!=t){
        printf(" %f ,",m->v[i]);
        }else if(cc==m->c_cnt && i!=t ){
            printf(" %f ;\n",m->v[i]);
            cc=0;
        }else if(cc==m->c_cnt && i==t){
            printf(" %f ]\n",m->v[i]);
        }
    }
        return 0;
}

static int lua_matrix_setvalue(lua_State *L){
    mtrx *m=(mtrx*)luaL_checkudata(L,1 ,"mtrxmeta" );
    lua_Integer r_id =luaL_checkinteger(L,2 );
    lua_Integer c_id= luaL_checkinteger(L,3 );
    lua_Number val = luaL_checknumber(L,4 );
    if(r_id>m->r_cnt || c_id>m->c_cnt || c_id<0 || r_id<0){
        lua_pushstring(L,"one or both of the supplyed index(s) are invalid\n" );
        lua_error(L);
    }

    m->v[(r_id-1)*m->c_cnt+c_id]=val;
    return 0;
}

static int lua_matrix_getvalue(lua_State *L){
    mtrx *m=(mtrx*)luaL_checkudata(L,1,"mtrxmeta");
    lua_Integer r_id = luaL_checkinteger(L,2);
    lua_Integer c_id =luaL_checkinteger(L,3);
    if(r_id>m->r_cnt || c_id>m->c_cnt || c_id<0 || r_id<0){
        lua_pushstring(L,"one or both of the supplyed index(s) are invalid\n" );
        lua_error(L);
    }
    lua_pushnumber(L, m->v[(r_id-1)*m->c_cnt+c_id]);
    return 1;
}

static int lua_matrix_lens(lua_State *L){
    mtrx* m=(mtrx*)luaL_checkudata(L,1 ,"mtrxmeta" );
    printf("%lld x %lld\n",m->r_cnt,m->c_cnt);
    return 0;
}

/* this adds a number to the matrix  the result is the supplied matrix updated m:scaler_add( scaler) */
static int lua_matrix_scaler_add(lua_State *L){
    mtrx *m=(mtrx*)luaL_checkudata(L,1 ,"mtrxmeta" );
    lua_Number scl = luaL_checknumber(L,2);
    lua_Integer t = m->r_cnt*m->c_cnt;
    for(int i=1;i<=t;i++){
        m->v[i]=m->v[i]+scl;
    }
    return 0;
}
/*this adds 2 matrixes returns a third dose not destroy the other 2 they must be the same size m:add(matix)*/
static int lua_matrix_add(lua_State *L){
    mtrx *m=(mtrx*)luaL_checkudata(L,1 ,"mtrxmeta" );
    mtrx *n=(mtrx*)luaL_checkudata(L,2 ,"mtrxmeta" );
    if(m->r_cnt!=n->r_cnt || m->c_cnt!=n->c_cnt){
        lua_pushstring(L,"both matrix must be the same size" );
        lua_error(L);
    }
    lua_Integer t = m->r_cnt*m->c_cnt;

    mtrx* M=(mtrx*)lua_newuserdata(L,sizeof(mtrx)+sizeof(lua_Number)*t);
    M->c_cnt=m->c_cnt;
    M->r_cnt=m->r_cnt;
    for(int i=1;i<=t;i++){
        M->v[i]=m->v[i]+n->v[i];
        printf("%d %f + %f = %f \n",i,m->v[i],n->v[i],M->v[i]);

    }
    luaL_getmetatable(L,"mtrxmeta" );
    lua_setmetatable(L,-2 );
    return 1;
}
/*returns a new matrix of the dimensions number of rows as the fist number of collums of the secoend and the number of colluams
 *in the first must be equal to number of rows in the second*/
static int lua_matrix_mult(lua_State *L){
    mtrx *m=(mtrx*)luaL_checkudata(L,1 ,"mtrxmeta" );
    mtrx *n=(mtrx*)luaL_checkudata(L,2 ,"mtrxmeta" );
    if(m->c_cnt!=n->r_cnt){
        lua_pushstring(L,"the first matrixs number of collums must match the seconds number of rows\n" );
        lua_error(L);
    }
    lua_Integer t=m->r_cnt*n->c_cnt;
    mtrx *M=(mtrx*)lua_newuserdata(L,sizeof(mtrx)+sizeof(lua_Number)*t );
    M->r_cnt=m->r_cnt;
    M->c_cnt=n->c_cnt;
    for(int i=1;i<=M->r_cnt;i++){
        for(int j=1;j<=M->c_cnt;j++){
            M->v[(i-1)*M->c_cnt+j]=0;
            for(int k=1;k<=n->r_cnt;k++){
                M->v[(i-1)*M->c_cnt+j]= M->v[(i-1)*M->c_cnt+j]+m->v[(i-1)*m->c_cnt+k]*n->v[(k-1)*n->c_cnt+j] ;
            }
        }
    }
    luaL_getmetatable(L,"mtrxmeta" );
    lua_setmetatable(L,-2 );
    return 1;

}

static const struct luaL_Reg mtrx_meta_methods[]={
    {"show",lua_matrix_show},
    {"lens",lua_matrix_lens},
    {"setval",lua_matrix_setvalue},
    {"getval",lua_matrix_getvalue},
    {"scl_add",lua_matrix_scaler_add},
    {"add",lua_matrix_add},
    {"mul",lua_matrix_mult},
    {NULL,NULL}
};

int luaopen_lua_matrix(lua_State *L){

    luaL_newmetatable(L,"mtrxmeta");
    lua_pushvalue(L,-1 );
    lua_setfield(L,-2 ,"__index" );
    luaL_setfuncs(L,mtrx_meta_methods ,0 );
    luaL_newlib(L,matrix_func );
    return 1;
}


