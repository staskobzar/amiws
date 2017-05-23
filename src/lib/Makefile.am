noinst_LIBRARIES = libamiws.a libmongoose.a libfrozen.a libamipack.a

libamiws_a_SOURCES   = amiws_lib.c amiws.h
#libmongoose_a_CPPFLAGS = -DMG_ENABLE_DEBUG
libmongoose_a_SOURCES= mongoose.c mongoose.h
libfrozen_a_SOURCES  = frozen.c frozen.h
libamipack_a_SOURCES = amipack.c amipack_parse_prompt.c amipack_parse.c amipack.h

amipack_parse_prompt.c: amipack_parse_prompt.re
	@echo "== Generating AMI prompt parser with re2c"
	re2c --no-generation-date -c -o $@ $^

amipack_parse.c: amipack_parse.re
	@echo "== Generating AMI parser with re2c"
	re2c --no-generation-date -c -o $@ $^

