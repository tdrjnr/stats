
static MenuStruct coord_popup[] = {
  {"rectanglar", reconfig, default_rec, NULL, 0, NULL},
  {"polar - deg", reconfig, default_polar, NULL, 0, NULL},
  {"polar - rad", reconfig, default_polar_in_radian, NULL, 0, NULL},
  {"spherical - deg", reconfig, default_spherical, NULL, 0, NULL},
  {"spherical - rad", reconfig, default_spherical_in_radian, NULL, 0, NULL}
};

static MenuStruct show_points_popup[] = {
  {"point", change_point, draw_one_point, NULL, 0, NULL},
  {"square", change_point, draw_square, NULL, 0, NULL},
  {"circle", change_point, draw_circle, NULL, 0, NULL},
  {"cross", change_point, draw_cross, NULL, 0, NULL},
  {"nothing", change_point, DPdummy, NULL, 0, NULL}
};

static MenuStruct axisgrid_popup[] = {
  {"Axis only", axistick, XyiAxis, NULL, 0, NULL},
  {"Tick Marks", axistick, XyiTick, NULL, 0, NULL},
  {"Grids", axistick, XyiGrid, NULL, 0, NULL},
  {"Nothing", axistick, XyiNoAxis, NULL, 0, NULL}
};

static MenuStruct second_popup[] = {
  {"help", help, NULL, NULL, 0, NULL},
  {"Coordinate Change", NULL,NULL, coord_popup, XtNumber(coord_popup), "Coordinates"},
  {"Range Change", range_change, NULL, NULL, 0, NULL},
  {"Axis - Grid", NULL, NULL, axisgrid_popup, XtNumber(axisgrid_popup), "axis - grid"},
  {"Change Display Points", NULL, NULL, show_points_popup, XtNumber(show_points_popup), "Change Display Points"}
};

static MenuStruct first_popup[]={
  {"Menu", NULL, NULL, second_popup, XtNumber(second_popup), "menu"}
};

