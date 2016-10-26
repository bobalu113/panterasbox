
#ifndef STUFF_INC
#define STUFF_INC

#define StuffBaseDir       "/zone/null/stuff/"
#define StuffCoreDir       StuffBaseDir "base/"
#define StuffLogDir        StuffBaseDir "logs/"
#define StuffSupportDir    StuffBaseDir "support/"
#define StuffServer        StuffCoreDir "server.c"
#define StuffCode          StuffCoreDir "stuff.c"
#define PotionStuff        StuffCoreDir "potion.c"
#define ScrollStuff        StuffCoreDir "scroll.c"
#define AmuletStuff        StuffCoreDir "amulet.c"
#define AmuletStuffShadow  StuffCoreDir "amulet_shadow.c"
#define RingStuff          StuffCoreDir "ring.c"
#define RingStuffShadow    StuffCoreDir "ring_shadow.c"
#define WandStuff          StuffCoreDir "wand.c"
#define ConversionStuff    StuffCoreDir "conversion.c"
#define _CS                StuffCoreDir "_CS.c"
#define ForceObject        StuffCoreDir "force.c"
#define ArtifactStuff      StuffCoreDir "artifact.c"
#define AltarStuff         StuffCoreDir "altar.c"
#define TrapStuff          StuffCoreDir "trap.c"

#define StuffP             "stuff_PROP"
#define NoStuffP           "no_stuff_PROP"
#define ID_ITEM            0x01
#define ID_STATUS          0x02
#define ID_CHARGES         0x04
#define ID_ALL             ( ID_ITEM | ID_STATUS | ID_CHARGES )

#define LOAD_DIRS          ({ StuffBaseDir "potions/", \
                              StuffBaseDir "scrolls/", \
                              StuffBaseDir "amulets/", \
                              StuffBaseDir "rings/",   \
                              StuffBaseDir "wands/",   \
                              StuffBaseDir "artifacts/", \
                              StuffBaseDir "traps/",   \
                              StuffBaseDir "altars/",  \
                           })

// Sorry about the bad formatting but there's a limit on how
// many characters can go into a #define.
#define LABELS ([ \
PotionP : ({ "gray", "violet", "blue", "brown", "green", "black", "red", "cerulean", "white", "pink", "yellow", "orange", "fuchsia", "dark", "milky", "goldenrod", "magenta", "maroon", "purple", "periwinkle", "mauve", "sepia", "tan", "teal", "puce", "cloudy", "smoky", "fizzy", "murky" }), \
ScrollP : ({ "OGIMA", "RETSEJ", "LE ARZA", "NOTPMOC", "NAM WOB", "NEW ODAHS", "GODKCIT", "ON RETS", "SUGAH", "DRAK NURD", "NIES SENIUQ", "GUBEL DOOD", "TRAHD LIW", "HAISSEM", "PILSE MIT", "ONIMAHS", "ESAH C", "Y DEEPS", "SEW LE", "ELIN", "GALS", "ACCAB WEHC", "NILAC", "NOITU LOVE", "SF FUP", "H CROCS", "PSI W" }), \
AmuletP : ({ "square", "triangular", "hexagonal", "oval", "circular", "octagonal", "flat", "skinny", "spiral", "jagged", "bumpy", "fuzzy", "webbed", "spikey", "beveled", "shiny", "squishy", "conical", "feathered", "mirrored", "rectangular", "heart-shaped", "hollow", "concave", "crooked" }), \
RingP   : ({ "garnet", "diamond", "emerald", "sapphire", "opal", "ruby", "topaz", "amethyst", "aquamarine", "opal", "peridot", "turquoise", "onyx", "jade", "agate", "jasper", "quartz", "amber", "coral", "pearl", "cubic zirconia", "citrine", "obsidian", "tiger eye", "moonstone", "bloodstone" }), \
WandP   : ({ "cork", "wooden", "leather", "bone", "ivory", "crystal", "glass", "ceramic", "clay", "stone", "concrete", "magnetic", "bronze", "iron", "lead", "aluminum", "platinum", "titanium", "steel", "nickel", "zinc", "copper", "silver", "golden", "jeweled", "plastic", "rubber", "latex" }), \
ArtifactP : ({ "antique", "antiquated", "aged", "ancient", "heirloom", "antediluvian", "archaic", "timeworn", "primeval", "venerable", "age-old", "old-fashioned", "seasoned", "musty", "hoary", "well-worn", "collectible", "novel", "exotic", "well-preserved", "ornamented", "ageless", "primitive" }), \
TrapP   : ({ "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s" }), \
NonStandardP : ({ "adobe", "bamboo", "brick", "earthen", "hemp", "wooden", "thatch", "wattle-and-daub", "stacked stone", "pyramidal", "painted", "lichen-covered", "moldy", "limestone" }), \
])

#define ForceID(arg)  \
{ \
  object _force, _oldp; \
  _oldp = this_player(); \
  _force = clone_object( ForceObject ); \
  _force->force_id( ID_ITEM|ID_STATUS ); \
  set_this_player( _force ); \
  arg \
  set_this_player( _oldp ); \
  destruct( _force ); \
}

#define ForceUnID(arg)  \
{ \
  object _force, _oldp; \
  _oldp = this_player(); \
  _force = clone_object( ForceObject ); \
  _force->force_id( 0 ); \
  set_this_player( _force ); \
  arg \
  set_this_player( _oldp ); \
  destruct( _force ); \
}

#endif
