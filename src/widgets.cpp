#include <cctype>
#include <cmath>
#include <cstdio>
#include <algorithm>

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

#include "animation.h"
#include "animation_set.h"
#include "bitmap.h"
#include "config.h"
#include "engine.h"
#include "frame.h"
#include "game_specific_globals.h"
#include "general.h"
#include "general_types.h"
#include "graphics.h"
#include "music.h"
#include "resource_manager.h"
#include "shaders.h"
#include "widgets.h"
#include "wrap.h"

const float W_Scrolling_List::DECELLERATION = 1;

bool W_Button::acceptsFocus()
{
	return true;
}

void W_Button::keyDown(int keycode)
{
	if (this == tgui::getFocussedWidget()) {
		if (keycode == ALLEGRO_KEY_ENTER || keycode == cfg.key_ability[3]) {
			if (enabled) {
				engine->play_sample(sample_name);
				pressed = true;
			}
			else {
				// FIXME:
				//engine->play_sample("ui_error");
			}
		}
	}
}

void W_Button::joyButtonDown(int button)
{
	if (this == tgui::getFocussedWidget()) {
		if (button == cfg.joy_ability[3]) {
			if (enabled) {
				engine->play_sample(sample_name);
				pressed = true;
			}
			else {
				// FIXME:
				//engine->play_sample("ui_error");
			}
		}
	}
}

void W_Button::mouseDown(int rel_x, int rel_y, int abs_x, int abs_y, int mb)
{
	if (rel_x >= 0 && rel_y >= 0) {
		if (enabled) {
			engine->play_sample(sample_name);
			pressed = true;
		}
		else {
			// FIXME
			//engine->play_sample("ui_error");
		}
	}
}

void W_Button::draw(int abs_x, int abs_y)
{
	if (enabled && image) {
		Graphics::quick_draw(image->bitmap, abs_x+offset.x, abs_y+offset.y, flags);
		General::draw_text(text, text_color, abs_x+offset.x+al_get_bitmap_width(image->bitmap)/2, abs_y+offset.y+al_get_bitmap_height(image->bitmap)/2-General::get_font_line_height(General::FONT_LIGHT)/2, ALLEGRO_ALIGN_CENTER);
	}
	else if (!enabled && disabled_image) {
		Graphics::quick_draw(disabled_image->bitmap, abs_x+offset.x, abs_y+offset.y, flags);
	}
	else if (enabled) {
		General::draw_text(text, text_color, abs_x+offset.x+2, abs_y+offset.y+text_yoffset, 0);
	}
}

tgui::TGUIWidget *W_Button::update()
{
	if (pressed) {
		pressed = false;
		return this;
	}

	return NULL;
}

void W_Button::setSize(int w, int h)
{
	width = w;
	height = h;
}

Wrap::Bitmap *W_Button::getImage()
{
	return image;
}

void W_Button::setImageFlags(int f)
{
	flags = f;
}

void W_Button::set_sample_name(std::string name)
{
	sample_name = name;
}

void W_Button::set_enabled(bool enabled)
{
	this->enabled = enabled;
}

void W_Button::set_text_color(ALLEGRO_COLOR text_color)
{
	this->text_color = text_color;
}

W_Button::W_Button(int x, int y, int width, int height) :
	filename(""),
	pressed(false),
	flags(0),
	enabled(true)
{
	image = NULL;
	disabled_image = NULL;
	this->x = x;
	this->y = y;
	this->width = width;
	this->height = height;
	sample_name = "sfx/menu_select.ogg";
	this->text_color = al_map_rgb(0xd3, 0xd3, 0xd3);
	text_yoffset = 2;
}

W_Button::W_Button(std::string filename) :
	filename(filename),
	pressed(false),
	flags(0),
	enabled(true)
{
	image = NULL;
	disabled_image = NULL;
	x = 0;
	y = 0;
	width = 0;
	height = 0;
	sample_name = "sfx/menu_select.ogg";
	text_yoffset = 2;

	if (filename != "") {
		image = resource_manager->reference_bitmap(filename);
		width = al_get_bitmap_width(image->bitmap);
		height = al_get_bitmap_height(image->bitmap);
	}

	this->text_color = al_map_rgb(0xd3, 0xd3, 0xd3);
}

W_Button::W_Button(std::string filename, std::string text) :
	filename(filename),
	pressed(false),
	flags(0),
	enabled(true),
	text(text)
{
	image = NULL;
	disabled_image = NULL;
	x = 0;
	y = 0;
	width = 0;
	height = 0;
	sample_name = "sfx/menu_select.ogg";
	text_yoffset = 2;

	if (filename != "") {
		image = resource_manager->reference_bitmap(filename);
		width = al_get_bitmap_width(image->bitmap);
		height = al_get_bitmap_height(image->bitmap);
	}
	else {
		width = General::get_text_width(General::FONT_LIGHT, text) + 4;
		height = General::get_font_line_height(General::FONT_LIGHT) + 4;
	}

	this->text_color = al_map_rgb(0xd3, 0xd3, 0xd3);
}

W_Button::~W_Button()
{
	if (filename != "") {
		resource_manager->release_bitmap(filename);
	}
}

void W_Title_Screen_Button::losingFocus()
{
	engine->play_sample("sfx/menu_select.ogg", 1.0f, 0.0f, 1.0f);
}

void W_Title_Screen_Button::draw(int abs_x, int abs_y)
{
	width = General::get_text_width(General::FONT_HEAVY, t(text.c_str())) + 8;
	General::draw_text(t(text.c_str()), abs_x + 4, abs_y + 2, 0, General::FONT_HEAVY);
}

W_Title_Screen_Button::W_Title_Screen_Button(std::string text) :
	W_Button("", text)
{
	//drawFocus = false;
	width = General::get_text_width(General::FONT_HEAVY, t(text.c_str())) + 8;
	height = General::get_font_line_height(General::FONT_HEAVY) + 4;
	sample_name = "sfx/use_item.ogg";
}

W_Title_Screen_Button::~W_Title_Screen_Button()
{
}

void W_Equipment_List::use_button(int button)
{
	int line_height = General::get_font_line_height(font);

	if (button == A) {
		if (activated) {
			float yyy = y - y_offset + selected * line_height;
			if (active_column == 1) {
				if (draw_info_box == false) {
					draw_info_box = true;
					info_box_x = x+offset.x+width-2-19 + al_get_bitmap_width(info_bmp->bitmap)/2;
					info_box_y = y - y_offset + selected * line_height + line_height / 2;
					info_box_text = descriptions[selected];
				}
			}
			else if (active_column == 2) {
				right_icon_clicked = true;
				if (notifier) {
					(*notifier)(notifier_data, yyy);
				}
			}
			else if (!disabled[selected]) {
				right_icon_clicked = false;
				if (notifier) {
					(*notifier)(notifier_data, yyy);
				}
			}
		}
		else if (item_names.size() > 0) {
			bool can_activate = false;
			int first = y_offset / line_height;
			if (first < 0) first = 0;
			if (first >= (int)item_names.size()) first = item_names.size() - 1;
			for (size_t i = first; i < item_names.size(); i++) {
				if (item_names[i] != "") {
					can_activate = true;
					first = i;
					break;
				}
			}
			if (can_activate) {
				activated = true;
				drawFocus = false;
				selected = first;
				active_column = 0;
				int yoffs = y_offset;
				y_offset -= yoffs % line_height;
				if (synced_widget) {
					if (max_y_offset == 0) {
						y_offset = 0;
						max_y_offset = 0;
						synced_widget->set_value(0);
					}
					else {
						synced_widget->set_value(MAX(0.0, MIN(1.0, (float)y_offset/max_y_offset)));
					}
				}
			}
		}
	}
	else if (button == B && activated) {
		deactivate();
	}
	else if (activated) {
		int top = y_offset / line_height;
		int num = height / line_height;
		if (button == LEFT) {
			if (active_column > 0) {
				active_column--;
				if (active_column == 1 && descriptions[selected] == "") {
					active_column = 0;
				}
			}
		}
		else if (button == RIGHT) {
			if (active_column < 2) {
				active_column++;
				if (active_column == 1 && descriptions[selected] == "") {
					active_column = 2;
				}
			}
		}
		else if (button == UP) {
			if (selected > 0) {
				int first = selected;
				for (int i = selected-1; i >= 0; i--) {
					if (item_names[i] != "") {
						first = i;
						break;
					}
				}
				selected = first;
				if (top > selected) {
					y_offset -= line_height;
				}
				if (synced_widget) {
					if (max_y_offset == 0) {
						y_offset = 0;
						max_y_offset = 0;
						synced_widget->set_value(0);
					}
					else {
						synced_widget->set_value(MAX(0.0, MIN(1.0, (float)y_offset/max_y_offset)));
					}
				}
				if (active_column == 1 && descriptions[selected] == "") {
					active_column = 0;
				}
			}
		}
		else if (button == DOWN) {
			if (selected < (int)item_names.size()-1) {
				int first = selected;
				for (size_t i = selected+1; i < item_names.size(); i++) {
					if (item_names[i] != "") {
						first = i;
						break;
					}
				}
				selected = first;
				if (top+num <= selected) {
					y_offset += line_height;
				}
				if (synced_widget) {
					if (max_y_offset == 0) {
						y_offset = 0;
						max_y_offset = 0;
						synced_widget->set_value(0);
					}
					else {
						synced_widget->set_value(MAX(0.0, MIN(1.0, (float)y_offset/max_y_offset)));
					}
				}
				if (active_column == 1 && descriptions[selected] == "") {
					active_column = 0;
				}
			}
		}
	}
}

