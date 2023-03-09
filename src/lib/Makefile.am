noinst_LIBRARIES = libmongoose.a libfrozen.a libamipack.a libamiws.a libamiwscfg.a

libamiwscfg_a_SOURCES = config.c config_parser.c config_parser.h amiws.h
libamiws_a_SOURCES   = amiws_lib.c amiws.h linked_str_stack.c linked_str_stack.h
#libmongoose_a_CPPFLAGS = -DMG_ENABLE_DEBUG
libmongoose_a_SOURCES= mongoose.c mongoose.h
libfrozen_a_SOURCES  = frozen.c frozen.h
libamipack_a_SOURCES = amipack.c amipack.h      \
                       amipack_parser_detect.c  \
                       amipack_parser_prompt.c  \
                       amipack_parser_message.c \
                       amipack_parser_command.c \
                       amipack_parser_queue.c

