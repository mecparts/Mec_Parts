.SUFFIXES: .cpp .o .h

CC = g++

DEPS = 
SRCS = main.cpp mainWindow.cpp config.cpp selectPartsDialog.cpp selectSetDialog.cpp newPartDialog.cpp newSetDialog.cpp newPricelistDialog.cpp newCurrencyDialog.cpp sql.cpp csvReader.cpp importResultDialog.cpp partPrice.cpp
OBJS = $(SRCS:.cpp=.o)
EXE = mecparts

CFLAGS = -c -O2 -Wall $(shell pkg-config --cflags gtkmm-3.0) $(shell pkg-config --cflags sqlite3) $(shell pkg-config --cflags libxml-2.0)
LFLAGS = -O2 -L/usr/lib $(shell pkg-config --libs gtkmm-3.0) $(shell pkg-config --libs sqlite3) $(shell pkg-config --libs libxml-2.0)
LIBS = -lboost_regex

all: $(SRCS) $(EXE)

install: mecparts
	/bin/mkdir -p $(HOME)/bin
	/bin/mkdir -p $(HOME)/Desktop
	/bin/mkdir -p $(HOME)/.mecparts/Icons
	/bin/mkdir -p $(HOME)/.mecparts/Database
	/bin/mkdir -p $(HOME)/.mecparts/Pictures
	/bin/echo "" > $(HOME)/Desktop/mecparts.desktop
	/bin/echo "[Desktop Entry]" >> $(HOME)/Desktop/mecparts.desktop
	/bin/echo "Version=1.0" >> $(HOME)/Desktop/mecparts.desktop
	/bin/echo "Type=Application" >> $(HOME)/Desktop/mecparts.desktop
	/bin/echo "Terminal=false" >> $(HOME)/Desktop/mecparts.desktop
	/bin/echo "Path=$(HOME)/.mecparts" >> $(HOME)/Desktop/mecparts.desktop
	/bin/echo "Icon=$(HOME)/.mecparts/Icons/mecparts.png" >> $(HOME)/Desktop/mecparts.desktop
	/bin/echo "Exec=$(HOME)/bin/mecparts" >> $(HOME)/Desktop/mecparts.desktop
	/bin/echo "Name=MecParts" >> $(HOME)/Desktop/mecparts.desktop
	/bin/echo "Comment=An inventory system for Meccano parts" >> $(HOME)/Desktop/mecparts.desktop
	/bin/echo "StartupNotify=true" >> $(HOME)/Desktop/mecparts.desktop
	/bin/chmod 755 $(HOME)/Desktop/mecparts.desktop
	/bin/cp -a mecparts $(HOME)/bin/mecparts
	/bin/cp -a mecparts.ui $(HOME)/.mecparts/mecparts.ui
	/bin/cp -a Icons/mecparts.png $(HOME)/.mecparts/Icons/mecparts.png
	/bin/cp -an meccano.base.db $(HOME)/.mecparts/Database/meccano.db
	/bin/cp -an Pictures/* $(HOME)/.mecparts/Pictures

#dependencies of object files on header files
config.o: config.h mecparts.h
main.o: mainWindow.h mecparts.h collectionStore.h partsStore.h setsStore.h toMakeStore.h config.h selectPartsDialog.h selectSetDialog.h newPartDialog.h newPricelistDialog.h newSetDialog.h sql.h importResultDialog.h newCurrencyDialog.h partPrice.h
mainWindow.o: mainWindow.h mecparts.h collectionStore.h partsStore.h setsStore.h toMakeStore.h pricelistsStore.h config.h selectPartsDialog.h selectSetDialog.h newPartDialog.h newSetDialog.h sql.h currenciesStore.h newCurrencyDialog.h csvReader.h
newCurrencyDialog.o: newCurrencyDialog.h currenciesStore.h mecparts.h
newPartDialog.o: newPartDialog.h mecparts.h
newSetDialog.o: newSetDialog.h mecparts.h
newPricelistDialog.o: newPricelistDialog.h currenciesStore.h mecparts.h
selectPartsDialog.o: selectPartsDialog.h mecparts.h config.h
selectSetDialog.o: selectSetDialog.h mecparts.h config.h
sql.o: sql.h mainWindow.h
csvReader.o: csvReader.h
importResultDialog.o: importResultDialog.h mecparts.h
partPrice.o: mecparts.h partPrice.h

$(EXE): $(OBJS)
	$(CC) $(OBJS) $(LFLAGS) $(LIBS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

# phony target clean: use 'make clean' to delete intermediates
.PHONY: clean
clean:
	rm -f *.o *~ core $(EXE)

