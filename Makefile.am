SUBDIRS = src test
CTAGSFLAGS= -R src

.PHONY: parser doc
parser: src/lib/amipack_parser_detect.c   \
        src/lib/amipack_parser_prompt.c   \
        src/lib/amipack_parser_message.c  \
        src/lib/amipack_parser_command.c  \
        src/lib/amipack_parser_queue.c


src/lib/amipack_parser_detect.c: src/lib/amipack_parser_detect.re
	@echo "Generating AMI packet detect parser with re2c..."
	@re2c --no-generation-date -o $@ $^

src/lib/amipack_parser_prompt.c: src/lib/amipack_parser_prompt.re
	@echo "Generating AMI Prompt parser with re2c"
	@re2c --no-generation-date -c -o $@ $^

src/lib/amipack_parser_message.c: src/lib/amipack_parser_message.re
	@echo "Generating AMI Standard message parser with re2c"
	@re2c --no-generation-date --tags -o $@ $^

src/lib/amipack_parser_command.c: src/lib/amipack_parser_command.re
	@echo "Generating AMI Command output parser with re2c"
	@re2c --no-generation-date --tags -o $@ $^

src/lib/amipack_parser_queue.c: src/lib/amipack_parser_queue.re
	@echo "Generating AMI Queue output parser with re2c"
	@re2c --no-generation-date -c --tags -o $@ $^

doc:
	cd doc && doxygen
