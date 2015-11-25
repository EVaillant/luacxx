file(
	GLOB_RECURSE
	luacxx_src
	*.cc
	../include/luacxx/*
)

add_library(
	luacxx
	STATIC
	${luacxx_src}
)