bool W_Equipment_List::keyChar(int keycode, int unichar)
{
	bool dib = draw_info_box;
	bool ret = W_Scrolling_List::keyChar(keycode, unichar);
	draw_info_box = (dib == true) ? false : draw_info_box;
	return ret;
}

void W_Equipment_List::joyButtonDown(int button)
{
	bool dib = draw_info_box;
	W_Scrolling_List::joyButtonDown(button);
	draw_info_box = (dib == true) ? false : draw_info_box;
}

bool W_Equipment_List::joyAxisRepeat(int stick, int axis, float value)
{
	bool dib = draw_info_box;
	bool ret = W_Scrolling_List::joyAxisRepeat(stick, axis, value);
	draw_info_box = (dib == true) ? false : draw_info_box;
	return ret;
}

void W_Equipment_List::update_description(int index, std::string desc)
{
	descriptions[index] = desc;
}

void W_Equipment_List::set_active_column(int c)
{
	active_column = c;
}

void W_Equipment_List::deactivate()
{
	activated = false;
	drawFocus = true;
}

void W_Equipment_List::mouseDown(int rel_x, int rel_y, int abs_x, int abs_y, int mb)
{
	if (activated) {
		activated = false;
		drawFocus = true;
		draw_info_box = false;
	}

	if (rel_x < 0 || rel_y < 0) {
		vy = 0;
		return;
	}

	int clicked = (rel_y + y_offset) / General::get_font_line_height(font);

	if (clicked >= (int)equipment_type.size()) {
		return;
	}

	int extra = right_icon_filenames.size() > 0 ? 10 : 0;

	if (rel_x > width-11-extra && rel_x < width-2-extra) {
		// info
		if (descriptions[clicked] != "" && item_names[clicked] != "") {
			draw_info_box = true;
			info_box_x = abs_x;
			info_box_y = abs_y;
			info_box_text = descriptions[clicked];
		}
	}
	else {
		W_Scrolling_List::mouseDown(rel_x, rel_y, abs_x, abs_y, mb);
	}
}

void W_Equipment_List::mouseUp(int rel_x, int rel_y, int abs_x, int abs_y, int mb)
{
	draw_info_box = false;
	W_Scrolling_List::mouseUp(rel_x, rel_y, abs_x, abs_y, mb);
}

void W_Equipment_List::mouseMove(int rel_x, int rel_y, int abs_x, int abs_y)
{
	if (draw_info_box) {
		float dx = abs_x - info_box_x;
		float dy = abs_y - info_box_y;
		if (sqrtf(dx*dx + dy*dy) > 5) {
			draw_info_box = false;
		}
	}
	W_Scrolling_List::mouseMove(rel_x, rel_y, abs_x, abs_y);
}

void W_Equipment_List::draw(int abs_x, int abs_y)
{
	bool ttf_was_quick = Graphics::ttf_is_quick();
	Graphics::ttf_quick(false); // all-text hold needs to be flush to apply the clipping rectangle
	//Graphics::ttf_quick(ttf_was_quick);

	int clip_x, clip_y, clip_w, clip_h;
	ALLEGRO_COLOR dark = General::UI_GREEN;
	al_get_clipping_rectangle(&clip_x, &clip_y, &clip_w, &clip_h);
	if (!General::set_clipping_rectangle((x+offset.x), (y+offset.y), width, height)) {
		al_set_clipping_rectangle(clip_x, clip_y, clip_w, clip_h);
		return;
	}

	ALLEGRO_COLOR light = Graphics::change_brightness(dark, 1.35f);
	dark = Graphics::change_brightness(dark, 0.65f);

	int line_height = General::get_font_line_height(font);
	int start = MAX(0, y_offset / line_height);
	int end = MIN((int)item_names.size(), start + height / line_height + 1);

	float yy = y - ((int)y_offset % line_height);
	for (int i = start; i < end; i++) {
		int this_y = yy+(i-start)*line_height;
		if (icons.size() > 0) {
			if (icons[i]) {
				Graphics::quick_draw(icons[i]->bitmap, x+2+offset.x, this_y+offset.y+2, 0);
			}
		}
		if (descriptions[i] != "" && item_names[i] != "") {
			Graphics::quick_draw(info_bmp->bitmap, x+offset.x+width-2-9-(right_icons.size() > 0 ? 10 : 0), this_y+offset.y+2, 0);
		}
		if (right_icons.size() > 0) {
			Graphics::quick_draw(
				right_icons[i]->bitmap,
				x+offset.x+width-2-9,
				this_y+offset.y+2,
				0
			);
		}
	}
	yy = y - ((int)y_offset % line_height);
	int icon_w = 0;
	if (icons.size() > 0) {
		icon_w = 9;
	}

	for (int i = start; i < end; i++) {
		int icons_x = x+offset.x+width-2;
		if (descriptions[i] != "" && item_names[i] != "") {
			icons_x = x+offset.x+width-2-9-(right_icons.size() > 0 ? 10 : 0);
		}
		else if (right_icons.size() > 0) {
			icons_x = x+offset.x+width-2-9;
		}
		int max_w = icons_x - (icon_w+x+3+offset.x);
		max_w -= 2;

		int this_y = yy+(i-start)*line_height;
		ALLEGRO_COLOR color;
		if (disabled[i]) {
			color = dark;
		}
		else {
			color = al_map_rgb(0xff, 0xff, 0xff);
		}
		General::draw_text_width(max_w, t(item_names[i].c_str()), color, icon_w+x+3+offset.x, this_y+offset.y, 0, font);
	}

	//Graphics::ttf_quick(false); // all-text hold needs to be flush to apply the clipping rectangle
	Graphics::ttf_quick(ttf_was_quick);

	if (draw_outline) {
		al_draw_line(offset.x+x+0.5f, offset.y+y+0.5f, offset.x+x+width-0.5f, offset.y+y+0.5f, dark, 1);
		al_draw_line(offset.x+x+0.5f, offset.y+y+0.5f, offset.x+x+0.5f, offset.y+y+height-0.5f, dark, 1);
		al_draw_line(offset.x+x+0.5f, offset.y+y+height-0.5f, offset.x+x+width-0.5f, offset.y+y+height-0.5f, light, 1);
		al_draw_line(offset.x+x+width-0.5f, offset.y+y+0.5f, offset.x+x+width-0.5f, offset.y+y+height-0.5f, light, 1);
	}

	yy = y - ((int)y_offset % line_height);
	for (int i = start; i < end; i++) {
		int icons_x = x+offset.x+width-2;
		if (descriptions[i] != "" && item_names[i] != "") {
			icons_x = x+offset.x+width-2-9-(right_icons.size() > 0 ? 10 : 0);
		}
		else if (right_icons.size() > 0) {
			icons_x = x+offset.x+width-2-9;
		}
		int max_w = icons_x - (icon_w+x+3+offset.x);
		max_w -= 2;

		int this_y = yy+(i-start)*line_height;
		if (activated && i == selected && active_column == 0) {
			tgui::drawFocusRectangle(icon_w+x+3+offset.x, this_y+offset.y, MIN(max_w, General::get_text_width(General::FONT_LIGHT, t(item_names[i].c_str()))), line_height);
		}
		if (descriptions[i] != "" && item_names[i] != "") {
			if (activated && i == selected && active_column == 1) {
				tgui::drawFocusRectangle(x+offset.x+width-2-19, this_y+offset.y+2, al_get_bitmap_width(info_bmp->bitmap), al_get_bitmap_height(info_bmp->bitmap));
			}
		}
		if (right_icons.size() > 0) {
			if (activated && i == selected && active_column == 2) {
				tgui::drawFocusRectangle(x+offset.x+width-2-9, this_y+offset.y+2, al_get_bitmap_width(info_bmp->bitmap), al_get_bitmap_height(info_bmp->bitmap));
			}
		}
	}

	al_set_clipping_rectangle(clip_x, clip_y, clip_w, clip_h);
}

