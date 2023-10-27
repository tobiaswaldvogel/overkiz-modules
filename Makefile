CFLAGS += \
    -g \
    -std=gnu99 \
    -fPIC -fpic \
    -fno-exceptions \
    -ffunction-sections \
    -fdata-sections \
    -Wall -Werror \
    -Wl,--gc-sections \

OVERKIZUTIL_DEPS = -ldbus-1

all: overkiz-modules

.PHONY: all clean

overkiz-modules:
	$(CC) $(CFLAGS) overkiz-modules.c $(OVERKIZUTIL_DEPS) -o $@ $^

clean:
	-rm -f overkiz-modules

