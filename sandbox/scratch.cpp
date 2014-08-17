void dumpProperties(Window win) {
    int nprops;
    Atom *atoms = XListProperties(display, win, &nprops);
    dbf("Properties of window %lx:", win);
    for(int i = 0; i < nprops; i++) {
	dbf("  atom[%d] = %lu:", i, atoms[i]);
	char *name = XGetAtomName(display, atoms[i]);
	dbf("    name=%s", name);
	XFree(name);

	Atom actual_type;
	int actual_format;
	unsigned long nitems;
	unsigned long bytes;
	unsigned char *data;

	XGetWindowProperty(display,
			   win,
			   atoms[i],
			   0,
			   ~(0L),
			   false,
			   AnyPropertyType,
			   &actual_type,
			   &actual_format,
			   &nitems,
			   &bytes,
			   &data);

	prf("    actual_type=%lu", actual_type);
	prf("    actual_format=%d", actual_format);
	prf("    nitems=%lu", nitems);
	prf("    bytes=%lu", bytes);
	if((actual_format == 8) && (nitems < 64)) {
	    char buf[nitems + 1];
	    memcpy(buf, data, nitems);
	    buf[nitems] = 0;
	    prf("    data='%s'", data);
	} else if((actual_format == 32) && (nitems == 1)) {
	    prf("    data=%d", *((int*)data));
	}

	XFree(data);
    }
    XFree(atoms);
}

void dumpAncestorProperties(Window win) {
    prf("--- DUMPING %lx ---", win);
    while(win) {
	dumpProperties(win);
	win = getParent(win);
    }
    prf("----------------------------");
}
