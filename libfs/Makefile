
# Variables

CFLAGS  := -Wall -Werror -g
CC-FLAGS1       := gcc $(CFLAGS)

objs    := disk.o fs.o


# Target library

lib := libfs.a

# Make all rule

all: $(lib)

# Dependencies

deps    := $(patsubst %.o,%.d,$(objs))
-include $(deps)
DEPFLAGS = -MMD -MF $(@:%.o=.d)


$(lib): $(objs)
	@echo "CC       $@"
	ar rcs $@ $^

%.o: %.c
	@echo "CC       $@"
	$(CC-FLAGS1) -c -o $@ $< $(DEPFLAGS)

clean:
	@echo "clean..."
	rm -f $(lib) $(objs) $(deps) .d
