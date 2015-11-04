#ifndef _gpc_h_
#define _gpc_h_

#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <math.h>






/*
===========================================================================
                               Constants
===========================================================================
*/

/* Increase GPC_EPSILON to encourage merging of near coincident edges    */

#define GPC_EPSILON (DBL_EPSILON)
#define GPC_VERSION "2.32"


/*
===========================================================================
                           Public Data Types
===========================================================================
*/

typedef enum                        /* Set operation type                */
{
  GPC_DIFF,                         /* Difference                        */
  GPC_INT,                          /* Intersection                      */
  GPC_XOR,                          /* Exclusive or                      */
  GPC_UNION                         /* Union                             */
} gpc_op;

typedef struct                      /* Polygon vertex structure          */
{
  double              x;            /* Vertex x component                */
  double              y;            /* vertex y component                */
  double			  z;
  int				  idx;
} gpc_vertex;

typedef struct                      /* Vertex list structure             */
{
  int                 num_vertices; /* Number of vertices in list        */
  gpc_vertex         *vertex;       /* Vertex array pointer              */
} gpc_vertex_list;

typedef struct                      /* Polygon set structure             */
{
  int                 num_contours; /* Number of contours in polygon     */
  int                *hole;         /* Hole / external contour flags     */
  gpc_vertex_list    *contour;      /* Contour array pointer             */
} gpc_polygon;

typedef struct                      /* Tristrip set structure            */
{
  int                 num_strips;   /* Number of tristrips               */
  gpc_vertex_list    *strip;        /* Tristrip array pointer            */
} gpc_tristrip;

/*
===========================================================================
Constants
===========================================================================
*/

#ifndef TRUE
#define FALSE              0
#define TRUE               1
#endif

#define LEFT               0
#define RIGHT              1

#define ABOVE              0
#define BELOW              1

#define CLIP               0
#define SUBJ               1

#define INVERT_TRISTRIPS   FALSE


/*
===========================================================================
Macros 
===========================================================================
*/

#define EQ(a, b)           (fabs((a) - (b)) <= GPC_EPSILON)

#define PREV_INDEX(i, n)   ((i - 1 + n) % n)
#define NEXT_INDEX(i, n)   ((i + 1    ) % n)

#define OPTIMAL(v, i, n)   ((v[PREV_INDEX(i, n)].y != v[i].y) || \
	(v[NEXT_INDEX(i, n)].y != v[i].y))

#define FWD_MIN(v, i, n)   ((v[PREV_INDEX(i, n)].vertex.y >= v[i].vertex.y) \
	&& (v[NEXT_INDEX(i, n)].vertex.y > v[i].vertex.y))

#define NOT_FMAX(v, i, n)   (v[NEXT_INDEX(i, n)].vertex.y > v[i].vertex.y)

#define REV_MIN(v, i, n)   ((v[PREV_INDEX(i, n)].vertex.y > v[i].vertex.y) \
	&& (v[NEXT_INDEX(i, n)].vertex.y >= v[i].vertex.y))

#define NOT_RMAX(v, i, n)   (v[PREV_INDEX(i, n)].vertex.y > v[i].vertex.y)

#define VERTEX(e,p,s,x,y)  {add_vertex(&((e)->outp[(p)]->v[(s)]), x, y); \
	(e)->outp[(p)]->active++;}

#define P_EDGE(d,e,p,i,j)  {(d)= (e); \
	do {(d)= (d)->prev;} while (!(d)->outp[(p)]); \
	(i)= (d)->bot.x + (d)->dx * ((j)-(d)->bot.y);}

#define N_EDGE(d,e,p,i,j)  {(d)= (e); \
	do {(d)= (d)->next;} while (!(d)->outp[(p)]); \
	(i)= (d)->bot.x + (d)->dx * ((j)-(d)->bot.y);}

#define MALLOC(p, b, s, t) {if ((b) > 0) { \
	p= (t*)malloc(b); if (!(p)) { \
	fprintf(stderr, "gpc malloc failure: %s\n", s); \
	exit(0);}} else p= NULL;}

#define FREE(p)            {if (p) {free(p); (p)= NULL;}}


