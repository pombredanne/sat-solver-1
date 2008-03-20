/*
 * Copyright (c) 2007, Novell Inc.
 *
 * This program is licensed under the BSD license, read LICENSE.BSD
 * for further information
 */

/*
 * pool.h
 * 
 */

#ifndef SATSOLVER_POOL_H
#define SATSOLVER_POOL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

#include "pooltypes.h"
#include "poolid.h"
#include "solvable.h"
#include "bitmap.h"
#include "queue.h"
#include "strpool.h"

/* well known ids */
#include "knownid.h"

/* well known solvable */
#define SYSTEMSOLVABLE		1


/* how many strings to maintain (round robin) */
#define POOL_TMPSPACEBUF 16

//-----------------------------------------------

struct _Repo;
struct _Repodata;
struct _Repokey;
struct _KeyValue;

struct _Pool {
  struct _Stringpool ss;

  Reldep *rels;               // table of rels: Id -> Reldep
  int nrels;                  // number of unique rels
  Hashtable relhashtbl;       // hash table: (name,evr,op ->) Hash -> Id
  Hashmask relhashmask;

  struct _Repo **repos;
  int nrepos;

  Solvable *solvables;
  int nsolvables;

  const char **languages;
  int nlanguages;
  Id *languagecache;
  int languagecacheother;

  int promoteepoch;             /* 0/1  */

  Id *id2arch;			/* map arch ids to scores */
  Id lastarch;			/* last valid entry in id2arch */
  Queue vendormap;		/* map vendor to vendorclasses mask */

  /* providers data, as two-step indirect list
   * whatprovides[Id] -> Offset into whatprovidesdata for name
   * whatprovidesdata[Offset] -> ID_NULL-terminated list of solvables providing Id
   */
  Offset *whatprovides;		/* Offset to providers of a specific name, Id -> Offset  */
  Offset *whatprovides_rel;	/* Offset to providers of a specific relation, Id -> Offset  */

  Id *whatprovidesdata;		/* Ids of solvable providing Id */
  Offset whatprovidesdataoff;	/* next free slot within whatprovidesdata */
  int whatprovidesdataleft;	/* number of 'free slots' within whatprovidesdata */

  /* If nonzero, then consider only the solvables with Ids set in this
     bitmap for solving.  If zero, consider all solvables.  */
  Map *considered;

  Id (*nscallback)(struct _Pool *, void *data, Id name, Id evr);
  void *nscallbackdata;

  /* our tmp space string space */
  char *tmpspacebuf[POOL_TMPSPACEBUF];
  int   tmpspacelen[POOL_TMPSPACEBUF];
  int   tmpspacen;

  /* debug mask and callback */
  int  debugmask;
  void (*debugcallback)(struct _Pool *, void *data, int type, const char *str);
  void *debugcallbackdata;

  /* load callback */
  FILE * (*loadcallback)(struct _Pool *, struct _Repodata *, void *);
  void *loadcallbackdata;
};

#define SAT_FATAL			(1<<0)
#define SAT_ERROR			(1<<1)
#define SAT_WARN			(1<<2)
#define SAT_DEBUG_STATS			(1<<3)
#define SAT_DEBUG_RULE_CREATION		(1<<4)
#define SAT_DEBUG_PROPAGATE		(1<<5)
#define SAT_DEBUG_ANALYZE		(1<<6)
#define SAT_DEBUG_UNSOLVABLE		(1<<7)
#define SAT_DEBUG_SOLUTIONS		(1<<8)
#define SAT_DEBUG_POLICY		(1<<9)
#define SAT_DEBUG_RESULT		(1<<10)
#define SAT_DEBUG_JOB			(1<<11)
#define SAT_DEBUG_SCHUBI		(1<<12)

//-----------------------------------------------


/* mark dependencies with relation by setting bit31 */

#define MAKERELDEP(id) ((id) | 0x80000000)
#define ISRELDEP(id) (((id) & 0x80000000) != 0)
#define GETRELID(id) ((id) ^ 0x80000000)				/* returns Id */
#define GETRELDEP(pool, id) ((pool)->rels + ((id) ^ 0x80000000))	/* returns Reldep* */

#define REL_GT		1
#define REL_EQ		2
#define REL_LT		4

#define REL_AND		16
#define REL_OR		17
#define REL_WITH	18
#define REL_NAMESPACE	19

#if !defined(__GNUC__) && !defined(__attribute__)
# define __attribute__(x)
#endif

/**
 * Creates a new pool
 */
extern Pool *pool_create(void);
/**
 * Delete a pool
 */
extern void pool_free(Pool *pool);