void W_Equipment_List::postDraw(int abs_x, int abs_y)
{
	if (draw_info_box) {
		Graphics::draw_info_box(info_box_x, info_box_y, 100, 40, info_box_text);
	}
}

void W_Equipment_List::insert_item(int pos, std::string icon_filename, std::string text, std::string right_icon_filename, Equipment::Equipment_Type equip_type, std::string desc, bool disabled)
{
	Wrap::Bitmap *icon = resource_manager->reference_bitmap(icon_filename);
	icons.insert(icons.begin()+pos, icon);
	icon_filenames.insert(icon_filenames.begin()+pos, icon_filename);
	item_names.insert(item_names.begin()+pos, text);
	if (right_icon_filenames.size() > 0 || right_icon_filename != "") {
		Wrap::Bitmap *right_icon = resource_manager->reference_bitmap(right_icon_filename);
		right_icons.insert(right_icons.begin()+pos, right_icon);
		right_icon_filenames.insert(right_icon_filenames.begin()+pos, right_icon_filename);
	}
	equipment_type.insert(equipment_type.begin()+pos, equip_type);
	descriptions.insert(descriptions.begin()+pos, desc);
	this->disabled.insert(this->disabled.begin()+pos, disabled);
	setHeight(height);
	if (synced_widget) {
		if (max_y_offset == 0) {
			y_offset = 0;
			max_y_offset = 0;
			synced_widget->set_value(0);
		}
		else {
			synced_widget->set_value(MAX(0.0, MIN(1.0, (float)y_offset/max_y_offset)));
		}
	}
}

void W_Equipment_List::remove_item(int index)
{
	item_names.erase(item_names.begin()+index);
	resource_manager->release_bitmap(icon_filenames[index]);
	icon_filenames.erase(icon_filenames.begin()+index);
	icons.erase(icons.begin()+index);
	equipment_type.erase(equipment_type.begin()+index);
	descriptions.erase(descriptions.begin()+index);
	disabled.erase(disabled.begin()+index);
	if ((int)right_icon_filenames.size() > index) {
		if (right_icon_filenames[index] != "") {
			resource_manager->release_bitmap(right_icon_filenames[index]);
		}
		right_icon_filenames.erase(right_icon_filenames.begin()+index);
	}
	if ((int)right_icons.size() > index) {
		right_icons.erase(right_icons.begin()+index);
	}
	setHeight(height);
	if (synced_widget) {
		if (max_y_offset == 0) {
			y_offset = 0;
			max_y_offset = 0;
			synced_widget->set_value(0);
		}
		else {
			synced_widget->set_value(MAX(0.0, MIN(1.0, (float)y_offset/max_y_offset)));
		}
	}
	if (item_names.size() == 0) {
		activated = false;
		drawFocus = true;
	}
}

Equipment::Equipment_Type W_Equipment_List::get_type(int index)
{
	return equipment_type[index];
}

std::string W_Equipment_List::get_item_name(int index)
{
	return item_names[index];
}

void W_Equipment_List::set_draw_outline(bool draw_outline)
{
	this->draw_outline = draw_outline;
}

W_Equipment_List::W_Equipment_List(std::vector<std::string> item_names, std::vector<std::string> icon_filenames, std::vector<Equipment::Equipment_Type> equipment_type, std::vector<std::string> descriptions, std::vector<bool> disabled, General::Font_Type font, bool draw_box) :
	W_Scrolling_List(item_names, icon_filenames, std::vector<std::string>(), disabled, font, draw_box),
	equipment_type(equipment_type),
	descriptions(descriptions),
	draw_info_box(false),
	draw_outline(true)
{
	info_bmp = resource_manager->reference_bitmap("misc_graphics/interface/info_icon.png");
}

W_Equipment_List::~W_Equipment_List()
{
	resource_manager->release_bitmap("misc_graphics/interface/info_icon.png");
}

void W_SaveLoad_Button::draw(int abs_x, int abs_y)
{
	General::draw_speech_window(SPEECH_NORMAL, abs_x, abs_y, width, height, false, al_map_rgb(0xff, 0xff, 0xff), 1.0f);

	int max_w = 0;
	int max_h = 0;
	for (int i = 0; i < 3; i++) {
		int w = al_get_bitmap_width(player_bmps[i]->bitmap);
		int h = al_get_bitmap_height(player_bmps[i]->bitmap);
		if (w > max_w) {
			max_w = w;
		}
		if (h > max_h) {
			max_h = h;
		}
	}

	General::draw_text(General::get_time_string(playtime), abs_x+15, abs_y+height/2-General::get_font_line_height(General::FONT_LIGHT)/2, 0);

	bool use_shader = this != tgui::getFocussedWidget();

	if (use_shader) {
		Wrap::Shader *tinter = Graphics::get_tint_shader();
		Shader::use(tinter);
		al_set_shader_float("color_r", 0.5f);
		al_set_shader_float("color_g", 0.5f);
		al_set_shader_float("color_b", 0.5f);
	}
	std::map<std::string, int> m;
	m["egbert"] = 0;
	m["frogbert"] = 1;
	m["bisou"] = 2;
	for (int i = 0; i < num_players; i++) {
		int w = al_get_bitmap_width(player_bmps[m[players[i]]]->bitmap);
		int h = al_get_bitmap_height(player_bmps[m[players[i]]]->bitmap);
		Graphics::quick_draw(player_bmps[m[players[i]]]->bitmap, abs_x+40+(max_w+2)/2-w/2+(max_w+2)*i, abs_y+height/2+max_h/2-h, 0);
	}
	if (use_shader) {
		Shader::use(NULL);
	}

	General::draw_text(area_name, abs_x+40+(max_w+2)*3+5, abs_y+height/2-General::get_font_line_height(General::FONT_LIGHT)/2, 0);
}

W_SaveLoad_Button::W_SaveLoad_Button(int x, int y, int width, int height, std::vector<std::string> players, double playtime, std::string area_name) :
	W_Button(x, y, width, height),
	playtime(playtime),
	area_name(area_name),
	players(players)
{
	num_players = players.size();

	player_bmps[0] = resource_manager->reference_bitmap("misc_graphics/interface/egbert_normal_icon.png");
	player_bmps[1] = resource_manager->reference_bitmap("misc_graphics/interface/frogbert_normal_icon.png");
	player_bmps[2] = resource_manager->reference_bitmap("misc_graphics/interface/bisou_normal_icon.png");
}

W_SaveLoad_Button::~W_SaveLoad_Button()
{
	resource_manager->release_bitmap("misc_graphics/interface/egbert_normal_icon.png");
	resource_manager->release_bitmap("misc_graphics/interface/frogbert_normal_icon.png");
	resource_manager->release_bitmap("misc_graphics/interface/bisou_normal_icon.png");
}

bool W_Scrolling_List::acceptsFocus()
{
	return true;
}

