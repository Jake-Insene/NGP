/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#pragma once
#include "Video/GUDevice.h"
#include "Video/Math.h"


struct VRasterizer
{
	enum PutPixel
	{
		PP_NONE,
		PP_RGBA8,
	};

	struct VertexColor
	{
		Vector2 position;
		Color color;
	};

	struct RasterizerState
	{
		struct
		{
			PhysicalAddress address;
			Vector2I size;
			Vector2I offset;
			GU::TextureFormat format;
		} draw_buffer;

		PutPixel put_pixel;
	};

	static inline RasterizerState state;

	[[nodiscard]] static RasterizerState& get_state() { return state; }

	static void initialize();
	static void shutdown();

	static void reset_state();

	static void pixel_default(Vector2I, Color) {}
	static void pixel(Vector2I position, Color rgb);

	static void set_draw_buffer(PhysicalAddress address, Vector2I size, 
		Vector2I offset, GU::TextureFormat format);

	static void fill_rect(Vector2 position, Vector2 size, Color color);
	static void fill_triangle(VertexColor v0, VertexColor v1, VertexColor v2);
};