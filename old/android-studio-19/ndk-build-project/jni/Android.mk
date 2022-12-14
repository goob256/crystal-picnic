# Copyright (C) 2010 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

ANDROID_DIR := C:/Users/trent/code/android.m3-m2-cp

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := liballegro_monolith-prebuilt
LOCAL_SRC_FILES := $(TARGET_ARCH_ABI)/liballegro_monolith.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE := libbass-prebuilt
LOCAL_SRC_FILES := $(TARGET_ARCH_ABI)/libbass.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE := libbassmidi-prebuilt
LOCAL_SRC_FILES := $(TARGET_ARCH_ABI)/libbassmidi.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)

LOCAL_ALLOW_UNDEFINED_SYMBOLS := false
LOCAL_MODULE    := crystalpicnic
SRC_PATH := ../../../src
LOCAL_SRC_FILES := $(SRC_PATH)/abilities.cpp $(SRC_PATH)/android.cpp $(SRC_PATH)/animation.cpp $(SRC_PATH)/animation_set.cpp $(SRC_PATH)/area_loop.cpp $(SRC_PATH)/area_manager.cpp $(SRC_PATH)/astar.cpp $(SRC_PATH)/astar_character_role.cpp $(SRC_PATH)/atlas.cpp $(SRC_PATH)/atlas_accessor.cpp $(SRC_PATH)/audio_config_loop.cpp $(SRC_PATH)/bass.cpp $(SRC_PATH)/bass_fileprocs.cpp $(SRC_PATH)/battle_ai.cpp $(SRC_PATH)/battle_entity.cpp $(SRC_PATH)/battle_loop.cpp $(SRC_PATH)/battle_pathfind.cpp $(SRC_PATH)/battle_transition_in.cpp $(SRC_PATH)/bitmap.cpp $(SRC_PATH)/bones.cpp $(SRC_PATH)/camera.cpp $(SRC_PATH)/character_map_entity.cpp $(SRC_PATH)/character_role.cpp $(SRC_PATH)/collidable.cpp $(SRC_PATH)/collision_detection.cpp $(SRC_PATH)/config.cpp $(SRC_PATH)/cpa.cpp $(SRC_PATH)/credits_loop.cpp $(SRC_PATH)/crystal_loop.cpp $(SRC_PATH)/crystalpicnic.cpp $(SRC_PATH)/difficulty_loop.cpp $(SRC_PATH)/direct3d.cpp $(SRC_PATH)/dirty.cpp $(SRC_PATH)/enemy_avatar.cpp $(SRC_PATH)/enemy_avatar_wander_character_role.cpp $(SRC_PATH)/engine.cpp $(SRC_PATH)/entity.cpp $(SRC_PATH)/error.cpp $(SRC_PATH)/follow_character_role.cpp $(SRC_PATH)/frame.cpp $(SRC_PATH)/game_specific_globals.cpp $(SRC_PATH)/general.cpp $(SRC_PATH)/graphics.cpp $(SRC_PATH)/input_config_loop.cpp $(SRC_PATH)/language_config_loop.cpp $(SRC_PATH)/lua.cpp $(SRC_PATH)/main.cpp $(SRC_PATH)/main_menu_loop.cpp $(SRC_PATH)/map_entity.cpp $(SRC_PATH)/map_loop.cpp $(SRC_PATH)/mt19937ar.c $(SRC_PATH)/music.cpp $(SRC_PATH)/my_load_bitmap.cpp $(SRC_PATH)/npc.cpp $(SRC_PATH)/particle.cpp $(SRC_PATH)/player.cpp $(SRC_PATH)/resource_manager.cpp $(SRC_PATH)/runner_loop.cpp $(SRC_PATH)/saveload_loop.cpp $(SRC_PATH)/settings_loop.cpp $(SRC_PATH)/shaders.cpp $(SRC_PATH)/shop_loop.cpp $(SRC_PATH)/skeleton.cpp $(SRC_PATH)/sound.cpp $(SRC_PATH)/speech_loop.cpp $(SRC_PATH)/steering.cpp $(SRC_PATH)/triangulate.cpp $(SRC_PATH)/video_config_loop.cpp $(SRC_PATH)/video_player.cpp $(SRC_PATH)/wander_character_role.cpp $(SRC_PATH)/weaponized_entity.cpp $(SRC_PATH)/well512.c $(SRC_PATH)/whack_a_skunk_loop.cpp $(SRC_PATH)/widgets.cpp $(SRC_PATH)/wrap.cpp $(SRC_PATH)/xml.cpp $(SRC_PATH)/../third_party/poly2tri/poly2tri/sweep/advancing_front.cc $(SRC_PATH)/../third_party/poly2tri/poly2tri/sweep/cdt.cc $(SRC_PATH)/../third_party/poly2tri/poly2tri/sweep/sweep.cc $(SRC_PATH)/../third_party/poly2tri/poly2tri/sweep/sweep_context.cc $(SRC_PATH)/../third_party/poly2tri/poly2tri/common/shapes.cc


LOCAL_CFLAGS    := -I$(ANDROID_DIR)/$(TARGET_ARCH_ABI)/include -I../../include -Wall -DALLEGRO_UNSTABLE $(DEMO)

LOCAL_SHARED_LIBRARIES := liballegro_monolith-prebuilt libbass-prebuilt libbassmidi-prebuilt
LOCAL_LDLIBS    := -L$(ANDROID_DIR)/$(TARGET_ARCH_ABI)/lib -L$(LOCAL_PATH)/$(TARGET_ARCH_ABI) -llog -ltgui2 -llua5.2 -lzstatic -lGLESv1_CM -lGLESv2

include $(BUILD_SHARED_LIBRARY)
