
#define MAX_NAMES 100

typedef enum obj_type { LINE, SPHERE, BOUNDS} OBJ_TYPE;
typedef enum proj { PARALLEL, PERSPECTIVE} PROJ;

typedef struct objecttype {
    float x1,y1,z1;
    float x2,y2,z2;
    float x1_orig, y1_orig, z1_orig;
    float x2_orig, y2_orig, z2_orig;
    float radius;
    int x1r,y1r,z1r;
    int x2r,y2r,z2r;
    int active;
    int visible;
    int radius_scaled;
    char color[25];
    char name[25];
    int color_index;
    Pixmap mask_pixmap,pixmap,save_pixmap,stipple_pixmap;
    GC pixmap_gc,win_gc;
    struct objecttype *next;
    OBJ_TYPE type;
    struct objecttype *use_this;
    struct objecttype *sphere_1;
    struct objecttype *sphere_2;
} OBJECT;

typedef struct depth {
    float z1;
    OBJECT *ptr;
    int next;
  } DEPTH;

extern float xmin,ymin,xmax,ymax,zmin,zmax;
extern float biggest_r;
extern Widget toplevel, frame_canvas, canvas,form,board;
extern GC gc,gc_clip,canvas_gc,text_gc;
extern Pixmap pix,canvas_pix;
extern XColor start_color;
extern OBJECT object_head;
extern OBJECT *object;
extern int object_count;
extern int lshade;
extern int cshade;
extern XGCValues values;
extern Dimension canvas_width,canvas_height;
extern int pfd[2];
extern DEPTH *depth_list;
extern int first_z;
extern int do_proj;
extern int no_size;
extern Widget x_text,y_text,z_text,name_text;
extern float z_proj;
extern int bounds_set;
extern OBJECT bounds;
extern PROJ proj_type;
extern double xp,yp,zp;
extern int old_object_count,total_objects;
extern Widget frame_x;
extern int zclip_active;
extern int end_depth;
extern float offset_width,offset_height;
extern int input_2D;
extern int radius_scale;
extern char colors[][25];
extern int apply_resize[100];
extern Widget frame_y;
extern int update_rotate;
extern float pic_xmin, pic_ymin, pic_zmin, pic_xmax, pic_ymax, pic_zmax;
extern float global_rot[3][3];
extern int first_time;
extern int interactive;
extern int mono_only;
extern int old_cshade;
extern XColor cdata[];
extern int num_colors;
extern char *name_table[];
extern long pix_back;
extern int depth_cues;



