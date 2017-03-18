

#include "globals.h"
#include "errno.h"

#include "util/string.h"
#include "util/debug.h"

#include "mm/mmobj.h"
#include "mm/pframe.h"
#include "mm/mm.h"
#include "mm/page.h"
#include "mm/slab.h"
#include "mm/tlb.h"

#include "vm/vmmap.h"
#include "vm/shadow.h"
#include "vm/shadowd.h"

#define SHADOW_SINGLETON_THRESHOLD 5

int shadow_count = 0; /* for debugging/verification purposes */
#ifdef __SHADOWD__
/*
 * number of shadow objects with a single parent, that is another shadow
 * object in the shadow objects tree(singletons)
 */
static int shadow_singleton_count = 0;
#endif

static slab_allocator_t *shadow_allocator;

static void shadow_ref(mmobj_t *o);
static void shadow_put(mmobj_t *o);
static int  shadow_lookuppage(mmobj_t *o, uint32_t pagenum, int forwrite, pframe_t **pf);
static int  shadow_fillpage(mmobj_t *o, pframe_t *pf);
static int  shadow_dirtypage(mmobj_t *o, pframe_t *pf);
static int  shadow_cleanpage(mmobj_t *o, pframe_t *pf);

static mmobj_ops_t shadow_mmobj_ops = {
        .ref = shadow_ref,
        .put = shadow_put,
        .lookuppage = shadow_lookuppage,
        .fillpage  = shadow_fillpage,
        .dirtypage = shadow_dirtypage,
        .cleanpage = shadow_cleanpage
};

/*
 * This function is called at boot time to initialize the
 * shadow page sub system. Currently it only initializes the
 * shadow_allocator object.
 */
void
shadow_init()
{
        /* NOT_YET_IMPLEMENTED("VM: shadow_init"); */

	shadow_allocator = slab_allocator_create("shadow", sizeof(mmobj_t));

	KASSERT(shadow_allocator);
	dbg(DBG_PRINT, "\n (GRADING3A 6.a) Condition KASSERT(shadow_allocator) has been passed !\n");
}

/*
 * You'll want to use the shadow_allocator to allocate the mmobj to
 * return, then then initialize it. Take a look in mm/mmobj.h for
 * macros which can be of use here. Make sure your initial
 * reference count is correct.
 */
mmobj_t *
shadow_create()
{
        /* NOT_YET_IMPLEMENTED("VM: shadow_create"); */

	dbg( DBG_PRINT, "\n GRADING3D ENTERING shadow_create() \n");
	mmobj_t *mmobj = (mmobj_t*)slab_obj_alloc(shadow_allocator);
	if (mmobj != NULL)
	{
		mmobj_init(mmobj, &shadow_mmobj_ops);
		mmobj->mmo_refcount = 1;
		dbg( DBG_PRINT, "\n GRADING3D INSIDE shadow_create() : mmobj is not null \n");
	}

	dbg( DBG_PRINT, "\n GRADING3D EXITING shadow_create() \n");
	return mmobj;
}

/* Implementation of mmobj entry points: */

/*
 * Increment the reference count on the object.
 */
static void
shadow_ref(mmobj_t *o)
{
        /* NOT_YET_IMPLEMENTED("VM: shadow_ref"); */

	KASSERT(o && (0 < o->mmo_refcount) && (&shadow_mmobj_ops == o->mmo_ops));
	dbg(DBG_PRINT, "(GRADING3A 6.b) Condition KASSERT(o && (0 < o->mmo_refcount) && (&shadow_mmobj_ops == o->mmo_ops)) has been passed !\n");

	o->mmo_refcount++;
}

/*
 * Decrement the reference count on the object. If, however, the
 * reference count on the object reaches the number of resident
 * pages of the object, we can conclude that the object is no
 * longer in use and, since it is a shadow object, it will never
 * be used again. You should unpin and uncache all of the object's
 * pages and then free the object itself.
 */
static void
shadow_put(mmobj_t *o)
{
        /* NOT_YET_IMPLEMENTED("VM: shadow_put"); */

	KASSERT(o && (0 < o->mmo_refcount) && (&shadow_mmobj_ops == o->mmo_ops));
	dbg(DBG_PRINT, "(GRADING3A 6.c) Condition KASSERT(o && (0 < o->mmo_refcount) && (&shadow_mmobj_ops == o->mmo_ops)) has been passed !\n");

	if ( (o->mmo_refcount - 1) == o->mmo_nrespages )
	{
		dbg( DBG_PRINT, "\n GRADING3D INSIDE shadow_put() : refcount - 1 <= no. of res pages \n");

		if ( ! list_empty ( &o->mmo_respages ) )
		{

			dbg( DBG_PRINT, "\n GRADING3D INSIDE shadow_put() : list is not empty \n");
			pframe_t *pf;
			list_iterate_begin ( &(o->mmo_respages), pf, pframe_t, pf_olink )
			{
				KASSERT((!pframe_is_free(pf)));

				if ( pframe_is_pinned (pf) )
				{
					dbg( DBG_PRINT, "\n GRADING3D INSIDE shadow_put() : pframe is pinned then unpin \n");
					pframe_unpin(pf);
				}
				
				if ( pframe_is_dirty (pf) )
				{
					dbg( DBG_PRINT, "\n GRADING3D INSIDE shadow_put() : pframe is dirty then clean \n");
					pframe_clean(pf);
				}

				pframe_free(pf);

			} list_iterate_end();

			 
			o->mmo_shadowed->mmo_ops->put(o->mmo_shadowed);

			slab_obj_free(shadow_allocator, o);
		}
		else
		{
			dbg( DBG_PRINT, "\n GRADING3D INSIDE shadow_put() : list is empty \n");

			o->mmo_shadowed->mmo_ops->put(o->mmo_shadowed);

			slab_obj_free(shadow_allocator, o);
		}
	}
	o->mmo_refcount--;
	dbg( DBG_PRINT, "\n GRADING3D EXITING shadow_put() \n");
}

