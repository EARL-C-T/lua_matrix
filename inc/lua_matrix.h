/*lua matrix
 * EARL T {ET}
 *
 * a basic lib of matrix math for lua capi*/

#include <lua5.5/lua.h>
#include <lua5.5/lauxlib.h>
#include <lua5.5/luaconf.h>
#include <lua5.5/lualib.h>

/*base matrix mtrx type uses flat array of values meaning that its just one array and takes a bit of math *(row_index-1)*colum_count+colum_index to find your posision from row_index, colum_index or i,j notation for speed i hope*/
typedef struct matrix{
    lua_Integer r_cnt;
    lua_Integer c_cnt;
    lua_Number v[];
}mtrx;

/*makes a matrix of supplied row and collum count lua: matrix.mk(row_cnt,clm_cnt)*/

static int lua_matrix_mk(lua_State *L);

/*same as before except uses supplied table of values to populate array insted of 0s*/

static int lua_matrix_mkfull(lua_State *L);

/*****************************these are meta methods for mtrx***************************/

/*shows the matrix pretty print style lua: matrix.show(matrix) or m:show()*/

 static int lua_matrix_show(lua_State *L);

/*sets a single value in supplied matrix with modified value lua: matrix.setval(matrix,r_index,c_index,val)*/

static int lua_matrix_setvalue(lua_State *L);



/*adds supplied number to matrix matrix is modified not retuned lua: matrix.scladd(matrix, scaler)*/

static int lua_matrix_scaler_add(lua_State *L);

/*adds to matrixs together must be the same dim retuns a new matrix non destructive lua: matrix.add(matrixA,matrixB)*/

static int lua_matrix_add(lua_State *L);

/*multiplys to matrix the first col_cnt must equal the second row_cnt will return a matrix with first row_cnt secound col_cnt lua:
 * matrix.mul(matrixA,matrixB)*/

static int lua_matrix_mult(lua_State *L);


int luaopen_lua_matrix(lua_State *L);
