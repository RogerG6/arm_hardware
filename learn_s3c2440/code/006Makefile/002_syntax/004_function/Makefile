A = a b c
B = $(foreach f, $(A), $(f).o)

C = a b c d/
E = $(filter %/, $(C))
F = $(filter-out %/, $(C))

files = $(wildcard *.c)
files1 = a.c b.c c.c d.c e.c f.c
files2 = $(wildcard $(files1))

dep_files = $(patsubst %.c, %.d, $(files1))

all:
	@echo B = $(B)
	@echo E = $(E)
	@echo F = $(F)
	@echo files = $(files)
	@echo files2 = $(files2)
	@echo dep_files = $(dep_files)