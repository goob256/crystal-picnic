#include <cctype>
#include <cstdio>
#include <algorithm>

#include <allegro5/allegro.h>

ALLEGRO_DEBUG_CHANNEL("CrystalPicnic")

#include "config.h"
#include "engine.h"
#include "frame.h"
#include "general.h"
#include "general_types.h"
#include "graphics.h"
#include "snprintf.h"
#include "video_player.h"
#include "wrap.h"

void Video_Player::start()
{
	engine->loaded_video(this);
}

// returns true when video is done
bool Video_Player::draw()
{
	double total_len = total_frames * (1.0/FPS);
	int frame;
	
	if (elapsed >= total_len) {
		frame = total_frames-1;
	}
	else {
		frame = total_frames * (elapsed / total_len);
	}

	Wrap::Bitmap *to_destroy = NULL;

	// Drop passed frames
	while (frames.size() > 0 && frames[0].frame_num < frame) {
		if (to_destroy) {
			Wrap::destroy_bitmap(to_destroy);
		}
		to_destroy = frames[0].bitmap;
		frames.erase(frames.begin());
	}

	if (frames.size() == 0) {
		update(); // load some frames
		if (frames.size() == 0) {
			return false;
		}
		frame = 0;
	}

	Wrap::Bitmap *to_draw;

	if (to_destroy) {
		to_draw = to_destroy;
	}
	else {
		to_draw = frames[0].bitmap;
	}

	Graphics::quick_draw(to_draw->bitmap, 0, 0, al_get_bitmap_width(frames[0].bitmap->bitmap), al_get_bitmap_height(frames[0].bitmap->bitmap), offset.x, offset.y, cfg.screen_w, cfg.screen_h, 0);

	if (to_destroy) {
		Wrap::destroy_bitmap(to_destroy);
	}
	
	if (elapsed >= total_len) {
		return true;
	}

	return false;
}

void Video_Player::update()
{
	elapsed = al_get_time() - start_time;

	if (total_frames_loaded >= total_frames) {
		return;
	}

	int needed = MIN(total_frames-total_frames_loaded, buffer_size_in_frames - (int)frames.size());
	if (needed > 0) {
		get_frames(needed);
	}
}

void Video_Player::set_offset(General::Point<float> offset)
{
	this->offset = offset;
}

void Video_Player::get_frames(int num)
{
	for (int i = 0; i < num; i++) {
		ALLEGRO_STATE state;
		al_store_state(&state, ALLEGRO_STATE_NEW_BITMAP_PARAMETERS);
		al_set_new_bitmap_format(General::noalpha_bmp_format);
		char buf[1000];
		snprintf(buf, 1000, "%s/%06d.png", dirname.c_str(), total_frames_loaded+i);
		Wrap::Bitmap *b = Wrap::load_bitmap(buf);
		Video_Frame frame;
		frame.bitmap = b;
		frame.frame_num = total_frames_loaded + i;
		frames.push_back(frame);
		al_restore_state(&state);
	}

	total_frames_loaded += num;
}

Video_Player::Video_Player(std::string dirname, int buffer_size_in_frames) :
	buffer_size_in_frames(buffer_size_in_frames),
	offset(0, 0)
{
	elapsed = 0.0;

	this->dirname = dirname;

	// Count frames
	total_frames = 0;
	for (; total_frames < 0xffff /* RANDOM */; total_frames++) {
		char filename[1000];
		snprintf(filename, 1000, "%s/%06d.png", this->dirname.c_str(), total_frames);
		if (!General::exists(filename)) {
			break;
		}
	}
	total_frames_loaded = 0;

	ALLEGRO_DEBUG("TOTAL FRAMES = %d\n", total_frames);

	// Request a full buffer of frames to start
	get_frames(MIN(buffer_size_in_frames, total_frames));

	start_time = al_get_time();
}

Video_Player::~Video_Player()
{
	// Cleanup potentially unused frames (due to skipping/lag)
	for (size_t i = 0; i < frames.size(); i++) {
		Wrap::destroy_bitmap(frames[i].bitmap);
	}
}

int Video_Player::get_current_frame_num()
{
	if (frames.size() > 0) {
		return frames[0].frame_num;
	}
	return 0;
}

void Video_Player::reset_elapsed()
{
	elapsed = 0;
}