void W_Scrolling_List::use_button(int button)
{
	int line_height = General::get_font_line_height(font);

	if (button == A) {
		if (activated) {
			float yyy = y - y_offset + selected * line_height;
			bool go = false;
			if (active_column == 1) {
				right_icon_clicked = true;
				go = true;
			}
			else {
				if (!disabled[selected]) {
					right_icon_clicked = false;
					go = true;
				}
			}
			if (go && notifier) {
				(*notifier)(notifier_data, yyy);
			}
		}
		else if (item_names.size() > 0) {
			activated = true;
			drawFocus = false;
			int first = y_offset / line_height;
			if (first < 0) first = 0;
			if (first >= (int)item_names.size()) first = item_names.size() - 1;
			selected = first;
			active_column = 0;
			int yoffs = y_offset;
			y_offset -= yoffs % line_height;
			if (synced_widget) {
				if (max_y_offset == 0) {
					y_offset = 0;
					max_y_offset = 0;
					synced_widget->set_value(0);
				}
				else {
					synced_widget->set_value(MAX(0.0, MIN(1.0, (float)y_offset/max_y_offset)));
				}
			}
		}
	}
	else if (button == B && activated) {
		activated = false;
		drawFocus = true;
	}
	else if (activated) {
		int top = y_offset / line_height;
		int num = height / line_height;
		if (button == LEFT) {
			if (active_column == 1) {
				active_column = 0;
			}
		}
		else if (button == RIGHT) {
			if (active_column == 0 && right_icons.size() > 0 && right_icons[selected] != NULL) {
				active_column = 1;
			}
		}
		else if (button == UP) {
			if (selected > 0) {
				selected--;
				if (top > selected) {
					y_offset -= line_height;
				}
				if (synced_widget) {
					if (max_y_offset == 0) {
						y_offset = 0;
						max_y_offset = 0;
						synced_widget->set_value(0);
					}
					else {
						synced_widget->set_value(MAX(0.0, MIN(1.0, (float)y_offset/max_y_offset)));
					}
				}
			}
		}
		else if (button == DOWN) {
			if (selected < (int)item_names.size()-1) {
				selected++;
				if (top+num <= selected) {
					y_offset += line_height;
				}
				if (synced_widget) {
					if (max_y_offset == 0) {
						y_offset = 0;
						max_y_offset = 0;
						synced_widget->set_value(0);
					}
					else {
						synced_widget->set_value(MAX(0.0, MIN(1.0, (float)y_offset/max_y_offset)));
					}
				}
			}
		}
	}
}

bool W_Scrolling_List::keyChar(int keycode, int unichar)
{
	if (this != tgui::getFocussedWidget()) {
		return false;
	}

	int button = 0;
	if (keycode == ALLEGRO_KEY_LEFT) {
		button = LEFT;
	}
	else if (keycode == ALLEGRO_KEY_RIGHT) {
		button = RIGHT;
	}
	else if (keycode == ALLEGRO_KEY_UP) {
		button = UP;
	}
	else if (keycode == ALLEGRO_KEY_DOWN) {
		button = DOWN;
	}
	else if (keycode == ALLEGRO_KEY_ENTER || keycode == cfg.key_ability[3]) {
		button = A;
	}
	else if (
		keycode == ALLEGRO_KEY_ESCAPE
#if defined ALLEGRO_ANDROID
		|| keycode == ALLEGRO_KEY_BUTTON_B
		|| keycode == ALLEGRO_KEY_BACK
		|| keycode == ALLEGRO_KEY_SELECT
#endif
		) {
		button = B;
	}
	else if (keycode == ALLEGRO_KEY_PGUP && can_arrange && activated) {
		move_up();
	}
	else if (keycode == ALLEGRO_KEY_PGDN && can_arrange && activated) {
		move_down();
	}

	if (button) {
		use_button(button);
	}

	if (activated && (button == LEFT || button == RIGHT || button == UP || button == DOWN)) {
		return true;
	}
	return false;
}

void W_Scrolling_List::joyButtonDown(int button)
{
	if (this != tgui::getFocussedWidget()) {
		return;
	}

	if (button == cfg.joy_ability[3]) {
		use_button(A);
	}
	else if (button == cfg.joy_ability[2]) {
		use_button(B);
	}
	else if (button == cfg.joy_arrange_up && can_arrange && activated) {
		move_up();
	}
	else if (button == cfg.joy_arrange_down && can_arrange && activated) {
		move_down();
	}
}

bool W_Scrolling_List::joyAxisRepeat(int stick, int axis, float value)
{
	if (this != tgui::getFocussedWidget()) {
		return false;
	}

	int button = 0;

	if (axis == 0) {
		if (value <= -0.5f) {
			button = LEFT;
		}
		else if (value >= 0.5f) {
			button = RIGHT;
		}
	}
	else if (axis == 1) {
		if (value <= -0.5f) {
			button = UP;
		}
		else if (value >= 0.5f) {
			button = DOWN;
		}
	}

	if (button) {
		use_button(button);
	}

	if (activated && (button == LEFT || button == RIGHT || button == UP || button == DOWN)) {
		return true;
	}
	return false;
}

int W_Scrolling_List::get_num_items() {
	return item_names.size();
}

void W_Scrolling_List::set_translate_item_names(bool translate)
{
	translate_item_names = translate;
}

void W_Scrolling_List::mouseDown(int rel_x, int rel_y, int abs_x, int abs_y, int mb)
{
	activated = false;
	drawFocus = true;

	vy = 0;
	if (rel_x >= 0 && rel_y >= 0) {
		if (right_icons.size() > 0 && rel_x > width-11) {
			if (notifier) {
				int line_height = General::get_font_line_height(font);
				int yy = abs_y - y;
				int total_offset = y_offset + yy;
				total_offset /= line_height;
				if (total_offset < 0)
					total_offset = 0;
				else if (total_offset >= (int)item_names.size())
					total_offset = item_names.size()-1;
				selected = total_offset;
				float yyy = y - y_offset + selected * line_height;
				right_icon_clicked = true;
				(*notifier)(notifier_data, yyy);
			}
		}
		else {
			can_notify = true;
			mouse_is_down = true;
			real_mouse_down_point = General::Point<int>(abs_x, abs_y);
			mouse_down_point = real_mouse_down_point;
		}
	}
}

void W_Scrolling_List::mouseUp(int rel_x, int rel_y, int abs_x, int abs_y, int mb)
{
	if (rel_x >= 0 && rel_y >= 0) {
		set_velocity();
		if (mouse_is_down) {
			// if not moved > 5 pixels, select
			int dy = abs(abs_y-real_mouse_down_point.y);
			if (dy < 5) {
				int yy = abs_y - y;
				int total_offset = y_offset + yy;
				total_offset /= General::get_font_line_height(font);
				if (total_offset < 0)
					total_offset = 0;
				else if (total_offset >= (int)item_names.size())
					total_offset = item_names.size()-1;
				selected = total_offset;
				if (selected >= 0 && selected < (int)disabled.size()) {
					if (notifier && can_notify && !disabled[selected]) {
						int line_height = General::get_font_line_height(font);
						float yy = y - y_offset + selected * line_height;
						right_icon_clicked = false;
						(*notifier)(notifier_data, yy);
						can_notify = false;
					}
				}
			}
		}
	}
	mouse_is_down = false;
	move_points.clear();
}

void W_Scrolling_List::mouseMove(int rel_x, int rel_y, int abs_x, int abs_y)
{
	if (mouse_is_down) {
		if (rel_x >= 0 && rel_y >= 0) {
			if ((int)move_points.size() >= POINTS_TO_TRACK) {
				move_points.erase(move_points.begin());
			}
			std::pair<double, General::Point<int> > p;
			p.first = al_get_time();
			p.second = General::Point<int>(abs_x, abs_y);
			move_points.push_back(p);
		}
		else {
			if (y_offset < 0) {
				vy = -MAX_VELOCITY;
			}
			else if (y_offset > max_y_offset) {
				vy = MAX_VELOCITY;
			}
			mouse_is_down = false;
			move_points.clear();
		}
	}
}

void W_Scrolling_List::show_selected()
{
	int line_height = General::get_font_line_height(font);
	y_offset = MIN(max_y_offset, line_height*selected);
	y_offset -= (int)y_offset % line_height;
	if (y_offset < 0) {
		y_offset = 0;
	}
	if (synced_widget) {
		if (max_y_offset == 0) {
			y_offset = 0;
			max_y_offset = 0;
			synced_widget->set_value(0);
		}
		else {
			synced_widget->set_value(MAX(0.0, MIN(1.0, (float)y_offset/max_y_offset)));
		}
	}
}

