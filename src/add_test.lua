#! /usr/bin/lua5.5
mtrx=require('lua_matrix')

m=mtrx.mk_full(2,2,{1,2,3,4})
n=mtrx.mk_full(2,2,{4,5,6,7})
m:show()
n:show()
k=m:add(n)
k:show()
