# 315CAb_StanMihai-Catalin
# compiler setup
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -lm

#define targets
TARGETS=image_editor

build: $(TARGETS)

image_editor: image_editor.c
	$(CC) image_editor.c $(CFLAGS) -o image_editor

pack:
	zip -FSr 315CA_StanMihaiCatalin_Tema3.zip README Makefile *.c *.h
    
clean:
	rm -f $(TARGETS)

.PHONY: pack clean