void W_Scrolling_List::draw(int abs_x, int abs_y)
{
	bool ttf_was_quick = Graphics::ttf_is_quick();
	Graphics::ttf_quick(false); // all-text hold needs to be flush to apply the clipping rectangle
	//Graphics::ttf_quick(ttf_was_quick);

	int clip_x, clip_y, clip_w, clip_h;
	ALLEGRO_COLOR dark = General::UI_GREEN;
	al_get_clipping_rectangle(&clip_x, &clip_y, &clip_w, &clip_h);
	if (!General::set_clipping_rectangle((x+offset.x), (y+offset.y), width, height)) {
		al_set_clipping_rectangle(clip_x, clip_y, clip_w, clip_h);
		return;
	}

	ALLEGRO_COLOR light = Graphics::change_brightness(dark, 1.35f);
	dark = Graphics::change_brightness(dark, 0.65f);

	int line_height = General::get_font_line_height(font);
	int start = MAX(0, y_offset / line_height);
	int end = MIN((int)item_names.size(), start + height / line_height + 1);
	float yy = y - ((int)y_offset % line_height);

	int icon_w = 0;
	for (int i = start; i < end; i++) {
		int this_y = yy+(i-start)*line_height;
		ALLEGRO_COLOR color;
		if ((int)i == selected) {
			color = al_map_rgb(0xff, 0xff, 0x00);
		}
		else {
			color = al_map_rgb(0xff, 0xff, 0xff);
		}
		if (icons.size() > 0) {
			icon_w = 9;
			if (icons[i]) {
				ALLEGRO_COLOR c;
				if (!tint_icons) {
					c = al_map_rgb(0xff, 0xff, 0xff);
				}
				else {
					c = color;
				}
				Graphics::quick_draw(icons[i]->bitmap, c, x+2+offset.x, this_y+offset.y+2, 0);
			}
		}
		if (right_icons.size() > 0) {
			Graphics::quick_draw(
				right_icons[i]->bitmap,
				x+offset.x+width-2-9,
				this_y+offset.y+2,
				0
			);
		}
	}
	yy = y - ((int)y_offset % line_height);
	for (int i = start; i < end; i++) {
		int icons_x = x+offset.x+width-2;
		if (right_icons.size() > 0) {
			icons_x = x+offset.x+width-2-9;
		}
		int max_w = icons_x - (icon_w+x+3+offset.x);
		if ((int)right_justified_text.size() > i) {
			max_w -= General::get_text_width(font, right_justified_text[i]);
		}
		max_w -= 2;

		int this_y = yy+(i-start)*line_height;
		ALLEGRO_COLOR color;
		if (disabled[i]) {
			color = dark;
		}
		else if ((int)i == selected) {
			color = al_map_rgb(0xff, 0xff, 0x00);
		}
		else {
			color = al_map_rgb(0xff, 0xff, 0xff);
		}
		if (translate_item_names) {
			General::draw_text_width(max_w, t(item_names[i].c_str()), color, icon_w+x+3+offset.x, this_y+offset.y, 0, font);
		}
		else {
			General::draw_text_width(max_w, item_names[i], color, icon_w+x+3+offset.x, this_y+offset.y, 0, font);
		}
		int right_text_offset;
		if (right_icons.size() > 0) {
			right_text_offset = -10;
		}
		else {
			right_text_offset = 0;
		}
		if ((int)right_justified_text.size() > i) {
			General::draw_text(right_justified_text[i], color, x+offset.x+width-2+right_text_offset, this_y+offset.y, ALLEGRO_ALIGN_RIGHT, font);
		}
	}

	//Graphics::ttf_quick(false); // all-text hold needs to be flush to apply the clipping rectangle
	Graphics::ttf_quick(ttf_was_quick);

	al_draw_line(offset.x+x+0.5f, offset.y+y+0.5f, offset.x+x+width-0.5f, offset.y+y+0.5f, dark, 1);
	al_draw_line(offset.x+x+0.5f, offset.y+y+0.5f, offset.x+x+0.5f, offset.y+y+height-0.5f, dark, 1);
	al_draw_line(offset.x+x+0.5f, offset.y+y+height-0.5f, offset.x+x+width-0.5f, offset.y+y+height-0.5f, light, 1);
	al_draw_line(offset.x+x+width-0.5f, offset.y+y+0.5f, offset.x+x+width-0.5f, offset.y+y+height-0.5f, light, 1);

	yy = y - ((int)y_offset % line_height);
	for (int i = start; i < end; i++) {
		int icons_x = x+offset.x+width-2;
		if (right_icons.size() > 0) {
			icons_x = x+offset.x+width-2-9;
		}
		int max_w = icons_x - (icon_w+x+3+offset.x);
		if ((int)right_justified_text.size() > i) {
			max_w -= General::get_text_width(font, right_justified_text[i]);
		}
		max_w -= 2;

		int this_y = yy+(i-start)*line_height;
		if (activated && selected == i && active_column == 0) {
			//tgui::drawFocusRectangle(icon_w+x+3+offset.x, this_y+offset.y, MIN(max_w, General::get_text_width(General::FONT_LIGHT, item_names[i])), line_height);
			if (translate_item_names) {
				tgui::drawFocusRectangle(icon_w+x+3+offset.x, this_y+offset.y, MIN(max_w, General::get_text_width(General::FONT_LIGHT, t(item_names[i].c_str()))), line_height);
			}
			else {
				tgui::drawFocusRectangle(icon_w+x+3+offset.x, this_y+offset.y, MIN(max_w, General::get_text_width(General::FONT_LIGHT, item_names[i].c_str())), line_height);
			}
		}
		if (right_icons.size() > 0) {
			if (activated && selected == i && active_column == 1) {
				tgui::drawFocusRectangle(x+offset.x+width-2-9, this_y+offset.y+2, al_get_bitmap_width(right_icons[i]->bitmap), al_get_bitmap_height(right_icons[i]->bitmap));
			}
		}
	}

	al_set_clipping_rectangle(clip_x, clip_y, clip_w, clip_h);
}

tgui::TGUIWidget *W_Scrolling_List::update()
{
	if (vy != 0) {
		y_offset += vy*(General::LOGIC_MILLIS/1000.0);
		if (vy < 0) {
			vy += DECELLERATION;
			if (vy > 0)
				vy = 0;
		}
		else {
			vy -= DECELLERATION;
			if (vy < 0)
				vy = 0;
		}
		if (y_offset <= 0) {
			vy = 0;
			y_offset = 0;
		}
		else if (y_offset >= max_y_offset) {
			vy = 0;
			y_offset = max_y_offset;
		}
		if (synced_widget) {
			if (max_y_offset == 0) {
				y_offset = 0;
				max_y_offset = 0;
				synced_widget->set_value(0);
			}
			else {
				synced_widget->set_value(MAX(0.0, MIN(1.0, (float)y_offset/max_y_offset)));
			}
		}
	}
	else if (mouse_is_down && move_points.size() > 0) {
		int end = move_points.size()-1;
		if (move_points[end].second.y != mouse_down_point.y) {
			y_offset -= move_points[end].second.y - mouse_down_point.y;
			mouse_down_point = move_points[end].second;
			if (synced_widget) {
				if (max_y_offset == 0) {
					y_offset = 0;
					max_y_offset = 0;
					synced_widget->set_value(0);
				}
				else {
					synced_widget->set_value(MAX(0.0, MIN(1.0, (float)y_offset/max_y_offset)));
				}
			}
		}
	}
	return NULL;
}

void W_Scrolling_List::set_value(float v) {
	y_offset = max_y_offset * v;
}

int W_Scrolling_List::get_selected() {
	return selected;
}

void W_Scrolling_List::set_selected(int sel) {
	selected = sel;
}

void W_Scrolling_List::setHeight(int h) {
	TGUIWidget::setHeight(h);

	if (General::get_font_line_height(font)*item_names.size() <= height) {
		max_y_offset = 0;
	}
	else {
		max_y_offset = (General::get_font_line_height(font)*item_names.size()) - height + 1;
	}
}

void W_Scrolling_List::setNotifier(W_Notifier n, void *data)
{
	notifier = n;
	notifier_data = data;
}

// returns percent of max height
float W_Scrolling_List::get_scrollbar_tab_size()
{
	int line_height = General::get_font_line_height(General::FONT_LIGHT);
	int total_height = line_height * item_names.size();
	float p = height / (float)total_height;
	if (p > 1.0f) p = 1.0f;
	else if (p < 0.1f) p = 0.1f;
	return p;
}

