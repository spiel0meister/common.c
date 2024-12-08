HEADERS = src/arena.h src/cperf.h \
			src/dah.h src/easings.h src/flag.h \
			src/linear.h src/log.h src/process.h src/string_builder.h \
			src/string_view.h src/tsprintf.h src/types.h src/utils.h

dummy: dummy.c
	$(CC) -Wall -Wextra -ggdb -o $@ $<

common.h: $(HEADERS)
	cat $(HEADERS) > common.h
