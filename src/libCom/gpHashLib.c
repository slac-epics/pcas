/* share/src/libCom gphLib.c*/
/* share/src/libCom $Id$ */
/* Author:  Marty Kraimer Date:    04-07-94 */
/*****************************************************************
                          COPYRIGHT NOTIFICATION
*****************************************************************

THE FOLLOWING IS A NOTICE OF COPYRIGHT, AVAILABILITY OF THE CODE,
AND DISCLAIMER WHICH MUST BE INCLUDED IN THE PROLOGUE OF THE CODE
AND IN ALL SOURCE LISTINGS OF THE CODE.
 
(C)  COPYRIGHT 1993 UNIVERSITY OF CHICAGO
 
Argonne National Laboratory (ANL), with facilities in the States of 
Illinois and Idaho, is owned by the United States Government, and
operated by the University of Chicago under provision of a contract
with the Department of Energy.

Portions of this material resulted from work developed under a U.S.
Government contract and are subject to the following license:  For
a period of five years from March 30, 1993, the Government is
granted for itself and others acting on its behalf a paid-up,
nonexclusive, irrevocable worldwide license in this computer
software to reproduce, prepare derivative works, and perform
publicly and display publicly.  With the approval of DOE, this
period may be renewed for two additional five year periods. 
Following the expiration of this period or periods, the Government
is granted for itself and others acting on its behalf, a paid-up,
nonexclusive, irrevocable worldwide license in this computer
software to reproduce, prepare derivative works, distribute copies
to the public, perform publicly and display publicly, and to permit
others to do so.

*****************************************************************
                                DISCLAIMER
*****************************************************************

NEITHER THE UNITED STATES GOVERNMENT NOR ANY AGENCY THEREOF, NOR
THE UNIVERSITY OF CHICAGO, NOR ANY OF THEIR EMPLOYEES OR OFFICERS,
MAKES ANY WARRANTY, EXPRESS OR IMPLIED, OR ASSUMES ANY LEGAL
LIABILITY OR RESPONSIBILITY FOR THE ACCURACY, COMPLETENESS, OR
USEFULNESS OF ANY INFORMATION, APPARATUS, PRODUCT, OR PROCESS
DISCLOSED, OR REPRESENTS THAT ITS USE WOULD NOT INFRINGE PRIVATELY
OWNED RIGHTS.  

*****************************************************************
LICENSING INQUIRIES MAY BE DIRECTED TO THE INDUSTRIAL TECHNOLOGY
DEVELOPMENT CENTER AT ARGONNE NATIONAL LABORATORY (708-252-2000).
 *
 * Modification Log:
 * -----------------
 * .01  04-07-94	mrk	Initial Implementation
 */

#include <dbDefs.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <gpHash.h>
#include <ellLib.h>


#define 	HASH_NO	256		/* number of hash table entries */


/*The hash algorithm is the algorithm described in			*/
/* Fast Hashing of Variable Length Text Strings, Peter K. Pearson,	*/
/* Communications of the ACM, June 1990					*/

static unsigned char T0[256] = {
 39,159,180,252, 71,  6, 13,164,232, 35,226,155, 98,120,154, 69,
157, 24,137, 29,147, 78,121, 85,112,  8,248,130, 55,117,190,160,
176,131,228, 64,211,106, 38, 27,140, 30, 88,210,227,104, 84, 77,
 75,107,169,138,195,184, 70, 90, 61,166,  7,244,165,108,219, 51,
  9,139,209, 40, 31,202, 58,179,116, 33,207,146, 76, 60,242,124,
254,197, 80,167,153,145,129,233,132, 48,246, 86,156,177, 36,187,
 45,  1, 96, 18, 19, 62,185,234, 99, 16,218, 95,128,224,123,253,
 42,109,  4,247, 72,  5,151,136,  0,152,148,127,204,133, 17, 14,
182,217, 54,199,119,174, 82, 57,215, 41,114,208,206,110,239, 23,
189, 15,  3, 22,188, 79,113,172, 28,  2,222, 21,251,225,237,105,
102, 32, 56,181,126, 83,230, 53,158, 52, 59,213,118,100, 67,142,
220,170,144,115,205, 26,125,168,249, 66,175, 97,255, 92,229, 91,
214,236,178,243, 46, 44,201,250,135,186,150,221,163,216,162, 43,
 11,101, 34, 37,194, 25, 50, 12, 87,198,173,240,193,171,143,231,
111,141,191,103, 74,245,223, 20,161,235,122, 63, 89,149, 73,238,
134, 68, 93,183,241, 81,196, 49,192, 65,212, 94,203, 10,200, 47 
};

#ifdef __STDC__
static void *myCalloc(size_t nobj,size_t size)
#else
static void *myCalloc(nobj,size)
  size_t nobj;
  size_t size;
#endif
{
    void *p;

    p=calloc(nobj,size);
    if(p) return(p);
#ifdef vxWorks
    taskSuspend(0);
#else
    abort();
#endif
    return(NULL);
}

#ifdef __STDC__
static unsigned char hash( char *pname)
#else
static unsigned char hash(pname)
 char *pname;
#endif
{
    unsigned char  h=0;

    while(*pname) {
	h = T0[h^*pname];
	pname++;
    }
    return(h);
}

#ifdef __STDC__
void gphInitPvt(void **pgphPvt)
#else
void gphInitPvt(pgphPvt)
  void **pgphPvt;
