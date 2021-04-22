# Edit following two lines to set component requirements (see docs)
set(COMPONENT_REQUIRES )
set(COMPONENT_PRIV_REQUIRES )

set(COMPONENT_SRCS 
"main.c"
"system.c"
"serial.c")
set(COMPONENT_ADD_INCLUDEDIRS "./inc")

register_component()
