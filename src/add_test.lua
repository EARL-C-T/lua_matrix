#! /usr/bin/lua5.5
local mtrx=require('lua_matrix')

local m=mtrx.mk_full(2,2,{ 1,2,3,6 })
local n=mtrx.mk_full(2,2,{ 4,5,6,7 })
m:show()
n:show()
local k=m:add(n)
k:show()