#endif
{
    ELLLIST        **pgph;
    pgph = myCalloc(HASH_NO, sizeof(ELLLIST *));
    *pgphPvt = (void *)pgph;
    return;
}
	
#ifdef __STDC__
GPHENTRY *gphFind(void *gphPvt,char *name,void *pvtid)
#else
GPHENTRY *gphFind(gphPvt,name,pvtid)
void *gphPvt;
char *name;
void *pvtid;
#endif
{
    unsigned short	hashInd;
    ELLLIST		**pgph = (ELLLIST **) gphPvt;
    ELLLIST		*gphlist;
    GPHENTRY		*pgphNode;
    
    hashInd = (unsigned short)hash(name);
    if ((gphlist=pgph[hashInd]) == NULL) return (NULL);
    pgphNode = (GPHENTRY *) ellFirst(gphlist);
    while(pgphNode) {
	if(strcmp(name,(char *)pgphNode->name) == 0) {
	    if(pvtid==pgphNode->pvtid) return(pgphNode);
	}
	pgphNode = (GPHENTRY *) ellNext((ELLNODE*)pgphNode);
    }
    return (NULL);
}

#ifdef __STDC__
GPHENTRY *gphAdd(void *gphPvt,char *name,void *pvtid)
#else
GPHENTRY *gphAdd(gphPvt,name,pvtid)
void *gphPvt;
char *name;
void *pvtid;
#endif
{
    unsigned short	hashInd;
    ELLLIST		**pgph = (ELLLIST **) gphPvt;
    ELLLIST		*plist;
    GPHENTRY		*pgphNode;
    
    hashInd = (unsigned short)hash(name);
    if (pgph[hashInd] == NULL) {
	pgph[hashInd] = myCalloc(1, sizeof(ELLLIST));
	ellInit(pgph[hashInd]);
    }
    plist=pgph[hashInd];
    pgphNode = (GPHENTRY *) ellFirst(plist);
    while(pgphNode) {
	if((strcmp(name,(char *)pgphNode->name) == 0)
	&&(pvtid == pgphNode->pvtid)) return(NULL);
	pgphNode = (GPHENTRY *) ellNext((ELLNODE*)pgphNode);
    }
    pgphNode = myCalloc(1, (unsigned) sizeof(GPHENTRY));
    pgphNode->name = name;
    pgphNode->pvtid = pvtid;
    ellAdd(plist, (ELLNODE*)pgphNode);
    return (pgphNode);
}

#ifdef __STDC__
void gphDelete(void *gphPvt,char *name,void *pvtid)
#else
void gphDelete(gphPvt,name,pvtid)
void *gphPvt;
char *name;
void *pvtid;
#endif
{
    unsigned short	hashInd;
    ELLLIST		**pgph = (ELLLIST **) gphPvt;
    ELLLIST		*plist;
    GPHENTRY		*pgphNode;
    
    hashInd = (unsigned short)hash(name);
    if (pgph[hashInd] == NULL) return;
    plist=pgph[hashInd];
    pgphNode = (GPHENTRY *) ellFirst(plist);
    while(pgphNode) {
	if((strcmp(name,(char *)pgphNode->name) == 0)
	&&(pvtid == pgphNode->pvtid)) {
	    ellDelete(plist, (ELLNODE*)pgphNode);
	    free((void *)pgphNode);
	    return;
	}
	pgphNode = (GPHENTRY *) ellNext((ELLNODE*)pgphNode);
    }
    return;
}

#ifdef __STDC__
void gphFreeMem(void * gphPvt)
#else
void gphFreeMem(gphPvt)
void * gphPvt;
#endif
{
    unsigned short	hashInd;
    ELLLIST		**pgph = (ELLLIST **) gphPvt;
    ELLLIST		*plist;
    GPHENTRY		*pgphNode;
    GPHENTRY		*next;;
    
    if (pgph == NULL) return;
    for (hashInd=0; hashInd<HASH_NO; hashInd++) {
	if(pgph[hashInd] == NULL) continue;
	plist=pgph[hashInd];
	pgphNode = (GPHENTRY *) ellFirst(plist);
	while(pgphNode) {
	    next = (GPHENTRY *) ellNext((ELLNODE*)pgphNode);
	    ellDelete(plist,(ELLNODE*)pgphNode);
	    free((void *)pgphNode);
	    pgphNode = next;
	}
	free((void *)pgph[hashInd]);
    }
    free((void *)pgph);
}

#ifdef __STDC__
void gphDump(void * gphPvt)
#else
void gphDump(gphPvt)
void * gphPvt;
#endif
{
    unsigned short	hashInd;
    ELLLIST		**pgph = (ELLLIST **) gphPvt;
    ELLLIST		*plist;
    GPHENTRY		*pgphNode;
    int			number;
    
    if (pgph == NULL) return;
    for (hashInd=0; hashInd<HASH_NO; hashInd++) {
	if(pgph[hashInd] == NULL) continue;
	plist=pgph[hashInd];
	pgphNode = (GPHENTRY *) ellFirst(plist);
	printf("\n %3.3hd=%3.3d",hashInd,ellCount(plist));
	number=0;
	while(pgphNode) {
	    printf(" %s %8x",pgphNode->name,pgphNode->pvtid);
	    if(number++ ==2) {number=0;printf("\n        ");}
	    pgphNode = (GPHENTRY *) ellNext((ELLNODE*)pgphNode);
	}
    }
    printf("\n End of General Purpose Hash\n");
}
