swig -lua -external-runtime ../src/LuaEngine/swigluarun.h
swig -c++ -lua -o ../src/LuaEngine/Eluna_wrap.cpp Eluna.i