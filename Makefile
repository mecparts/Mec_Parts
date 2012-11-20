.SUFFIXES: .cpp .o .h

CC = g++

DEPS = 
SRCS = main.cpp mainWindow.cpp config.cpp selectPartsDialog.cpp selectSetDialog.cpp newPartDialog.cpp newSetDialog.cpp newPricelistDialog.cpp newCurrencyDialog.cpp sql.cpp csvReader.cpp importPricesResultDialog.cpp
OBJS = $(SRCS:.cpp=.o)
EXE = mecparts

CFLAGS = -c -O2 -Wall $(shell pkg-config --cflags gtkmm-3.0) $(shell pkg-config --cflags sqlite3) $(shell pkg-config --cflags libxml-2.0)
LFLAGS = -O2 -L/usr/lib $(shell pkg-config --libs gtkmm-3.0) $(shell pkg-config --libs sqlite3) $(shell pkg-config --libs libxml-2.0)
LIBS = -lboost_regex

all: $(SRCS) $(EXE)

install: mecparts
	/bin/cp mecparts /home/wayneh/bin/mecdata/mecparts
	/bin/cp mecparts.ui /home/wayneh/bin/mecdata/mecparts.ui
	/bin/cp mecparts.png /home/wayneh/bin/mecdata/mecparts.png
	/bin/cp mp-icon.png /home/wayneh/bin/mecdata/mp-icon.png

#dependencies of object files on header files
config.o: config.h mecparts.h
main.o: mainWindow.h mecparts.h collectionStore.h partsStore.h setsStore.h toMakeStore.h config.h selectPartsDialog.h selectSetDialog.h newPartDialog.h newPricelistDialog.h newSetDialog.h sql.h importPricesResultDialog.h newCurrencyDialog.h
mainWindow.o: mainWindow.h mecparts.h collectionStore.h partsStore.h setsStore.h toMakeStore.h pricelistsStore.h config.h selectPartsDialog.h selectSetDialog.h newPartDialog.h newSetDialog.h sql.h currenciesStore.h newCurrencyDialog.h csvReader.h
newPartDialog.o: newPartDialog.h mecparts.h
newSetDialog.o: newSetDialog.h mecparts.h
newPricelistDialog.o: newPricelistDialog.h currenciesStore.h mecparts.h
selectPartsDialog.o: selectPartsDialog.h mecparts.h config.h
selectSetDialog.o: selectSetDialog.h mecparts.h config.h
sql.o: sql.h mainWindow.h
csvReader.o: csvReader.h

$(EXE): $(OBJS)
	$(CC) $(OBJS) $(LFLAGS) $(LIBS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

# phoney target clean: use 'make clean' to delete intermediates
.PHONY: clean
clean:
	rm -f *.o *~ core $(EXE)