/*
===========================================================================
Private Data Types
===========================================================================
*/

typedef enum                        /* Edge intersection classes         */
{
	NUL,                              /* Empty non-intersection            */
	EMX,                              /* External maximum                  */
	ELI,                              /* External left intermediate        */
	TED,                              /* Top edge                          */
	ERI,                              /* External right intermediate       */
	RED,                              /* Right edge                        */
	IMM,                              /* Internal maximum and minimum      */
	IMN,                              /* Internal minimum                  */
	EMN,                              /* External minimum                  */
	EMM,                              /* External maximum and minimum      */
	LED,                              /* Left edge                         */
	ILI,                              /* Internal left intermediate        */
	BED,                              /* Bottom edge                       */
	IRI,                              /* Internal right intermediate       */
	IMX,                              /* Internal maximum                  */
	FUL                               /* Full non-intersection             */
} vertex_type;

typedef enum                        /* Horizontal edge states            */
{
	NH,                               /* No horizontal edge                */
	BH,                               /* Bottom horizontal edge            */
	TH                                /* Top horizontal edge               */
} h_state;

typedef enum                        /* Edge bundle state                 */
{
	UNBUNDLED,                        /* Isolated edge not within a bundle */
	BUNDLE_HEAD,                      /* Bundle head node                  */
	BUNDLE_TAIL                       /* Passive bundle tail node          */
} bundle_state;

typedef struct v_shape              /* Internal vertex list datatype     */
{
	double              x;            /* X coordinate component            */
	double              y;            /* Y coordinate component            */
	struct v_shape     *next;         /* Pointer to next vertex in list    */
} vertex_node;

typedef struct p_shape              /* Internal contour / tristrip type  */
{
	int                 active;       /* Active flag / vertex count        */
	int                 hole;         /* Hole / external contour flag      */
	vertex_node        *v[2];         /* Left and right vertex list ptrs   */
	struct p_shape     *next;         /* Pointer to next polygon contour   */
	struct p_shape     *proxy;        /* Pointer to actual structure used  */
} polygon_node;

typedef struct edge_shape
{
	gpc_vertex          vertex;       /* Piggy-backed contour vertex data  */
	gpc_vertex          bot;          /* Edge lower (x, y) coordinate      */
	gpc_vertex          top;          /* Edge upper (x, y) coordinate      */
	double              xb;           /* Scanbeam bottom x coordinate      */
	double              xt;           /* Scanbeam top x coordinate         */
	double              dx;           /* Change in x for a unit y increase */
	int                 type;         /* Clip / subject edge flag          */
	int                 bundle[2][2]; /* Bundle edge flags                 */
	int                 bside[2];     /* Bundle left / right indicators    */
	bundle_state        bstate[2];    /* Edge bundle state                 */
	polygon_node       *outp[2];      /* Output polygon / tristrip pointer */
	struct edge_shape  *prev;         /* Previous edge in the AET          */
	struct edge_shape  *next;         /* Next edge in the AET              */
	struct edge_shape  *pred;         /* Edge connected at the lower end   */
	struct edge_shape  *succ;         /* Edge connected at the upper end   */
	struct edge_shape  *next_bound;   /* Pointer to next bound in LMT      */
} edge_node;

typedef struct lmt_shape            /* Local minima table                */
{
	double              y;            /* Y coordinate at local minimum     */
	edge_node          *first_bound;  /* Pointer to bound list             */
	struct lmt_shape   *next;         /* Pointer to next local minimum     */
} lmt_node;

typedef struct sbt_t_shape          /* Scanbeam tree                     */
{
	double              y;            /* Scanbeam node y value             */
	struct sbt_t_shape *less;         /* Pointer to nodes with lower y     */
	struct sbt_t_shape *more;         /* Pointer to nodes with higher y    */
} sb_tree;

typedef struct it_shape             /* Intersection table                */
{
	edge_node          *ie[2];        /* Intersecting edge (bundle) pair   */
	gpc_vertex          point;        /* Point of intersection             */
	struct it_shape    *next;         /* The next intersection table node  */
} it_node;

