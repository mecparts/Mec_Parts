MecParts
--------
A Meccano Parts inventory database application

I wouldn't be surprised if I'm one of a *very* few people who's into both
Meccano *and* Linux. I wouldn't be shocked to find out that I'm the *only* one!
But on the off chance that I'm wrong, I'm putting this code out there.

Yes, it would have probably made more sense to do something like this in
Windows. Certainly it would have found a wider audience. But as I don't have a
single computer running Windows in my house, Linux it was. Developing this was
as much do get some more real world practice developing graphical apps in Linux
as it was to get a database app I could use. Well, maybe not quite that much...
I *really* did need something to keep track of my Meccano! But you get the idea.

Could this be ported to Windows? Yes. The most direct route is to build it
under Cygwin. But the gtkmm libraries aren't a standard part of Cygwin so
you'll have to go hunting for them, and you'll have to edit the Makefile and
change 'LIBS = -lboost_regex' to 'LIBS = -lboost_regex-mt'. Start at
http://sourceware.org/cygwinports/ and good luck! It does 'sort of' work;
there's an issue of something in Cygwin+Gdk not recognizing perfectly valid
image files and crashing badly and I imagine there are other gremlins hiding
in the shadows as well. 

How to install MecParts
-----------------------

I didn't even consider packaging this up into a .deb package. I don't know how
and really wasn't at all interested in figuring it out. I did toy with the idea
of putting the source into a git repository somewhere and although that may be
a really good idea, I got lazy and didn't. If it does turn out that there's a
community of Linux using Meccano-ites out there who want to help expand the
program, it's something that can always be done. So right now if you really
want to try it out, you'll have to build it from source. If you're reading
this, you've probably already extracted the files into a convenient directory
somewhere. Install the development libraries, do a 'make install' and you're
done.

It sounds so easy if you say it quick, doesn't it?

On a freshly installed Ubuntu 12.04 system, I had to install the following four
development libraries:

sudo apt-get install libgtkmm-3.0-dev
sudo apt-get install libboost-regex-dev
sudo apt-get install libsqlite3-dev
sudo apt-get install libxml2-dev

They installed a whole bunch of other dependencies along the way, including the
compiler. Once they are all installed, a simple 'make install' does the rest.
You get an icon on the desktop, a sample database with dozens of sets and
hundreds of parts and even a smattering of part pictures. You can add your own
part pictures into the Pictures subdirectory. Name them the same as the part
number. You can use JPEGs, GIFs, PNGs, BMPs or even TIFFs. The program is going
to scale them to fit in a 300x300 box when it displays them so large images
will only slow things down.

And why didn't I include a full set of part pictures? The main reason is size.
My Pictures subdirectory is currently 166Mb. That's about 1,000 times as large
as the zip'd source code. The other reason is that a lot of the pictures I use
came from various sources on the web and they aren't mine to distribute. It
wouldn't be polite. Maybe one day I'll have nothing else to do but go through
my collection and photograph one of each of my parts but until then you'll have
to dig up your own images.

Getting up and running
----------------------

In general:

* Right click on a list and a popup menu of the actions you can perform appears.
* Columns with a white background are editable. Columns with a grey background
  aren't.
* Prices are shown in your selected currency, based on the selected pricelist.
  If a pricelist is in GBP (Great Britain Pounds) and you've select CDN
  (Canadian dollars) as your currency, the prices will be in CDN.
* When you're adding a new part, you can add a price at the same time. For
  convenience's sake, this price is in the currency of the current pricelist,
  *NOT* your current currency!
* Pictures can be added to the Pictures subdirectory. Name them with the part
  name. When displayed they'll be scaled to fit in a 300 by 300 box.
  
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

Wayne Hortensius
whortens@shaw.ca

November 23, 2012