void W_Scrolling_List::set_tint_icons(bool tint)
{
	tint_icons = tint;
}

void W_Scrolling_List::remove_item(int index)
{
	item_names.erase(item_names.begin()+index);
	resource_manager->release_bitmap(icon_filenames[index]);
	icon_filenames.erase(icon_filenames.begin()+index);
	icons.erase(icons.begin()+index);
	right_justified_text.erase(right_justified_text.begin()+index);
	disabled.erase(disabled.begin()+index);
	if ((int)right_icon_filenames.size() > index) {
		if (right_icon_filenames[index] != "") {
			resource_manager->release_bitmap(right_icon_filenames[index]);
		}
		right_icon_filenames.erase(right_icon_filenames.begin()+index);
	}
	if ((int)right_icons.size() > index) {
		right_icons.erase(right_icons.begin()+index);
	}
	setHeight(height);
	if (synced_widget) {
		if (max_y_offset == 0) {
			y_offset = 0;
			max_y_offset = 0;
			synced_widget->set_value(0);
		}
		else {
			synced_widget->set_value(MAX(0.0, MIN(1.0, (float)y_offset/max_y_offset)));
		}
	}
	if (item_names.size() == 0) {
		activated = false;
		drawFocus = true;
	}
}

void W_Scrolling_List::move(int index, int dir)
{
	if (index == 0 && dir == -1) {
		return;
	}
	if (index == (int)item_names.size()-1 && dir >= 1) {
		return;
	}

	int del = (dir >= 1) ? 0 : 1;

	// FIXME: this only works with items because of these lines:
	Game_Specific_Globals::get_items().insert(Game_Specific_Globals::get_items().begin()+index+dir, Game_Specific_Globals::get_items()[index]);
	Game_Specific_Globals::get_items().erase(Game_Specific_Globals::get_items().begin()+index+del);

	item_names.insert(item_names.begin()+index+dir, item_names[index]);
	item_names.erase(item_names.begin()+index+del);
	icon_filenames.insert(icon_filenames.begin()+index+dir, icon_filenames[index]);
	icon_filenames.erase(icon_filenames.begin()+index+del);
	icons.insert(icons.begin()+index+dir, icons[index]);
	icons.erase(icons.begin()+index+del);
	right_justified_text.insert(right_justified_text.begin()+index+dir, right_justified_text[index]);
	right_justified_text.erase(right_justified_text.begin()+index+del);
	disabled.insert(disabled.begin()+index+dir, disabled[index]);
	disabled.erase(disabled.begin()+index+del);
	if ((int)right_icon_filenames.size() > index) {
		right_icon_filenames.insert(right_icon_filenames.begin()+index+dir, right_icon_filenames[index]);
		right_icon_filenames.erase(right_icon_filenames.begin()+index+del);
	}
	if ((int)right_icons.size() > index) {
		right_icons.insert(right_icons.begin()+index+dir, right_icons[index]);
		right_icons.erase(right_icons.begin()+index+del);
	}

	if (item_images != NULL) {
		std::vector<Wrap::Bitmap *> &v = *item_images;
		v.insert(v.begin() + index + dir, v[index]);
		v.erase(v.begin()+index+del);
	}

	active_column = 0;

	selected += General::sign(dir);
}

void W_Scrolling_List::move_up()
{
	move(selected, -1);
}

void W_Scrolling_List::move_down()
{
	move(selected, 2);
}

void W_Scrolling_List::update_item(int index, bool update_name, std::string name, bool update_icon, std::string icon_filename, bool update_right_justified_text, std::string _right_justified_text)
{
	if (update_name) {
		item_names[index] = name;
	}
	if (update_icon) {
		resource_manager->release_bitmap(icon_filenames[index]);
		icons[index] = resource_manager->reference_bitmap(icon_filename);
		icon_filenames[index] = icon_filename;
	}
	if (update_right_justified_text) {
		right_justified_text[index] = _right_justified_text;
	}
}

std::string W_Scrolling_List::get_item_name(int index)
{
	return item_names[index];
}

void W_Scrolling_List::set_right_icon_filenames(std::vector<std::string> right_icon_filenames)
{
	this->right_icon_filenames = right_icon_filenames;
	for (size_t i = 0; i < right_icon_filenames.size(); i++) {
		Wrap::Bitmap *bmp = resource_manager->reference_bitmap(right_icon_filenames[i]);
		right_icons.push_back(bmp);
	}
}

bool W_Scrolling_List::get_right_icon_clicked()
{
	return right_icon_clicked;
}

bool W_Scrolling_List::is_activated()
{
	return activated;
}

void W_Scrolling_List::set_can_arrange(bool can_arrange)
{
	this->can_arrange = can_arrange;
}

void W_Scrolling_List::set_item_images(std::vector<Wrap::Bitmap *> *images)
{
	item_images = images;
}

W_Scrolling_List::W_Scrolling_List(std::vector<std::string> item_names, std::vector<std::string> icon_filenames, std::vector<std::string> right_justified_text, std::vector<bool> disabled, General::Font_Type font, bool draw_box) :
	selected(0),
	item_names(item_names),
	font(font),
	right_justified_text(right_justified_text),
	y_offset(0),
	vy(0.0),
	mouse_is_down(false),
	mouse_down_point(General::Point<int>(0, 0)),
	draw_box(draw_box),
	tint_icons(true),
	activated(false),
	translate_item_names(false),
	can_arrange(false)
{
	max_y_offset = 0;

	can_notify = false;
	notifier = NULL;

	if (icon_filenames.size() > 0) {
		this->icon_filenames = icon_filenames;
		for (size_t i = 0; i < icon_filenames.size(); i++) {
			Wrap::Bitmap *b;
			if (icon_filenames[i] != "") {
				b = resource_manager->reference_bitmap(icon_filenames[i]);
			}
			else {
				b = NULL;
			}
			icons.push_back(b);
		}
	}

	if (disabled.size() == 0) {
		for (size_t i = 0; i < item_names.size(); i++) {
			disabled.push_back(false);
		}
	}
	this->disabled = disabled;
}

W_Scrolling_List::~W_Scrolling_List() {
	for (size_t i = 0; i < icon_filenames.size(); i++) {
		if (icon_filenames[i] != "") {
			resource_manager->release_bitmap(icon_filenames[i]);
		}
	}
	for (size_t i = 0; i < right_icon_filenames.size(); i++) {
		if (right_icon_filenames[i] != "") {
			resource_manager->release_bitmap(right_icon_filenames[i]);
		}
	}
}

void W_Checkbox::draw(int abs_x, int abs_y)
{
	int sz = General::get_font_line_height(General::FONT_LIGHT) - 4;
	int cx = abs_x + 2;
	int cy = abs_y + 4;

	ALLEGRO_COLOR fore, back;

	tguiWidgetsGetColors(&fore, &back);

	al_draw_filled_rectangle(cx, cy, cx+sz, cy+sz, back);
	al_draw_rectangle(cx+0.5f, cy+0.5f, cx+sz-0.5f, cy+sz-0.5f, fore, 1);
	if (checked) {
		al_draw_line(cx+0.5f, cy+0.5f, cx+sz-0.5f, cy+sz-0.5f, fore, 1);
		al_draw_line(cx+0.5f, cy+sz-0.5f, cx+sz-0.5f, cy+0.5f, fore, 1);
	}

	General::draw_text(text, al_map_rgb(0xd3, 0xd3, 0xd3), abs_x+sz+6, abs_y+2, 0);
}

void W_Checkbox::keyDown(int keycode)
{
	if (this == tgui::getFocussedWidget() && (keycode == cfg.key_ability[3] || keycode == ALLEGRO_KEY_ENTER)) {
		checked = !checked;
	}
}

void W_Checkbox::joyButtonDown(int button)
{
	if (this == tgui::getFocussedWidget()) {
		checked = !checked;
	}
}

W_Checkbox::W_Checkbox(int x, int y, bool checked, std::string text) :
	TGUI_Checkbox(x, y, 0, 0, checked),
	text(text)
{
	int sz = General::get_font_line_height(General::FONT_LIGHT);
	width = sz + General::get_text_width(General::FONT_LIGHT, text) + 4;
	height = sz + 4;
}

bool W_Checkbox::acceptsFocus()
{
	return true;
}

