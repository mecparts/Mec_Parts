# MecParts

## A Meccano Parts inventory database application

I wouldn't be surprised if I'm one of a *very* few people who's into both
Meccano *and* Linux. I wouldn't even be all that shocked to find out that I'm
the *only* one! But on the off chance that I'm wrong, I'm putting this code out
there.

Yes, it would have probably made more sense to do something like this in
Windows. Certainly it would have found a wider audience. But as I don't have a
single computer running Windows in my house, Linux it was. Developing this was
as much to get some more real world practice developing graphical apps in Linux
as it was to get a database app I could use. Well, maybe not quite that much...
I *really* did need something to keep track of my Meccano! But you get the idea.

Could this be ported to Windows? Yes. The most direct route is to build it
under Cygwin. But the gtkmm libraries aren't a standard part of Cygwin so
you'll have to go hunting for them, and you'll have to edit the Makefile and
change **LIBS = -lboost_regex** to **LIBS = -lboost_regex-mt**. Start at
[http://sourceware.org/cygwinports/](http://sourceware.org/cygwinports/) and 
good luck! It does *sort of* work; I kept getting Gtk errors every time I tried
reading any image file until I installed eog (Eye of Gnome). It's also 
dreadfully slow. I think rewriting the app as a native C# application would be
a better idea.

### How to install MecParts

I didn't even consider packaging this up into a .deb package (and as it turns
out the Debian package manager isn't really intended for installing things
into a user's home directory).  I did eventually put the source into a 
repository at github. Assuming you already have git installed on your system,
you can grab a copy of the repository with

    git clone git://github.com/mecparts/MecParts.git

That'll create a MecParts directory in your current directory and fill it with
the MecParts source code. Install the development libraries, cd into MecParts,
do a *make install* and you're done.

It sounds so easy if you say it quick, doesn't it?

On a freshly installed Ubuntu 12.04 system, I had to install the following four
development libraries:

    sudo apt-get update
    sudo apt-get install libgtkmm-3.0-dev libboost-regex-dev libsqlite3-dev libxml2-dev

They installed a whole bunch of other dependencies along the way, including the
compiler. Once they are all installed, a simple *make install* in the MecParts
directory does the rest. You get an icon on the desktop, a sample database
with dozens of sets and hundreds of parts and even a smattering of part
pictures. You can add your own part pictures into the ~/.mecparts/Pictures sub 
directory.

And why didn't I include a full set of part pictures? The main reason is size.
My Pictures sub directory is currently 166Mb. That's about 1,000 times as large
as the zip'd source code. The other reason is that a lot of the pictures I use
came from various sources on the web and they aren't mine to distribute. It
wouldn't be polite. Maybe one day I'll have nothing else to do but go through
my collection and photograph one of each of my parts but until then you'll have
to dig up your own images.

## The bits and pieces of MecPart's database

**PART**: exactly what you think it is, a description of a single Meccano part. A
part has:

 * a unique part number, which is composed of three parts:
   * an optional multi character alphabetic prefix (the f in f73a)
   * a required multi digit numeric middle (the 73 in f73a)
   * an optional multi character alphanumeric suffix (the a in f73a)
 * a description,
 * a size,
 * notes,
 * an optional picture,
 * and an optional price in each pricelist.

The picture, if there is one, goes into the .mecdata/Pictures sub directory. It
can be a JPEG, GIF, PNG, BMP or TIFF image. Name them the same as the part
number. You can use JPEGs, GIFs, PNGs, BMPs or even TIFFs. The program is going
to scale them to fit in a 300x300 box when it displays them so large images
will only slow things down.

**SET**: the name of a collection of parts. A set can be an actual Meccano set,
the parts list of a model, the contents of an order from a parts supplier, or
any other collection you can think of. A set has:

 * a unique set number, usually the model number if it's an actual Meccano set.
 * a description,
 * a starting year (used for sorting the list of sets),
 * and an ending year.

**COLLECTION**: the collection of parts contained in a set. Each part in a
collection has a count, namely the number of this part found in this set.

**PRICELIST**: A pricelist is a collection of part prices in a particular currency.
It's meant to be used as a vendor's pricelist though there are doubtless other
uses as well. Each pricelist has:

 * a name,
 * and a currency (the currency that the prices in this list are in).

**PRICE**: each part can have price in each pricelist. If a part isn't available
from a particular supplier, it won't have a price in their pricelist. Prices
are stored in the native currency of the pricelist it belongs to. When prices
are displayed, they are converted on the fly to whatever currency you've chosen
to see things in. There is one exception to this rule: when you're creating a
new part, if you enter a price it that price is in the currency of the current
pricelist. I was forever entering prices and realizing afterwards that though
the pricelist was in pounds, I had the currency set to dollars.

**CURRENCY**: A currency is the rate of exchange of a particular monetary unit
against the euro. It is made up of:

 * a name,
 * a three letter ISO currency code,
 * the exchange rate against the euro.
 
You can manually change the exchange rate, but it will be overwritten if you
click on 'Update Currencies'. There is a way around this though. Create a new
currency and use a three letter currency code that isn't in use. You can see
the rate on this and it will be left alone since it isn't a recognized currency
code.

### Getting up and running


In general:

* Right click on a list and a pop-up menu of the actions you can perform appears.
* Columns with a white background are editable. Columns with a grey background
  aren't.
* Prices are shown in your selected currency, based on the selected pricelist.
  If a pricelist is in GBP and you've select CDN as your currency, the prices
  will be in CDN.
* When you're adding a new part, you can add a price at the same time. For
  convenience's sake, this price is in the currency of the current pricelist,
  *NOT* your current currency!
* Pictures can be added to the Pictures sub directory. Name them with the part
  name. When displayed they'll be scaled to fit in a 300 by 300 box.
* Each pricelist has an associated currency. Currency exchange rates are
  against the euro. Rated can be manually adjusted as you like, or you can
  right click on the currencies list and automatically adjust them to the
  latest rates from the European Central Bank.
* The format of the csv file used to import prices is very simple. Each line
  consists of a part number, a comma, and a price (in the currency of the
  pricelist you're importing into). No column headers, nothing fancy.

I've included a sample database with the program. There's 76 sets, 1170 parts,
3 pricelists and 8 currencies included to get you going. The first set in the
list is called 'My Collection' and it's exactly what it says it is. Right click
on the empty Collection tab, select 'Add Part' and start adding your parts. You
can add multiple parts at a time by Ctrl-clicking on the parts in the list.
Once you've added some parts, you can enter the quantities of each part that
you have. Your part count and total cost (based on the current pricelist) will
be shown.

You can also add all the parts in an existing set. If for example you have a
1953 10 set (lucky you!), you can select 'Add Set' and add the entire contents
of the set all at once.

The 'Collection' tab is probably where you'll spend most of your time. A close
second may be the 'To Make' tab. This tab displays the difference between two
sets. What use is that? Imagine you 'have' your collection, and 'want' to build
a particular model. Do you have all the parts you'll need? If you create a set
for the model and add the parts in its parts list to that set, you can easily
find out. And it'll even tell you how much it'll cost to buy those parts you
don't have.

Enjoy!