/* This function looks up the given page in this shadow object. The
 * forwrite argument is true if the page is being looked up for
 * writing, false if it is being looked up for reading. This function
 * must handle all do-not-copy-on-not-write magic (i.e. when forwrite
 * is false find the first shadow object in the chain which has the
 * given page resident). copy-on-write magic (necessary when forwrite
 * is true) is handled in shadow_fillpage, not here. It is important to
 * use iteration rather than recursion here as a recursive implementation
 * can overflow the kernel stack when looking down a long shadow chain */
static int
shadow_lookuppage(mmobj_t *o, uint32_t pagenum, int forwrite, pframe_t **pf)
{
        /* NOT_YET_IMPLEMENTED("VM: shadow_lookuppage"); */

	dbg( DBG_PRINT, "\n GRADING3D ENTERING shadow_lookuppage() \n");
	mmobj_t *temp_obj;
	pframe_t *temp_frame = NULL;
	temp_obj = o;

	if(!forwrite)
	{
		dbg( DBG_PRINT, "\n GRADING3D INSIDE shadow_lookuppage() : forwrite is not 0 \n");
		while( temp_obj->mmo_shadowed != NULL)
		{
			temp_frame = pframe_get_resident(temp_obj, pagenum);
			temp_obj = temp_obj->mmo_shadowed;
			if(temp_frame != NULL)
			{
				dbg( DBG_PRINT, "\n GRADING3D INSIDE shadow_lookuppage()  : temp_frame is not null inside loop \n");
				break;
			}
		}
		if ( temp_frame == NULL)
		{
			dbg( DBG_PRINT, "\n GRADING3D INSIDE shadow_lookuppage() : temp_frame is null \n");
			return pframe_lookup(temp_obj, pagenum, 0 , pf);
		}
		else
		{
			dbg( DBG_PRINT, "\n GRADING3D INSIDE shadow_lookuppage() : temp_frame is not null \n");
			*pf = temp_frame;

			KASSERT(NULL != (*pf));
			dbg(DBG_PRINT, "(GRADING3A 6.d) Condition KASSERT(NULL != (*pf)) has been passed !\n");

			KASSERT((pagenum == (*pf)->pf_pagenum) && (!pframe_is_busy(*pf)));
			dbg(DBG_PRINT, "(GRADING3A 6.d) Condition KASSERT((pagenum == (*pf)->pf_pagenum) && (!pframe_is_busy(*pf))); has been passed !\n");

			return 0;
		}
	}
	else
	{
		dbg( DBG_PRINT, "\n GRADING3D EXITING shadow_lookuppage() \n");
		return pframe_get(o, pagenum, pf);
	}
}

/* As per the specification in mmobj.h, fill the page frame starting
 * at address pf->pf_addr with the contents of the page identified by
 * pf->pf_obj and pf->pf_pagenum. This function handles all
 * copy-on-write magic (i.e. if there is a shadow object which has
 * data for the pf->pf_pagenum-th page then we should take that data,
 * if no such shadow object exists we need to follow the chain of
 * shadow objects all the way to the bottom object and take the data
 * for the pf->pf_pagenum-th page from the last object in the chain).
 * It is important to use iteration rather than recursion here as a 
 * recursive implementation can overflow the kernel stack when 
 * looking down a long shadow chain */
static int
shadow_fillpage(mmobj_t *o, pframe_t *pf)
{
        /* NOT_YET_IMPLEMENTED("VM: shadow_fillpage"); */

	dbg( DBG_PRINT, "\n GRADING3D ENTERING  shadow_fillpage() \n");
	KASSERT(pframe_is_busy(pf));
	dbg(DBG_PRINT, "(GRADING3A 6.e) Condition KASSERT(pframe_is_busy(pf)) has been passed !\n");

	KASSERT(!pframe_is_pinned(pf));
	dbg(DBG_PRINT, "(GRADING3A 6.e) Condition KASSERT(!pframe_is_pinned(pf)) has been passed !\n");

	pframe_t *frame;

	int status;
	status = shadow_lookuppage(o->mmo_shadowed, pf->pf_pagenum, 0, &frame);
	if ( status == 0 )
	{
		dbg( DBG_PRINT, "\n GRADING3D INSIDE shadow_fillpage() : status of lookup is 0 \n");
		memcpy(pf->pf_addr, frame->pf_addr, PAGE_SIZE);
		if ( !pframe_is_pinned(pf) )
		{
			dbg( DBG_PRINT, "\n GRADING3D INSIDE shadow_fillpage() : pframe is not pinned then pin it\n");
			pframe_pin(pf);
		}
	}

	dbg( DBG_PRINT, "\n GRADING3D EXITING shadow_fillpage() \n");
	return status;
}

/* These next two functions are not difficult. */

static int
shadow_dirtypage(mmobj_t *o, pframe_t *pf)
{
        /* NOT_YET_IMPLEMENTED("VM: shadow_dirtypage"); */
        return -1;
}

static int
shadow_cleanpage(mmobj_t *o, pframe_t *pf)
{
       /* NOT_YET_IMPLEMENTED("VM: shadow_cleanpage"); */
        return -1;
}
