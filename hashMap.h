# ifndef HashMap
# define HashMap

# define KeyType char *
# define ValueType void*

typedef void (*keyPrinter)(KeyType key);
typedef void (*valPrinter)(ValueType val);

/* load factor threshold for resizing the hash table */
# define LOAD_FACTOR_THRESHOLD 0.75

int stringHash2(char * str);

struct hashMap *createMap(int tableSize);

void deleteMap(struct hashMap *ht);

void insertMap (struct hashMap * ht, KeyType k, ValueType v);

ValueType atMap (struct hashMap * ht, KeyType k);

int containsKey (struct hashMap * ht, KeyType k);

void removeKey (struct hashMap * ht, KeyType k);

int size (struct hashMap *ht);

int capacity(struct hashMap *ht);

int emptyBuckets(struct hashMap *ht);

float tableLoad(struct hashMap *ht);

void printMap (struct hashMap * ht, keyPrinter kp, valPrinter vp);
void printKeyValues(struct hashMap *ht, keyPrinter kp, valPrinter vp);

/* Iterator Interface */
struct mapItr *createMapIterator(struct hashMap *ht);
void initMapIterator(struct hashMap *ht, struct mapItr *itr);
int hasNextMap(struct mapItr *itr);
KeyType nextMap(struct mapItr *itr);

# endif