typedef struct st_shape             /* Sorted edge table                 */
{
	edge_node          *edge;         /* Pointer to AET edge               */
	double              xb;           /* Scanbeam bottom x coordinate      */
	double              xt;           /* Scanbeam top x coordinate         */
	double              dx;           /* Change in x for a unit y increase */
	struct st_shape    *prev;         /* Previous edge in sorted list      */
} st_node;

typedef struct bbox_shape           /* Contour axis-aligned bounding box */
{
	double             xmin;          /* Minimum x coordinate              */
	double             ymin;          /* Minimum y coordinate              */
	double             xmax;          /* Maximum x coordinate              */
	double             ymax;          /* Maximum y coordinate              */
} bbox;


/*
===========================================================================
Global Data
===========================================================================
*/

/* Horizontal edge state transitions within scanbeam boundary */
const h_state next_h_state[3][6]=
{
	/*        ABOVE     BELOW     CROSS */
	/*        L   R     L   R     L   R */  
	/* NH */ {BH, TH,   TH, BH,   NH, NH},
	/* BH */ {NH, NH,   NH, NH,   TH, TH},
	/* TH */ {NH, NH,   NH, NH,   BH, BH}
};



/*
===========================================================================
                       Public Function Prototypes
===========================================================================
*/
class GpcPoly
{

public:
	static void gpc_read_polygon	  (FILE            *infile_ptr, 
		int              read_hole_flags,
		gpc_polygon     *polygon);

	static void gpc_write_polygon       (FILE            *outfile_ptr,
		int              write_hole_flags,
		gpc_polygon     *polygon);

	static void gpc_add_contour(gpc_polygon     *polygon,
		gpc_vertex_list *contour,
		int              hole);

	static void gpc_polygon_clip(gpc_op           set_operation,
		gpc_polygon     *subject_polygon,
		gpc_polygon     *clip_polygon,
		gpc_polygon     *result_polygon);

	static void gpc_tristrip_clip(gpc_op           set_operation,
		gpc_polygon     *subject_polygon,
		gpc_polygon     *clip_polygon,
		gpc_tristrip    *result_tristrip);

	static void gpc_polygon_to_tristrip(gpc_polygon     *polygon,
		gpc_tristrip    *tristrip);

	static void gpc_free_polygon(gpc_polygon     *polygon);

	static void gpc_free_tristrip(gpc_tristrip    *tristrip);

private:
	static void reset_it(it_node **it);
	static void reset_lmt(lmt_node **lmt);
	static void insert_bound(edge_node **b, edge_node *e);
	static edge_node** bound_list(lmt_node **lmt, double y);

	static void add_to_sbtree(int *entries, sb_tree **sbtree, double y);
	static void build_sbt(int *entries, double *sbt, sb_tree *sbtree);
	static void free_sbtree(sb_tree **sbtree);

	static int count_optimal_vertices(gpc_vertex_list c);
	static edge_node* build_lmt(lmt_node **lmt, sb_tree **sbtree, int *sbt_entries, gpc_polygon *p, int type, gpc_op op);
	static void add_edge_to_aet(edge_node **aet, edge_node *edge, edge_node *prev);
	static void add_intersection(it_node **it, edge_node *edge0, edge_node *edge1, double x, double y);
	static void add_st_edge(st_node **st, it_node **it, edge_node *edge, double dy);
	static void build_intersection_table(it_node **it, edge_node *aet, double dy);
	static int count_contours(polygon_node *polygon);
	static void add_left(polygon_node *p, double x, double y);
	static void merge_left(polygon_node *p, polygon_node *q, polygon_node *list);
	static void add_right(polygon_node *p, double x, double y);
	static void merge_right(polygon_node *p, polygon_node *q, polygon_node *list);
	static void add_local_min(polygon_node **p, edge_node *edge, double x, double y);
	static int count_tristrips(polygon_node *tn);
	static void add_vertex(vertex_node **t, double x, double y);
	static void new_tristrip(polygon_node **tn, edge_node *edge, double x, double y);
	static bbox* create_contour_bboxes(gpc_polygon *p);
	static void minimax_test(gpc_polygon *subj, gpc_polygon *clip, gpc_op op);

};


#endif