bool W_Translated_Button::acceptsFocus()
{
	return true;
}

void W_Translated_Button::keyDown(int keycode)
{
	if (this == tgui::getFocussedWidget()) {
		if (keycode == ALLEGRO_KEY_ENTER || keycode == cfg.key_ability[3]) {
			engine->play_sample(sample_name);
			pressed = true;
		}
	}
}

void W_Translated_Button::joyButtonDown(int button)
{
	if (this == tgui::getFocussedWidget()) {
		if (button == cfg.joy_ability[3]) {
			engine->play_sample(sample_name);
			pressed = true;
		}
	}
}

void W_Translated_Button::mouseDown(int rel_x, int rel_y, int abs_x, int abs_y, int mb)
{
	if (rel_x >= 0 && rel_y >= 0) {
		engine->play_sample(sample_name);
		pressed = true;
	}
}

void W_Translated_Button::draw(int abs_x, int abs_y)
{
	width = General::get_text_width(General::FONT_LIGHT, t(text.c_str())) + 4;
	General::draw_text(t(text.c_str()), al_map_rgb(0xd3, 0xd3, 0xd3), abs_x+offset.x+2, abs_y+offset.y+2, 0);
}

tgui::TGUIWidget *W_Translated_Button::update()
{
	if (pressed) {
		pressed = false;
		return this;
	}

	return NULL;
}

W_Translated_Button::W_Translated_Button(std::string text) :
	pressed(false),
	text(text)
{
	x = 0;
	y = 0;
	sample_name = "sfx/menu_select.ogg";

	width = General::get_text_width(General::FONT_LIGHT, t(text.c_str())) + 4;
	height = General::get_font_line_height(General::FONT_LIGHT) + 4;
}

W_Translated_Button::~W_Translated_Button()
{
}

void W_Title_Screen_Icon::losingFocus()
{
	engine->play_sample("sfx/menu_select.ogg", 1.0f, 0.0f, 1.0f);
}

W_Title_Screen_Icon::W_Title_Screen_Icon(std::string filename) :
	W_Button(filename)
{
}

W_Title_Screen_Icon::~W_Title_Screen_Icon()
{
}

bool W_Slider::keyChar(int keycode, int unichar)
{
	if (this != tgui::getFocussedWidget()) {
		return false;
	}

	if (keycode == cfg.key_left) {
		pos -= 5.0f / 100;
		if (pos < 0) {
			pos = 0;
		}
		return true;
	}
	else if (keycode == cfg.key_right) {
		pos += 5.0f / 100;
		if (pos > 1) {
			pos = 1;
		}
		return true;
	}

	return false;
}

bool W_Slider::joyAxisRepeat(int stick, int axis, float value)
{
	if (this != tgui::getFocussedWidget()) {
		return false;
	}

	if (axis == 0) {
		if (value < 0) {
			pos -= 5.0f / 100;
			if (pos < 0) {
				pos = 0;
			}
		}
		else {
			pos += 5.0f / 100;
			if (pos > 1) {
				pos = 1;
			}
		}
		return true;
	}

	return false;
}

void W_Slider::draw(int abs_x, int abs_y)
{
	int x1, y1, w, h;
	int lx, ly, lw, lh;

	int MY_TAB_SIZE = 5;
	int MY_HEIGHT = height-4;
	int sy = abs_y + 2;

	if (direction == TGUI_HORIZONTAL) {
		x1 = abs_x + pos*(size-MY_TAB_SIZE);
		y1 = sy;
		w = MY_TAB_SIZE;
		h = MY_HEIGHT;
		lx = MY_TAB_SIZE/2;
		ly = MY_HEIGHT/2;
		lw = size-MY_TAB_SIZE;
		lh = 0;
	}
	else {
		x1 = y1 = w = h = lx = ly = lw = lh = 0;
	}

	al_draw_line(abs_x+lx+0.5, sy+ly+0.5, abs_x+lx+0.5+lw, sy+ly+0.5+lh, al_map_rgb(0x00, 0x00, 0x00), 1);
	al_draw_filled_rounded_rectangle(x1, y1, x1 + w, y1 + h, 3, 3, al_map_rgb(0xff, 0xff, 0x00));
}

bool W_Slider::acceptsFocus()
{
	return true;
}

W_Slider::W_Slider(int x, int y, int size) :
	TGUI_Slider(x, y, size, TGUI_HORIZONTAL)
{
	width = size;
	height = General::get_font_line_height(General::FONT_LIGHT) + 4;
}

void W_Audio_Settings_Button::keyDown(int keycode)
{
	if (this == tgui::getFocussedWidget()) {
		if (keycode == ALLEGRO_KEY_ENTER || keycode == cfg.key_ability[3]) {
			apply();
		}
	}

	W_Button::keyDown(keycode);
}

void W_Audio_Settings_Button::joyButtonDown(int button)
{
	if (this == tgui::getFocussedWidget()) {
		if (button == cfg.joy_ability[3]) {
			apply();
		}
	}

	W_Button::joyButtonDown(button);
}

void W_Audio_Settings_Button::mouseDown(int rel_x, int rel_y, int abs_x, int abs_y, int mb)
{
	if (rel_x >= 0 && rel_y >= 0) {
		if (enabled) {
			apply();
		}
	}

	W_Button::mouseDown(rel_x, rel_y, abs_x, abs_y, mb);
}

void W_Audio_Settings_Button::apply()
{
	cfg.sfx_volume = sfx_slider->getPosition();

	bool enable_music = false;

	if (cfg.music_volume == 0 && music_slider->getPosition() != 0) {
		enable_music = true;
	}

	cfg.music_volume = music_slider->getPosition();

	if (enable_music) {
		cfg.mute = false;
		Music::play("music/title.mid");
	}
	else {
		if (cfg.music_volume == 0) {
			Music::stop();
			cfg.mute = true;
		}
		else {
			Music::set_volume(cfg.music_volume);
		}
	}

	if (cfg.reverb != reverb_checkbox->getChecked()) {
		cfg.reverb = reverb_checkbox->getChecked();
		cfg.loaded_reverb = cfg.reverb;
		if (!cfg.mute) {
			std::string playing = Music::get_playing();
			Music::stop();
			Music::play(playing);
		}
	}
}

W_Audio_Settings_Button::W_Audio_Settings_Button(std::string filename, std::string text) :
	W_Button(filename, text)
{
}

//--

void W_Integer::draw(int abs_x, int abs_y)
{
	std::string s = General::itos(number);
	int s_w = General::get_text_width(General::FONT_LIGHT, s);
	int s_h = General::get_font_line_height(General::FONT_LIGHT);
	General::draw_text(s, color, abs_x-s_w/2, abs_y-s_h/2, 0);
	// this widget is drawn in modal dialogs with a different transformation, thus needs to be flushed for full-text caching
	bool ttf_was_quick = Graphics::ttf_is_quick();
	Graphics::ttf_quick(false);
	Graphics::ttf_quick(ttf_was_quick);
}

//--

void W_Icon::draw(int abs_x, int abs_y)
{
	Graphics::quick_draw(bitmap->bitmap, abs_x, abs_y, 0);
}

W_Icon::W_Icon(Wrap::Bitmap *bitmap) :
	bitmap(bitmap)
{
	width = al_get_bitmap_width(bitmap->bitmap);
	height = al_get_bitmap_height(bitmap->bitmap);
}

//--

void W_FontScale_Icon::draw(int abs_x, int abs_y)
{
	bool was_quick = Graphics::is_quick_on();
	bool korean = cfg.language == "Korean";
	ALLEGRO_TRANSFORM backup, t;
	if (korean) {
		Graphics::quick(false);
		al_copy_transform(&backup, al_get_current_transform());
		al_identity_transform(&t);
		al_use_transform(&t);
	}
	Graphics::quick_draw(bitmap->bitmap, abs_x*General::font_scale, abs_y*General::font_scale, 0);
	if (korean) {
		al_use_transform(&backup);
		Graphics::quick(was_quick);
	}
}

W_FontScale_Icon::W_FontScale_Icon(Wrap::Bitmap *bitmap) :
	W_Icon(bitmap)
{
	width = al_get_bitmap_width(bitmap->bitmap)/General::font_scale;
	height = al_get_bitmap_height(bitmap->bitmap)/General::font_scale;
}

