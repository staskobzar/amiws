SUBDIRS = src test
CTAGSFLAGS= -R src

.PHONY: parser doc
parser: src/lib/amipack_parse.c src/lib/amipack_parse_prompt.c

src/lib/amipack_parse_prompt.c: src/lib/amipack_parse_prompt.re
	@echo "== Generating AMI prompt parser with re2c"
	re2c --no-generation-date -c -o $@ $^

src/lib/amipack_parse.c: src/lib/amipack_parse.re
	@echo "== Generating AMI parser with re2c"
	re2c --no-generation-date -c -o $@ $^

doc:
	cd doc && doxygen
