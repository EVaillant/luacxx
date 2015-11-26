file(
	GLOB_RECURSE
	luacxx_src
	*.cc
	../include/luacxx/*
)

add_library(
	luacxx_static
	STATIC
	${luacxx_src}
)

add_library(
	luacxx
	SHARED
	${luacxx_src}
)

install (TARGETS luacxx
         ARCHIVE DESTINATION lib
         LIBRARY DESTINATION lib
         RUNTIME DESTINATION bin)

install (TARGETS luacxx_static
         ARCHIVE DESTINATION lib
         LIBRARY DESTINATION lib
         RUNTIME DESTINATION bin)
