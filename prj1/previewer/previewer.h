#ifndef PREVIEWER_H
#define PREVIEWER_H

#include "driver.h"

/*
 * Run the rendering and periodically update a live preview
 * of the rendered image
 * returns false if any errors were encountered or rendering was
 * aborted for some reason
 */
bool render_with_preview(Driver &driver);

#endif

