# copyright (C) 2014-2014 Julian Wergieluk  <julian@wergieluk.com> 

env=Environment()
env.MergeFlags("-O2 -Wall -std=c++0x")
env.MergeFlags("-Wno-sign-compare")

#env.Append( LIBS=[ "pthread"] )
env.Program("timerl", [ Glob("src/*.cpp") ] )