extern void pool_debug(Pool *pool, int type, const char *format, ...) __attribute__((format(printf, 3, 4)));

extern char *pool_alloctmpspace(Pool *pool, int len);

/**
 * Solvable management
 */
extern Id pool_add_solvable(Pool *pool);
extern Id pool_add_solvable_block(Pool *pool, int count);

extern void pool_free_solvable_block(Pool *pool, Id start, int count, int reuseids);
static inline Solvable *pool_id2solvable(Pool *pool, Id p)
{
  return pool->solvables + p;
}
extern const char *solvable2str(Pool *pool, Solvable *s);

void pool_set_languages(Pool *pool, const char **languages, int nlanguages);

Id solvable_lookup_id(Solvable *s, Id keyname);
unsigned int solvable_lookup_num(Solvable *s, Id keyname, unsigned int notfound);
const char *solvable_lookup_str(Solvable *s, Id keyname);
const char *solvable_lookup_str_poollang(Solvable *s, Id keyname);
const char *solvable_lookup_str_lang(Solvable *s, Id keyname, const char *lang);
int solvable_lookup_bool(Solvable *s, Id keyname);
char * solvable_get_location(Solvable *s, unsigned int *medianrp);
const unsigned char *solvable_lookup_bin_checksum(Solvable *s, Id keyname, Id *typep);
const char *solvable_lookup_checksum(Solvable *s, Id keyname, Id *typep);




/**
 * Prepares a pool for solving
 */
extern void pool_createwhatprovides(Pool *pool);
extern void pool_addfileprovides(Pool *pool, struct _Repo *installed);
extern void pool_addfileprovides_ids(Pool *pool, struct _Repo *installed, Id **idp);
extern void pool_freewhatprovides(Pool *pool);
extern Id pool_queuetowhatprovides(Pool *pool, Queue *q);

static inline int pool_installable(Pool *pool, Solvable *s)
{
  if (!s->arch || s->arch == ARCH_SRC || s->arch == ARCH_NOSRC)
    return 0;
  if (pool->id2arch && (s->arch > pool->lastarch || !pool->id2arch[s->arch]))
    return 0;
  if (pool->considered)
    { 
      Id id = s - pool->solvables;
      if (!MAPTST(pool->considered, id))
	return 0;
    }
  return 1;
}

extern Id *pool_addrelproviders(Pool *pool, Id d);

static inline Id *pool_whatprovides(Pool *pool, Id d)
{
  Id v;
  if (!ISRELDEP(d))
    return pool->whatprovidesdata + pool->whatprovides[d];
  v = GETRELID(d);
  if (pool->whatprovides_rel[v])
    return pool->whatprovidesdata + pool->whatprovides_rel[v];
  return pool_addrelproviders(pool, d);
}

extern void pool_setdebuglevel(Pool *pool, int level);

static inline void pool_setdebugcallback(Pool *pool, void (*debugcallback)(struct _Pool *, void *data, int type, const char *str), void *debugcallbackdata)
{
  pool->debugcallback = debugcallback;
  pool->debugcallbackdata = debugcallbackdata;
}

static inline void pool_setdebugmask(Pool *pool, int mask)
{
  pool->debugmask = mask;
}

static inline void pool_setloadcallback(Pool *pool, FILE *(*cb)(struct _Pool *, struct _Repodata *, void *), void *loadcbdata)
{
  pool->loadcallback = cb;
  pool->loadcallbackdata = loadcbdata;
}

/* search the pool. the following filters are available:
 *   p     - search just this solvable
 *   key   - search only this key
 *   match - key must match this string
 */
void pool_search(Pool *pool, Id p, Id key, const char *match, int flags, int (*callback)(void *cbdata, Solvable *s, struct _Repodata *data, struct _Repokey *key, struct _KeyValue *kv), void *cbdata);

typedef struct _duchanges {
  const char *path;
  int kbytes;
  int files;
} DUChanges;

void pool_calc_duchanges(Pool *pool, Queue *pkgs, DUChanges *mps, int nmps);
int pool_calc_installsizechange(Pool *pool, Queue *pkgs);

/* loop over all providers of d */
#define FOR_PROVIDES(v, vp, d) 						\
  for (vp = pool_whatprovides(pool, d) ; (v = *vp++) != 0; )

#define POOL_DEBUG(type, ...) do {if ((pool->debugmask & (type)) != 0) pool_debug(pool, (type), __VA_ARGS__);} while (0)
#define IF_POOLDEBUG(type) if ((pool->debugmask & (type)) != 0)

#ifdef __cplusplus
}
#endif


#endif /* SATSOLVER_POOL_H */
