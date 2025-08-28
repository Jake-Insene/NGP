/******************************************************/
/*              This file is part of NGP              */
/******************************************************/
/*       Copyright (c) 2024-Present Jake-Insene       */
/*        See the LICENSE in the project root.        */
/******************************************************/
#include "Video/VGU/VGUQueue.h"

#include "Video/VGU/VGU.h"
#include "Video/VGU/VRasterizer.h"

static constexpr Word MinimumWordsPerCommand[256] =
{
    0, // CMD_END
    2, // CMD_DRAW_BUFFER
    1, // CMD_TEXTURE_SET
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    2, // CMD_RECT
    3, // CMD_TRIANGLE
};


void VGUQueue::try_execute()
{
    if (signal == QUEUE_SIGNAL_IDLE)
        return;

    Word* cmd_words = (Word*)Bus::get_physical_addr(cmd_list);

    while (cmd_len)
    {
        Word cmd_w = cmd_words[0];
        cmd_words++;
        cmd_len--;

        GU::Command cmd = GU::Command(cmd_w >> 24);

        if (cmd_len < MinimumWordsPerCommand[cmd])
        {
            state = GU::QUEUE_ERROR_BAD_LEN;
            return;
        }

        switch (cmd)
        {
        case GU::CMD_END:
            cmd_len = 0;
            break;
        case GU::CMD_DRAW_BUFFER:
        {
            VirtualAddress db_address = cmd_w & 0xFF'FFFF;
            db_address <<= 8;

            PhysicalAddress db_physical_address = VGU::get_physical_vram_address(db_address);

            cmd_w = cmd_words[0];
            cmd_words++;
            cmd_len--;

            u16 w = cmd_w & 0xFFF;
            u16 h = (cmd_w >> 12) & 0xFFF;
            GU::TextureFormat db_format = GU::TextureFormat(cmd_w >> 24);

            cmd_w = cmd_words[0];
            cmd_words++;
            cmd_len--;

            u16 x = cmd_w & 0xFFFF;
            u16 y = cmd_w >> 16;

            VRasterizer::set_draw_buffer(db_physical_address, Vector2I(w, h), Vector2I(x, y), db_format);
        }
            break;
        case GU::CMD_TEXTURE_SET:
        {
            VirtualAddress tex_address = cmd_w & 0xFF'FFFF;
            tex_address <<= 8;

            cmd_w = cmd_words[0];
            cmd_words++;
            cmd_len--;

            u16 w = (cmd_w) & 0xFFF;
            u16 h = (cmd_w >> 12) & 0xFFF;
            GU::TextureFormat tex_fmt = GU::TextureFormat((cmd_w >> 24) & 0xF);
            u8 tex_unit = cmd_w >> 28;

            VGU::TMU& texture_unit = VGU::get_state().texture_units[tex_unit];
            texture_unit.texture_address = tex_address;
            texture_unit.cache_texture_address = VGU::get_physical_vram_address(tex_address);
            texture_unit.width = w;
            texture_unit.height = h;
            texture_unit.texture_format = tex_fmt;
        }
        break;
        case GU::CMD_RECT:
        {
            Color rgb = {};
            rgb.rgba = cmd_w & 0xFF'FFFF;
            rgb.a = 0xFF;

            u16 x, y, w, h;
            Word pos = cmd_words[0];
            Word size = cmd_words[1];
            cmd_words += 2;
            cmd_len -= 2;

            x = pos & 0xFFFF;
            y = (pos >> 16);

            w = (size) & 0xFFFF;
            h = (size >> 16);

            VRasterizer::rect(Vector2(x, y), Vector2(w, h), rgb);
        }
        break;
        case GU::CMD_FILL_RECT:
        {
            Color rgb = {};
            rgb.rgba = cmd_w & 0xFF'FFFF;
            rgb.a = 0xFF;

            u16 x, y, w, h;
            Word pos = cmd_words[0];
            Word size = cmd_words[1];
            cmd_words += 2;
            cmd_len -= 2;

            x = pos & 0xFFFF;
            y = (pos >> 16);

            w = (size) & 0xFFFF;
            h = (size >> 16);

            VRasterizer::fill_rect(Vector2(x, y), Vector2(w, h), rgb);
        }
        break;
        case GU::CMD_TRIANGLE:
        {
            Color rgb = {};
            VRasterizer::VertexColor v0 = {};
            VRasterizer::VertexColor v1 = {};
            VRasterizer::VertexColor v2 = {};

            *((Word*)&rgb) = cmd_w & 0xFF'FFFF;
            rgb.a = 0xFF;

            Word vertex0 = cmd_words[0];
            Word vertex1 = cmd_words[1];
            Word vertex2 = cmd_words[2];
            cmd_words += 3;
            cmd_len -= 3;

            v0.position.x = vertex0 & 0xFFFF;
            v0.position.y = (vertex0 >> 16);

            v1.position.x = vertex1 & 0xFFFF;
            v1.position.y = (vertex1 >> 16);

            v2.position.x = vertex2 & 0xFFFF;
            v2.position.y = (vertex2 >> 16);

            v0.color = rgb;
            v1.color = rgb;
            v2.color = rgb;

            VRasterizer::triangle(v0, v1, v2);
        }
            break;
        case GU::CMD_FILL_TRIANGLE:
        {
            Color rgb = {};
            VRasterizer::VertexColor v0 = {};
            VRasterizer::VertexColor v1 = {};
            VRasterizer::VertexColor v2 = {};

            *((Word*)&rgb) = cmd_w & 0xFF'FFFF;
            rgb.a = 0xFF;

            Word vertex0 = cmd_words[0];
            Word vertex1 = cmd_words[1];
            Word vertex2 = cmd_words[2];
            cmd_words += 3;
            cmd_len -= 3;

            v0.position.x = vertex0 & 0xFFFF;
            v0.position.y = (vertex0 >> 16);

            v1.position.x = vertex1 & 0xFFFF;
            v1.position.y = (vertex1 >> 16);

            v2.position.x = vertex2 & 0xFFFF;
            v2.position.y = (vertex2 >> 16);

            v0.color = rgb;
            v1.color.r = 255;
            v1.color.a = 255;
            v2.color.b = 255;
            v2.color.a = 255;

            VRasterizer::fill_triangle(v0, v1, v2);
        }
        break;
        default:
            break;
        }
    }

    VRasterizer::reset_state();

    cmd_list = 0;
    cmd_len = 0;
    signal = QUEUE_SIGNAL_IDLE;
}