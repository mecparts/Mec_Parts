.SUFFIXES: .cpp .o .h

CC = g++

DEPS = 
SRCS = main.cpp mainWindow.cpp config.cpp selectPartsDialog.cpp selectSetDialog.cpp newPartDialog.cpp newSetDialog.cpp sql.cpp csvReader.cpp
OBJS = $(SRCS:.cpp=.o)
EXE = mecparts

CFLAGS = -c -O2 -Wall $(shell pkg-config --cflags gtkmm-2.4) $(shell pkg-config --cflags sqlite3)
LFLAGS = -O2 -L/usr/lib $(shell pkg-config --libs gtkmm-2.4) $(shell pkg-config --libs sqlite3)
LIBS = -lboost_regex

all: $(SRCS) $(EXE)

install: mecparts
	/bin/cp mecparts /home/wayneh/bin/mecdata/mecparts
	/bin/cp mecparts.glade /home/wayneh/bin/mecdata/mecparts.glade
	/bin/cp mecparts.png /home/wayneh/bin/mecdata/mecparts.png
	/bin/cp mp-icon.png /home/wayneh/bin/mecdata/mp-icon.png

#dependencies of object files on header files
config.o: config.h mecparts.h
main.o: mainWindow.h mecparts.h collectionStore.h partsStore.h setsStore.h neededStore.h config.h selectPartsDialog.h selectSetDialog.h newPartDialog.h newSetDialog.h sql.h
mainWindow.o: mainWindow.h mecparts.h collectionStore.h partsStore.h setsStore.h neededStore.h pricelistsStore.h config.h selectPartsDialog.h selectSetDialog.h newPartDialog.h newSetDialog.h sql.h currenciesStore.h
newPartDialog.o: newPartDialog.h mecparts.h
newSetDialog.o: newSetDialog.h mecparts.h
selectPartsDialog.o: selectPartsDialog.h mecparts.h config.h
selectSetDialog.o: selectSetDialog.h mecparts.h config.h
sql.o: sql.h mainWindow.h
csvReader.o: csvReader.h

$(EXE): $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) $(LIBS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

# phoney target clean: use 'make clean' to delete intermediates
.PHONY: clean
clean:
	rm -f *.o *~ core $(EXE)

