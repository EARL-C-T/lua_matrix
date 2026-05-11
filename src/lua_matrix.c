/* lua matrix
 * EARL T ET
 *
 * a lua module for 2d matrix math
 */


/*as you can see this is full of reused code not in functions and is very heavy I am kinda working on a
 *ver-0.2.0 that will hopefully be designed a bit better cus im at over 550 lines and i haven't even done 3x3 *division or anything for larger size div det nothing and nothing on translations matrix exponits or any higher functions */

#include <lua5.5/lauxlib.h>
#include <lua5.5/lua.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

/*using flat matrix with row and collumn count in struct to I hope make this kinda fast*/

typedef struct matrix{
    lua_Integer r_cnt;
    lua_Integer c_cnt;
    lua_Number v[];
}mtrx;



/*the only functions are the opnes to create matrixes the rest are all methods of the matrix that
 * is created just regular make directly below make a matrix of supplid dimensions fills with 0s */

static int lua_matrix_mk(lua_State *L){
    if(2!=lua_gettop(L)){
        lua_pushstring(L,"must supply row count and colum count");/*TODO put all dialog in one place*/
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
   luaL_getmetatable(L,"mtrxmeta" );/*TODO change mtrxmeta to just mtrx of matix */
   lua_setmetatable(L,-2 );
   return 1;
}

/*makes a matrix of supplied dimensions fills it with values of the supplied table top left to bottom right*/

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
    luaL_getmetatable(L,"mtrxmeta" );
    lua_setmetatable(L,-2 );
    for(int i=1;i<=t;i++){
        lua_rawgeti(L,3,i);
        M->v[i]=luaL_checknumber(L,-1 );
       lua_pop(L,1);/* needed i think I'm gonna test few things but i end up with the matrix not being on top
       so i cant return it without returning garbage TODO test moveing matrix to top spot ans just useing a table and then ataching that to the matrix this would be better if the table may be popped off the stack someway */
    }

   return 1;
}

/* makes a matrix of either supplied dimensions or of random dimensions 1-9 x 1-9 you can not enter one
 * dimension and andomize the other may add latter either way fills it with random intager entrys using
 * rand seeded by time in no way secure do not use for any kind of encrytion or anything just saying */

static int lua_matrix_mkrand(lua_State *L){
    lua_Integer r_cnt , c_cnt, t;
    srand(time(0));
    if(!lua_isinteger(L,1 ) && !lua_isinteger(L,2)){
        r_cnt =(lua_Integer)rand()%10;
        c_cnt=(lua_Integer)rand()%10;
        (c_cnt==0)?c_cnt=1:c_cnt;
        (r_cnt==0)?r_cnt=1:r_cnt;
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
        m->v[i]=(lua_Number)rand();
    }
    luaL_getmetatable(L,"mtrxmeta" );
    lua_setmetatable(L,-2 );
    return 1;

}

/*the 3 make matrix functions*/
static const struct luaL_Reg matrix_func[]={
    {"mk",lua_matrix_mk},
    {"mk_full",lua_matrix_mkfull},
    {"mk_rand",lua_matrix_mkrand},
    {NULL,NULL}
};

/*displys matix in rows and collumns */

static int lua_matrix_show(lua_State *L){
    mtrx *m=(mtrx*)luaL_checkudata(L,1 ,"mtrxmeta" );
    lua_Integer t=m->r_cnt*m->c_cnt ;
    int cc=0;
    for (int i=1;i<=t;i++){
        cc++;
        if(cc<m->c_cnt){
        printf(" %f ",m->v[i]);
        }else if (cc==m->c_cnt){
            printf(" %f \n",m->v[i]);
            cc=0;
        }
    }
    return 0;
}

/*this takes a file name as an argument file must already exist will print same way show displays*/

static int lua_matrix_print(lua_State *L){
    mtrx* m=luaL_checkudata(L,1 ,"mtrxmeta" );
    const char *o=luaL_checkstring(L,2 );
    FILE* f=fopen(o,"a" );
    lua_Integer t=m->r_cnt*m->c_cnt ;
    int cc=0;
    for (int i=1;i<=t;i++){
        cc++;
        if(cc<m->c_cnt){
            fprintf(f," %f ",m->v[i]);
        }else if (cc==m->c_cnt){
            fprintf(f," %f \n",m->v[i]);
            cc=0;
        }

    }
    fprintf(f,"\n\n");
    fclose(f);
    return 0;
}

