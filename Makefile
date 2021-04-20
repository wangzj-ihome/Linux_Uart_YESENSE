EXEC=test

all:
	gcc -o $(EXEC) main.c analysis_data.c analysis_data.h

clean:
	rm -rf $(EXEC)
