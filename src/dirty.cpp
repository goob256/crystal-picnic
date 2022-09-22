#include <allegro5/allegro.h>

#ifdef ALLEGRO_ANDROID
extern void _al_opengl_backup_dirty_bitmaps(ALLEGRO_DISPLAY *display, bool flip);
#elif defined ALLEGRO_WINDOWS
#define ASSERT(x)
#include <allegro5/internal/aintern_bitmap.h>
#include <allegro5/internal/aintern_direct3d.h>
extern "C" {
extern void _al_d3d_prepare_bitmaps_for_reset(ALLEGRO_DISPLAY_D3D *disp);
}
#endif

#include "dirty.h"
#include "engine.h"

void backup_dirty_bitmaps()
{
#ifdef ALLEGRO_ANDROID
	if (engine == 0 || engine->get_display() == 0) {
		return;
	}
	engine->stop_timers();
	al_backup_dirty_bitmaps(engine->get_display());
	engine->start_timers();
#elif defined ALLEGRO_WINDOWS
	if (al_get_display_flags(engine->get_display()) & ALLEGRO_OPENGL) {
		return;
	}
	engine->stop_timers();
	al_backup_dirty_bitmaps(engine->get_display());
	engine->start_timers();
#endif
}
