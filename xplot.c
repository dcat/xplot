#include <xcb/xcb.h>
#include <stdio.h>
#include <err.h>

static xcb_connection_t *conn;
static xcb_screen_t *scr;
static uint32_t values[3];

void
line(int x0, int y0, int x1, int y1) {
	xcb_gcontext_t gc;
	xcb_point_t p[2];

	p[0].x = x0;
	p[0].y = y0;
	p[1].x = x1;
	p[1].y = y1;

	gc = xcb_generate_id(conn);
	if (!gc) {
		warnx("could not generate new id");
		return;
	}

	xcb_create_gc(conn, gc, scr->root, XCB_GC_FOREGROUND, values);

	xcb_poly_line(conn, XCB_COORD_MODE_ORIGIN, scr->root, gc, 2, p);
	xcb_flush(conn);
}

int
main(int argc, char **argv) {
	char *argv0;
	char buf[BUFSIZ];
	int x0, y0, x1, y1;


	conn = xcb_connect(NULL, NULL);
	if (xcb_connection_has_error(conn))
		err(1, "xcb connection error");

	scr = xcb_setup_roots_iterator(xcb_get_setup(conn)).data;
	if (scr == NULL)
		err(1, "unable to get screen");

	values[0] = scr->white_pixel;
	values[1] = 1;
	values[2] = XCB_SUBWINDOW_MODE_INCLUDE_INFERIORS;

	while (fgets(buf, BUFSIZ, stdin)) {
		if (buf[0] == '#')
			if (sscanf(buf, "#%x", &values[0]))
				continue;

		sscanf(buf, "%d %d %*s %d %d", &x0, &y0, &x1, &y1);

		if (y1)
			line(x0, y0, x1, y1);
		else
			/* if input is not valid, quit */
			break;

		x0 = x1 = y0 = y1 = 0;
	}

	if (conn != NULL)
		xcb_disconnect(conn);

	return 0;
}