//--

void W_Icon_Button::draw(int abs_x, int abs_y)
{
	W_Button::draw(abs_x, abs_y);

	if (icon) {
		int icon_w = al_get_bitmap_width(icon->bitmap);
		int total_w = icon_w + General::get_text_width(General::FONT_LIGHT, caption) + 2;
		int draw_x = x + width/2 - total_w/2 + offset.x;
		Graphics::quick_draw(
			icon->bitmap,
			draw_x,
			y + height/2 - icon_h/2 + offset.y,
			0
		);
		General::draw_text(
			caption,
			al_map_rgb(0xff, 0xff, 0xff),
			draw_x + icon_w + 2,
			y+height/2-General::get_font_line_height(General::FONT_LIGHT)/2+offset.y,
			ALLEGRO_ALIGN_LEFT,
			General::FONT_LIGHT
		);
	}
	else {
		General::draw_text(
			caption,
			al_map_rgb(0xff, 0xff, 0xff),
			x+width/2+offset.x,
			y+height/2-General::get_font_line_height(General::FONT_LIGHT)/2+offset.y,
			ALLEGRO_ALIGN_CENTER,
			General::FONT_LIGHT
		);
	}
}
	
W_Icon_Button::W_Icon_Button(std::string caption, std::string bg_name, std::string icon_name) :
	W_Button(bg_name),
	caption(caption),
	icon_filename(icon_name)
{
	if (icon_name != "") {
		icon = resource_manager->reference_bitmap(icon_name);
	}
	else {
		icon = NULL;
	}
	
	if (icon) {
		icon_w = al_get_bitmap_width(icon->bitmap);
		icon_h = al_get_bitmap_height(icon->bitmap);
	}
	else {
		icon_w = 0;
		icon_h = 0;
	}
}

W_Icon_Button::~W_Icon_Button()
{
	if (icon_filename != "") {
		resource_manager->release_bitmap(icon_filename);
	}
}

//--

void W_Vertical_Scrollbar::draw(int abs_x, int abs_y)
{
	Wrap::Bitmap *tab_top, *tab_middle, *tab_bottom;
	tab_anim->set_sub_animation("top");
	tab_top = tab_anim->get_current_animation()->get_current_frame()->get_bitmap()->get_bitmap();
	tab_anim->set_sub_animation("middle");
	tab_middle = tab_anim->get_current_animation()->get_current_frame()->get_bitmap()->get_bitmap();
	tab_anim->set_sub_animation("bottom");
	tab_bottom = tab_anim->get_current_animation()->get_current_frame()->get_bitmap()->get_bitmap();
	
	Wrap::Bitmap *trough_top, *trough_middle;
	trough_anim->set_sub_animation("top");
	trough_top = trough_anim->get_current_animation()->get_current_frame()->get_bitmap()->get_bitmap();
	trough_anim->set_sub_animation("middle");
	trough_middle = trough_anim->get_current_animation()->get_current_frame()->get_bitmap()->get_bitmap();
	
	bool was_quick = Graphics::is_quick_on();
	Graphics::quick(true);
	Graphics::quick_draw(trough_top->bitmap, abs_x+offset.x, abs_y+offset.y, 0);
	Graphics::quick_draw(trough_middle->bitmap, 0, 0, WIDTH, 1, abs_x+offset.x, abs_y+al_get_bitmap_height(trough_top->bitmap)+offset.y,
		WIDTH, height-al_get_bitmap_height(trough_top->bitmap)*2, 0);
	Graphics::quick_draw(
		trough_top->bitmap,
		abs_x+offset.x,
		abs_y+height-al_get_bitmap_height(trough_top->bitmap)+offset.y,
		ALLEGRO_FLIP_VERTICAL
	);
	Graphics::quick(was_quick);

	int tab_x = abs_x + WIDTH/2 - al_get_bitmap_width(tab_top->bitmap)/2 + offset.x;
	int tab_y = abs_y + al_get_bitmap_height(trough_top->bitmap) + value * travel + offset.y;
	Graphics::quick(true);
	Graphics::quick_draw(
		tab_top->bitmap,
		tab_x, tab_y,
		0
	);
	Graphics::quick_draw(
		tab_middle->bitmap,
		0, 0, al_get_bitmap_width(tab_middle->bitmap), al_get_bitmap_height(tab_middle->bitmap),
		tab_x, tab_y+al_get_bitmap_height(tab_top->bitmap),
		al_get_bitmap_width(tab_middle->bitmap),
		get_tab_size()-al_get_bitmap_height(tab_top->bitmap)-al_get_bitmap_height(tab_bottom->bitmap),
		0
	);
	Graphics::quick_draw(
		tab_bottom->bitmap,
		tab_x, tab_y+get_tab_size()-al_get_bitmap_height(tab_bottom->bitmap),
		0
	);
	Graphics::quick(was_quick);
}

void W_Vertical_Scrollbar::mouseDown(int rel_x, int rel_y, int abs_x, int abs_y, int mb)
{
	if (rel_x >= 0 && rel_y >= 0) {
		Wrap::Bitmap *bmp;
		trough_anim->set_sub_animation("top");
		bmp = trough_anim->get_current_animation()->get_current_frame()->get_bitmap()->get_bitmap();

		set(rel_y - al_get_bitmap_height(bmp->bitmap)-get_tab_size()/2);
		down_point = General::Point<int>(abs_x, abs_y);
		down = true;
	}
}

void W_Vertical_Scrollbar::setHeight(int h)
{
	TGUIWidget::setHeight(h);

	Wrap::Bitmap *bmp;
	trough_anim->set_sub_animation("top");
	bmp = trough_anim->get_current_animation()->get_current_frame()->get_bitmap()->get_bitmap();
	travel = height - al_get_bitmap_height(bmp->bitmap)*2 - get_tab_size();
}

int W_Vertical_Scrollbar::get_tab_size()
{
	int size = height;

	Wrap::Bitmap *bmp;
	trough_anim->set_sub_animation("top");
	bmp = trough_anim->get_current_animation()->get_current_frame()->get_bitmap()->get_bitmap();
	size -= al_get_bitmap_height(bmp->bitmap);

	trough_anim->set_sub_animation("middle");
	bmp = trough_anim->get_current_animation()->get_current_frame()->get_bitmap()->get_bitmap();
	size -= al_get_bitmap_height(bmp->bitmap);

	size *= f_tab_size;

	if (size < 8) size = 8;
	return size;
}

void W_Vertical_Scrollbar::set(int trough_pos)
{
	Wrap::Bitmap *bmp;
	trough_anim->set_sub_animation("top");
	bmp = trough_anim->get_current_animation()->get_current_frame()->get_bitmap()->get_bitmap();
	int ignore_size = al_get_bitmap_height(bmp->bitmap)*2;
	if (get_tab_size()+ignore_size >= height) {
		value = 0;
	}
	else if (trough_pos >= 0 && trough_pos < travel+get_tab_size()-ignore_size) {
		value = (float)trough_pos / travel;
		if (value < 0)
			value = 0;
		else if (value > 1)
			value = 1;
	}
	else if (trough_pos < 0)
		value = 0;
	else if (trough_pos >= travel+get_tab_size()) {
		value = 1;
	}
	if (synced_widget) {
		synced_widget->set_value(value);
	}
}

W_Vertical_Scrollbar::W_Vertical_Scrollbar(float tab_size) :
	down(false),
	value(0.0f),
	f_tab_size(tab_size)
{
	this->width = WIDTH;

	trough_anim = new Animation_Set();
	trough_anim->load("misc_graphics/interface/vertical_scrollbar_trough");

	tab_anim = new Animation_Set();
	tab_anim->load("misc_graphics/interface/vertical_scrollbar_tab");
}

W_Vertical_Scrollbar::~W_Vertical_Scrollbar()
{
	delete trough_anim;
	delete tab_anim;
}

void W_Vertical_Scrollbar::mouseMove(int rel_x, int rel_y, int abs_x, int abs_y)
{
	if (down) {
		Wrap::Bitmap *bmp;
		trough_anim->set_sub_animation("top");
		bmp = trough_anim->get_current_animation()->get_current_frame()->get_bitmap()->get_bitmap();
		set(abs_y-y-al_get_bitmap_height(bmp->bitmap)-get_tab_size()/2);
	}
}