/*this  prints to stdout but adds  square parenthises commas and semicolans to make it easier to send it to matlab or octave */

static int lua_matrix_show_ml(lua_State *L){
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

/*same rules as preivius figer it out*/

static int lua_matrix_print_ml(lua_State *L){
    mtrx* m=luaL_checkudata(L,1 ,"mtrxmeta" );
    const char *o=luaL_checkstring(L,2 );
    FILE* f=fopen(o,"a" );
    lua_Integer t=m->r_cnt*m->c_cnt ;
    fprintf(f,"[ ");
    int cc=0;
    for (int i=1;i<=t;i++){
        cc++;
        if(cc!=m->c_cnt && i!=t){
            fprintf(f," %f ,",m->v[i]);
        }else if(cc==m->c_cnt && i!=t ){
            fprintf(f," %f ;\n",m->v[i]);
            cc=0;
        }else if(cc==m->c_cnt && i==t){
            fprintf(f," %f ]\n",m->v[i]);
        }

    }
    fprintf(f,"\n\n");
    fclose(f);
    return 0;
}

/*sets a single value supplyed matrix row and collumn index and value over wrights values dosn't create
 * a new matrix  */
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

/*show a row supply row index*/

static int lua_matrix_showrow(lua_State *L){
    mtrx *m=(mtrx*)luaL_checkudata(L,1 ,"mtrxmeta" );
    lua_Integer r_idx =luaL_checkinteger(L,2 );
    if(m->r_cnt<r_idx || 0>=r_idx){
        lua_pushstring(L,"supplied index is out of range must be int between 0 and r_cnt\n" );
        lua_error(L);
    }
    int srt, stp;
    srt=(r_idx-1)*m->c_cnt+1;
    stp = srt+m->c_cnt-1;
    for(int i=srt;i<=stp;i++){
        if(i==stp){
            printf(" %f\n",m->v[i]);
        }else{
          printf(" %f ",m->v[i]);
        }
    }
    return 0;
}

/*returns the selected row as a table*/

static int lua_matrix_getrow(lua_State *L){
    mtrx *m=(mtrx*)luaL_checkudata(L,1 ,"mtrxmeta" );
    lua_Integer r_idx =luaL_checkinteger(L,2 );
    if(m->r_cnt<r_idx || 0>=r_idx){
        lua_pushstring(L,"supplied index is out of range must be int between 0 and r_cnt\n" );
        lua_error(L);
    }
    int srt, stp;
    lua_newtable(L);
    srt=(r_idx-1)*m->c_cnt+1;
    stp = srt+m->c_cnt-1;
    for(int i=srt;i<=stp;i++){
        lua_pushnumber(L,m->v[i] );
        lua_rawseti(L,-2,lua_rawlen(L,-2)+1);

    }
   return 1;
}

/*sets an entire row supply index and table must be a value for every collumn*/

static int lua_matrix_setrow(lua_State *L){
    mtrx* m=(mtrx*)luaL_checkudata(L,1 ,"mtrxmeta" );
    lua_Number r_idx=luaL_checkinteger(L,2 );

    if(r_idx>m->r_cnt || 0>r_idx){
        lua_pushstring(L,"row index out of range \n" );
        lua_error(L);
    }
     if (!lua_istable(L, 3)||m->c_cnt!=lua_rawlen(L,3 )){
         lua_pushstring(L,"must supply complete row of values in table\n" );
         lua_error(L);
    }
    lua_Number str=(r_idx-1)*m->c_cnt+1;
    lua_Number stp=str+m->c_cnt-1;
    int j=1;
     for(int i=str;i<=stp;i++){
        lua_rawgeti(L,3 ,j );
        m->v[i]=luaL_checknumber(L,-1 );
        lua_pop(L,1 );
        j++;
    }
return 0;
}

/*see set_row*/

static int lua_matrix_showcoll(lua_State *L){
    mtrx* m=(mtrx*)luaL_checkudata(L,1 ,"mtrxmeta" );
    lua_Integer c_idx=luaL_checkinteger(L,2 );
    if(c_idx>m->c_cnt || 0>=c_idx){
        lua_pushstring(L,"collumn index out of range" );
        lua_error(L);
    }
    int j=c_idx;
    for(int i=1;i<=m->r_cnt;i++){
        printf(" %f ",m->v[j]);
        j+=m->c_cnt;
        (i==m->r_cnt)?printf("\n"):0;
        }
        return 0;

}

/*see get_row*/

static int lua_matrix_getcoll(lua_State *L){
    mtrx* m=(mtrx*)luaL_checkudata(L,1 ,"mtrxmeta" );
    lua_Integer c_idx=luaL_checkinteger(L,2 );
    if(c_idx>m->c_cnt || 0>=c_idx){
        lua_pushstring(L,"collumn index out of range" );
        lua_error(L);
    }
    int j=c_idx;
    lua_newtable(L);
    for(int i=1;i<=m->r_cnt;i++){
        lua_pushnumber(L,m->v[j] );
        lua_rawseti(L,-2,lua_rawlen(L,-2)+1);
        j+=m->c_cnt;
    }
    return 1;
}

/*see set_row*/

static int lua_matrix_setcoll(lua_State *L){
    mtrx* m=(mtrx*)luaL_checkudata(L,1 ,"mtrxmeta" );
    lua_Number c_idx=luaL_checkinteger(L,2 );

    if(c_idx>m->c_cnt || 0>c_idx){
        lua_pushstring(L,"collumn index out of range \n" );
        lua_error(L);
    }
    if (!lua_istable(L, 3)||m->r_cnt!=lua_rawlen(L,3 )){
        lua_pushstring(L,"must supply complete row of values in table\n" );
        lua_error(L);
    }
    int j=c_idx;
    for(int i=1;i<=m->r_cnt;i++){
        lua_rawgeti(L,3 ,i );
        m->v[j]=luaL_checknumber(L,-1 );
        lua_pop(L,1 );
        j+=m->c_cnt;
    }
    return 0;
}
/*gets value at supplied row and colloum indexs*/

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

/*returns the row and column count in that order as 2 so r,c=mtrx:lens() just r will only get you row count*/

static int lua_matrix_lens(lua_State *L){
    mtrx* m=(mtrx*)luaL_checkudata(L,1 ,"mtrxmeta" );
    printf("%lld x %lld\n",m->r_cnt,m->c_cnt);
    lua_pushinteger(L,m->r_cnt );
    lua_pushinteger(L,m->c_cnt );
    return 2;
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

/*same as above*/

static int lua_matrix_scaler_sub(lua_State *L){
    mtrx *m=(mtrx*)luaL_checkudata(L,1 ,"mtrxmeta" );
    lua_Number scl = luaL_checknumber(L,2);
    lua_Integer t = m->r_cnt*m->c_cnt;
    for(int i=1;i<=t;i++){
        m->v[i]=m->v[i]-scl;
    }
    return 0;
}

/*see above*/

static int lua_matrix_scaler_mul(lua_State *L){
    mtrx *m=(mtrx*)luaL_checkudata(L,1 ,"mtrxmeta" );
    lua_Number scl = luaL_checknumber(L,2);
    lua_Integer t = m->r_cnt*m->c_cnt;
    for(int i=1;i<=t;i++){
        m->v[i]=m->v[i]*scl;
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

    }
    luaL_getmetatable(L,"mtrxmeta" );
    lua_setmetatable(L,-2 );
    return 1;
}

/*this subtracts m:sub(n) m-n it makes a new matrix the origonals must be same size TODO add to metatable useing __ entrys so i can get some things to work */

static int lua_matrix_sub(lua_State *L){
    mtrx* m=(mtrx*)luaL_checkudata(L,1 ,"mtrxmeta" );
    mtrx* n=(mtrx*)luaL_checkudata(L,2 ,"mtrxmeta" );
    if(m->r_cnt!=n->r_cnt ||m->c_cnt!=n->c_cnt){
        lua_pushstring(L,"both matrix must be the same size" );
        lua_error(L);
    }
    lua_Integer t = m->r_cnt*m->c_cnt;

    mtrx* M=(mtrx*)lua_newuserdata(L,sizeof(mtrx)+sizeof(lua_Number)*t);
    M->c_cnt=m->c_cnt;
    M->r_cnt=m->r_cnt;
    for(int i=1;i<=t;i++){
        M->v[i]=m->v[i]-n->v[i];

    }
    luaL_getmetatable(L,"mtrxmeta" );
    lua_setmetatable(L,-2 );
    return 1;
}

/*returns a new matrix of the dimensions number of rows as the fist number of collums of the secoend and the
 * number of colluams in the first must be equal to number of rows in the second*/

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

/*returns the determent of a 2x2 matrix */

static int lua_matrix_det_2x2(lua_State *L){
    mtrx *m=(mtrx*)luaL_checkudata(L,1,"mtrxmeta");
    if(m->r_cnt!=2 ||m->c_cnt!=2){
        lua_pushstring(L,"matrix must be 2 by 2\n" );
        lua_error(L);
    }
    lua_Number D=m->v[1]*m->v[4]-m->v[2]*m->v[3];
    lua_pushnumber(L,D );
    return 1;
}

/*I need to learn how to better string lua functions together as it should be MTRX=1/det*mtrx */

static int lua_matrix_invrt_2x2(lua_State *L){
    mtrx *m =(mtrx*)luaL_checkudata(L,1 ,"mtrxmeta" );
    if(m->r_cnt!=2 || m->c_cnt!=2){
        lua_pushstring(L,"matrix must be 2 by 2\n" );
        lua_error(L);
    }
    mtrx *M=(mtrx*)lua_newuserdata(L,sizeof(mtrx)+sizeof(lua_Number)*4 );
    lua_Number v[4]={m->v[4],-m->v[2],-m->v[3],m->v[1]};
    lua_Number d=(m->v[1]*m->v[4]-m->v[2]*m->v[3]);
    if(d==0){
        lua_pushstring(L ,"determent is 0 inverting this table is not possible\n" );
        lua_error(L);
    }
    lua_Number invt=1/d;
    for(int i=1;i<=4;i++){
        M->v[i]= invt*v[i-1];
}
    luaL_getmetatable(L,"mtrxmeta");
    lua_setmetatable(L,-2);
    return 1;
}

/*the first matrix m:div_twoby(n) m willbe divided by n m/n */

static int lua_matrix_div_2x2(lua_State *L ){
    mtrx *m=(mtrx*)luaL_checkudata(L,1 ,"mtrxmeta" );
    mtrx *n=(mtrx*)luaL_checkudata(L,2 , "mtrxmeta");
    if(m->r_cnt!=2 || m->c_cnt!=2 || n->r_cnt!=2 || n->c_cnt!=2){
        lua_pushstring(L,"both matrixes must be 2 by 2\n" );
        lua_error(L);
    }
    mtrx *M=(mtrx*)lua_newuserdata(L,sizeof(mtrx)+sizeof(lua_Number)*4 );
    M->r_cnt=2;
    M->c_cnt=2;
    lua_Number v[4]={m->v[4],-m->v[2],-m->v[3],m->v[1]};
    lua_Number d=(m->v[1]*m->v[4]-m->v[2]*m->v[3]);
    if(d==0){
        lua_pushstring(L ,"determent is 0 dividing by this table is not possible\n" );
        lua_error(L);
    }
    lua_Number invt=1/d;
    lua_Number inn[4];
    for(int i=1;i<=4;i++){
        inn[i]= d*v[i];
    }
    for(int i=1;i<2;i++){
        for(int j=1;j<2;j++){
            M->v[(i-1)*2+j]=0;
            for(int k=0;k<2;k++){
                M->v[(i-1)*2+j]= M->v[(i-1)*2+j]+m->v[(i-1)*2+k]*inn[(k-1)*2+j-1] ;
            }
        }


}
luaL_getmetatable(L,"mtrxmeta" );
lua_setmetatable(L,-2 );
return 1;
}

/*minor matrix this one is kinda useless not internally*/

static int lua_matrix_minor_3X3(lua_State *L){
        mtrx *m=(mtrx*)luaL_checkudata(L,1 ,"mtrxmeta" );
        if(m->r_cnt!=3 || m->c_cnt!=3){
            lua_pushstring(L,"matrix must be 3 by 3\n" );
            lua_error(L);
        }
        lua_Integer r_idx=luaL_checkinteger(L,2 );
        lua_Integer c_idx=luaL_checkinteger(L,3 );
        if(r_idx>3 ||r_idx<0 ||c_idx>3 ||c_idx<0){
            lua_pushstring(L,"both indexs must be in the range 1-3\n" );
            lua_error(L);
        }
        lua_Number mm[4];
        int c=0;
        for(int i=1;i<=3;i++){
            if(i!=r_idx){
              for(int j=1;j<=3;j++){
                  if(j!=c_idx){
                     mm[c]=m->v[(i-1)*3+j];
                     c++;
                }
            }
            }
        }
          lua_Number M=mm[0]*mm[3] -mm[1]*mm[2];
          lua_pushnumber(L,M );
          return 1;


}

/*i need to look into gause expantion or some other method of finding the detrminate for larger
 * matrixes doing the in  thi manner ladium expantion i belive I'm spelling it wrong is a O^n! problem
 * as a 4x4 will need you to get co-facters for 4 3X3 matrixes witch requires find detrminate of 9 2x2
 * matrixes and 5x5 would be 5 4x4 and 12 3x3 etc increasing the time and complexity by ! which is a
 * good way to get burryed ask any chess board covered in more rice then ever grown*/

/*static int lua_matrix_det_3x3(lua_State *L){
    mtrx *m=(mtrx*)luaL_checkudata(L,1 ,"mtrxmeta" );
    if(m->r_cnt!=3 || m->c_cnt!=3){
        lua_pushstring(L,"matrix must be 3 by 3" );
        lua_error(L);
    }
    lua_Number D=m->v[1]*(m->v[5]*m->v[9]-m->v[6]*m->v[7])-m->v[2]*(m->v[4]*m->v[9]-m->v[6]*m->v[7])+m->v[3]*(m->v[4]*m->v[8]-m->v[5]*m->v[7]);
    lua_pushnumber(L,D );
    return 1;
}*/

/* table of atrix methods inside index*/

static const struct luaL_Reg mtrx_meta_methods[]={
    {"show",lua_matrix_show},
    {"print",lua_matrix_print},
    {"show_ml",lua_matrix_show_ml},
    {"print_ml",lua_matrix_print_ml},
    {"lens",lua_matrix_lens},
    {"setval",lua_matrix_setvalue},
    {"getval",lua_matrix_getvalue},
    {"scl_add",lua_matrix_scaler_add},
    {"scl_sub",lua_matrix_scaler_sub},
    {"scl_mul",lua_matrix_scaler_mul},
    {"add",lua_matrix_add},
    {"sub",lua_matrix_sub},
    {"mul",lua_matrix_mult},
    {"getrow",lua_matrix_getrow},
    {"showrow",lua_matrix_showrow},
    {"setrow",lua_matrix_setrow},
    {"showcol",lua_matrix_showcoll},
    {"getcol",lua_matrix_getcoll},
    {"setcol",lua_matrix_setcoll},
    {"det_twoby",lua_matrix_det_2x2},
    {"invt_twoby",lua_matrix_invrt_2x2},
    {"div_twoby",lua_matrix_div_2x2},
    /*{"det_threeby",lua_matrix_det_3x3},*/
    {NULL,NULL}
};

/*function pushes to lua*/

int luaopen_lua_matrix(lua_State *L){

    luaL_newmetatable(L,"mtrxmeta");
    lua_pushvalue(L,-1 );
    lua_setfield(L,-2 ,"__index" );/*TODO add other __entrys like __add and __mult so that m + n would work
    i may just have them have the name __add or watever in the reg*/
    luaL_setfuncs(L,mtrx_meta_methods ,0 );
    luaL_newlib(L,matrix_func );
    return 1;
}


