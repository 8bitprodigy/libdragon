#include "gl_internal.h"
#include "gl_rsp_asm.h"

extern gl_state_t state;

#define VTX_SHIFT 5
#define TEX_SHIFT 5

#define DEFINE_SIMPLE_READ_FUNC(name, src_type, convert) \
    static void name(gl_cmd_stream_t *s, const src_type *src, uint32_t count) \
    { \
        for (uint32_t i = 0; i < count; i++) gl_cmd_stream_put_half(s, convert(src[i])); \
    }

#define DEFINE_NORMAL_READ_FUNC(name, src_type, convert) \
    static void name(gl_cmd_stream_t *s, const src_type *src, uint32_t count) \
    { \
        gl_cmd_stream_put_half(s, ((uint8_t)(convert(src[0])) << 8) | (uint8_t)(convert(src[1]))); \
        gl_cmd_stream_put_half(s, (uint8_t)(convert(src[2])) << 8); \
    }

#define VTX_CONVERT_INT(v) ((v) << VTX_SHIFT)
#define VTX_CONVERT_FLT(v) ((v) * (1<<VTX_SHIFT))

DEFINE_SIMPLE_READ_FUNC(vtx_read_u8,  uint8_t,  VTX_CONVERT_INT)
DEFINE_SIMPLE_READ_FUNC(vtx_read_i8,  int8_t,   VTX_CONVERT_INT)
DEFINE_SIMPLE_READ_FUNC(vtx_read_u16, uint16_t, VTX_CONVERT_INT)
DEFINE_SIMPLE_READ_FUNC(vtx_read_i16, int16_t,  VTX_CONVERT_INT)
DEFINE_SIMPLE_READ_FUNC(vtx_read_u32, uint32_t, VTX_CONVERT_INT)
DEFINE_SIMPLE_READ_FUNC(vtx_read_i32, int32_t,  VTX_CONVERT_INT)
DEFINE_SIMPLE_READ_FUNC(vtx_read_f32, float,    VTX_CONVERT_FLT)
DEFINE_SIMPLE_READ_FUNC(vtx_read_f64, double,   VTX_CONVERT_FLT)

#define COL_CONVERT_U8(v) ((v) << 7)
#define COL_CONVERT_I8(v) ((v) << 8)
#define COL_CONVERT_U16(v) ((v) >> 1)
#define COL_CONVERT_I16(v) ((v))
#define COL_CONVERT_U32(v) ((v) >> 17)
#define COL_CONVERT_I32(v) ((v) >> 16)
#define COL_CONVERT_F32(v) (FLOAT_TO_I16(v))
#define COL_CONVERT_F64(v) (FLOAT_TO_I16(v))

DEFINE_SIMPLE_READ_FUNC(col_read_u8,  uint8_t,  COL_CONVERT_U8)
DEFINE_SIMPLE_READ_FUNC(col_read_i8,  int8_t,   COL_CONVERT_I8)
DEFINE_SIMPLE_READ_FUNC(col_read_u16, uint16_t, COL_CONVERT_U16)
DEFINE_SIMPLE_READ_FUNC(col_read_i16, int16_t,  COL_CONVERT_I16)
DEFINE_SIMPLE_READ_FUNC(col_read_u32, uint32_t, COL_CONVERT_U32)
DEFINE_SIMPLE_READ_FUNC(col_read_i32, int32_t,  COL_CONVERT_I32)
DEFINE_SIMPLE_READ_FUNC(col_read_f32, float,    COL_CONVERT_F32)
DEFINE_SIMPLE_READ_FUNC(col_read_f64, double,   COL_CONVERT_F64)

#define TEX_CONVERT_INT(v) ((v) << TEX_SHIFT)
#define TEX_CONVERT_FLT(v) ((v) * (1<<TEX_SHIFT))

DEFINE_SIMPLE_READ_FUNC(tex_read_u8,  uint8_t,  TEX_CONVERT_INT)
DEFINE_SIMPLE_READ_FUNC(tex_read_i8,  int8_t,   TEX_CONVERT_INT)
DEFINE_SIMPLE_READ_FUNC(tex_read_u16, uint16_t, TEX_CONVERT_INT)
DEFINE_SIMPLE_READ_FUNC(tex_read_i16, int16_t,  TEX_CONVERT_INT)
DEFINE_SIMPLE_READ_FUNC(tex_read_u32, uint32_t, TEX_CONVERT_INT)
DEFINE_SIMPLE_READ_FUNC(tex_read_i32, int32_t,  TEX_CONVERT_INT)
DEFINE_SIMPLE_READ_FUNC(tex_read_f32, float,    TEX_CONVERT_FLT)
DEFINE_SIMPLE_READ_FUNC(tex_read_f64, double,   TEX_CONVERT_FLT)

#define NRM_CONVERT_U8(v) ((v) >> 1)
#define NRM_CONVERT_I8(v) ((v))
#define NRM_CONVERT_U16(v) ((v) >> 9)
#define NRM_CONVERT_I16(v) ((v) >> 8)
#define NRM_CONVERT_U32(v) ((v) >> 25)
#define NRM_CONVERT_I32(v) ((v) >> 24)
#define NRM_CONVERT_F32(v) ((v) * 0x7F)
#define NRM_CONVERT_F64(v) ((v) * 0x7F)

DEFINE_NORMAL_READ_FUNC(nrm_read_u8,  uint8_t,  NRM_CONVERT_U8)
DEFINE_NORMAL_READ_FUNC(nrm_read_i8,  int8_t,   NRM_CONVERT_I8)
DEFINE_NORMAL_READ_FUNC(nrm_read_u16, uint16_t, NRM_CONVERT_U16)
DEFINE_NORMAL_READ_FUNC(nrm_read_i16, int16_t,  NRM_CONVERT_I16)
DEFINE_NORMAL_READ_FUNC(nrm_read_u32, uint32_t, NRM_CONVERT_U32)
DEFINE_NORMAL_READ_FUNC(nrm_read_i32, int32_t,  NRM_CONVERT_I32)
DEFINE_NORMAL_READ_FUNC(nrm_read_f32, float,    NRM_CONVERT_F32)
DEFINE_NORMAL_READ_FUNC(nrm_read_f64, double,   NRM_CONVERT_F64)

static void mtx_index_read(gl_cmd_stream_t *s, const void *src, uint32_t count)
{
    // TODO
}

const rsp_read_attrib_func rsp_read_funcs[ATTRIB_COUNT][8] = {
    {
        (rsp_read_attrib_func)vtx_read_i8,
        (rsp_read_attrib_func)vtx_read_u8,
        (rsp_read_attrib_func)vtx_read_i16,
        (rsp_read_attrib_func)vtx_read_u16,
        (rsp_read_attrib_func)vtx_read_i32,
        (rsp_read_attrib_func)vtx_read_u32,
        (rsp_read_attrib_func)vtx_read_f32,
        (rsp_read_attrib_func)vtx_read_f64,
    },
    {
        (rsp_read_attrib_func)col_read_i8,
        (rsp_read_attrib_func)col_read_u8,
        (rsp_read_attrib_func)col_read_i16,
        (rsp_read_attrib_func)col_read_u16,
        (rsp_read_attrib_func)col_read_i32,
        (rsp_read_attrib_func)col_read_u32,
        (rsp_read_attrib_func)col_read_f32,
        (rsp_read_attrib_func)col_read_f64,
    },
    {
        (rsp_read_attrib_func)tex_read_i8,
        (rsp_read_attrib_func)tex_read_u8,
        (rsp_read_attrib_func)tex_read_i16,
        (rsp_read_attrib_func)tex_read_u16,
        (rsp_read_attrib_func)tex_read_i32,
        (rsp_read_attrib_func)tex_read_u32,
        (rsp_read_attrib_func)tex_read_f32,
        (rsp_read_attrib_func)tex_read_f64,
    },
    {
        (rsp_read_attrib_func)nrm_read_i8,
        (rsp_read_attrib_func)nrm_read_u8,
        (rsp_read_attrib_func)nrm_read_i16,
        (rsp_read_attrib_func)nrm_read_u16,
        (rsp_read_attrib_func)nrm_read_i32,
        (rsp_read_attrib_func)nrm_read_u32,
        (rsp_read_attrib_func)nrm_read_f32,
        (rsp_read_attrib_func)nrm_read_f64,
    },
    {
        (rsp_read_attrib_func)mtx_index_read,
        (rsp_read_attrib_func)mtx_index_read,
        (rsp_read_attrib_func)mtx_index_read,
        (rsp_read_attrib_func)mtx_index_read,
        (rsp_read_attrib_func)mtx_index_read,
        (rsp_read_attrib_func)mtx_index_read,
        (rsp_read_attrib_func)mtx_index_read,
        (rsp_read_attrib_func)mtx_index_read,
    },
};

static const gl_array_t dummy_arrays[ATTRIB_COUNT] = {
    { .enabled = true, .size = 4 }
};

typedef enum {
    IMMEDIATE_INDETERMINATE,
    IMMEDIATE_VERTEX,
    IMMEDIATE_ARRAY_ELEMENT,
} immediate_type_t;

static immediate_type_t immediate_type;
static uint32_t vtx_cmd_size;

static void upload_current_attributes(const gl_array_t *arrays)
{
    if (arrays[ATTRIB_COLOR].enabled) {
        gl_set_current_color(state.current_attributes.color);
    }

    if (arrays[ATTRIB_TEXCOORD].enabled) {
        gl_set_current_texcoords(state.current_attributes.texcoord);
    }

    if (arrays[ATTRIB_NORMAL].enabled) {
        gl_set_current_normal(state.current_attributes.normal);
    }

    if (arrays[ATTRIB_MTX_INDEX].enabled) {
        gl_set_current_mtx_index(state.current_attributes.mtx_index);
    }
}

static void load_attribs_at_index(const gl_array_t *arrays, uint32_t index)
{
    gl_fill_all_attrib_defaults(arrays);
    gl_load_attribs(arrays, index);
}

static void load_last_attributes(const gl_array_t *arrays, uint32_t last_index)
{
    load_attribs_at_index(arrays, last_index);
    upload_current_attributes(arrays);
}

static void glp_set_attrib(gl_array_type_t array_type, const void *value, GLenum type, uint32_t size)
{
    static const glp_command_t cmd_table[] = { GLP_CMD_SET_LONG, GLP_CMD_SET_LONG, GLP_CMD_SET_WORD };
    static const uint32_t cmd_size_table[] = { 3, 3, 2 };
    static const int16_t default_value_table[][4] = {
        { 0, 0, 0, 0x7FFF },
        { 0, 0, 0, 1 },
        { 0, 0, 0, 0x7FFF }
    };

    uint32_t table_index = array_type - 1;

    gl_cmd_stream_t s = gl_cmd_stream_begin(glp_overlay_id, cmd_table[table_index], cmd_size_table[table_index]);
    gl_cmd_stream_put_half(&s, offsetof(gl_server_state_t, color) + 8 * table_index);
    rsp_read_funcs[array_type][gl_type_to_index(type)](&s, value, size);
    rsp_read_funcs[array_type][gl_type_to_index(GL_SHORT)](&s, default_value_table[table_index], size);
    gl_cmd_stream_end(&s);
}

static void set_attrib(gl_array_type_t array_type, const void *value, GLenum type, uint32_t size)
{
    glp_set_attrib(array_type, value, type, size);
    gl_read_attrib(array_type, value, type, size);
}

static bool check_last_array_element(int32_t *index)
{
    if (state.last_array_element >= 0) {
        *index = state.last_array_element;
        state.last_array_element = -1;
        return true;
    }

    return false;
}

static void require_array_element(const gl_array_t *arrays)
{
    int32_t index;
    if (check_last_array_element(&index)) {
        for (uint32_t i = 0; i < ATTRIB_COUNT; i++)
        {
            const gl_array_t *array = &arrays[i];
            const void *value = gl_get_attrib_element(array, index);
            set_attrib(i, value, array->type, array->size);
        }
    }
}

static inline gl_cmd_stream_t write_vertex_begin(uint32_t cache_index)
{
    gl_cmd_stream_t s = gl_cmd_stream_begin(glp_overlay_id, GLP_CMD_SET_PRIM_VTX, vtx_cmd_size>>2);
    gl_cmd_stream_put_half(&s, cache_index * PRIM_VTX_SIZE);
    return s;
}

static inline void write_vertex_end(gl_cmd_stream_t *s)
{
    gl_cmd_stream_end(s);
}

static void write_vertex_from_arrays(const gl_array_t *arrays, uint32_t index, uint8_t cache_index)
{
    gl_cmd_stream_t s = write_vertex_begin(cache_index);

    for (uint32_t i = 0; i < ATTRIB_COUNT; i++)
    {
        const gl_array_t *array = &arrays[i];
        if (!array->enabled) {
            continue;
        }

        const void *src = gl_get_attrib_element(array, index);
        array->rsp_read_func(&s, src, array->size);
    }

    write_vertex_end(&s);
}

static inline void submit_vertex(uint32_t cache_index)
{
    uint8_t indices[3];
    if (gl_prim_assembly(cache_index, indices))
    {
        glpipe_draw_triangle(indices[0], indices[1], indices[2]);
    }
}

static void draw_vertex_from_arrays(const gl_array_t *arrays, uint32_t id, uint32_t index)
{
    uint8_t cache_index;
    if (gl_get_cache_index(id, &cache_index))
    {
        write_vertex_from_arrays(arrays, index, cache_index);
    }

    submit_vertex(cache_index);
}

static void gl_asm_vtx_loader(const gl_array_t *arrays)
{
    extern uint8_t rsp_gl_pipeline_text_start[];
    const uint32_t offsets_for_default[] = { 0, 8, 0 };

    rspq_write_t w = rspq_write_begin(glp_overlay_id, GLP_CMD_SET_VTX_LOADER, 3 + VTX_LOADER_MAX_COMMANDS);
    rspq_write_arg(&w, PhysicalAddr(rsp_gl_pipeline_text_start) - 0x1000);

    uint32_t pointer = PhysicalAddr(w.pointer);
    bool aligned = (pointer & 0x7) == 0;

    rspq_write_arg(&w, aligned ? pointer + 8 : pointer + 4);

    if (aligned) {
        rspq_write_arg(&w, 0);
    }

    const uint8_t default_reg = 16;
    const uint8_t current_reg = 17;
    const uint8_t cmd_ptr_reg = 20;
    const uint8_t norm_reg = 2;
    const uint8_t dst_vreg_base = 24;

    uint32_t cmd_offset = 0;

    for (uint32_t i = 0; i < ATTRIB_NORMAL; i++)
    {
        const uint32_t dst_vreg = dst_vreg_base + i;
        const gl_array_t *array = &arrays[i];

        if (!array->enabled) {
            rspq_write_arg(&w, rsp_asm_lwc2(VLOAD_DOUBLE, dst_vreg, 0, i-1, current_reg));
        } else {
            uint32_t cmd_size = array->size * 2;
            uint32_t alignment = next_pow2(cmd_size);
            if (cmd_offset & (alignment-1)) {
                rspq_write_arg(&w, rsp_asm_addi(cmd_ptr_reg, cmd_ptr_reg, cmd_offset));
                cmd_offset = 0;
            }

            switch (array->size)
            {
            case 1:
                rspq_write_arg(&w, rsp_asm_lwc2(VLOAD_DOUBLE, dst_vreg, 0, offsets_for_default[i]>>3, default_reg));
                rspq_write_arg(&w, rsp_asm_lwc2(VLOAD_HALF, dst_vreg, 0, cmd_offset>>1, cmd_ptr_reg));
                break;
            case 2:
                rspq_write_arg(&w, rsp_asm_lwc2(VLOAD_LONG, dst_vreg, 0, cmd_offset>>2, cmd_ptr_reg));
                rspq_write_arg(&w, rsp_asm_lwc2(VLOAD_LONG, dst_vreg, 4, (offsets_for_default[i]>>2) + 1, default_reg));
                break;
            case 3:
                rspq_write_arg(&w, rsp_asm_lwc2(VLOAD_DOUBLE, dst_vreg, 0, cmd_offset>>3, cmd_ptr_reg));
                rspq_write_arg(&w, rsp_asm_lwc2(VLOAD_HALF, dst_vreg, 6, (offsets_for_default[i]>>1) + 3, default_reg));
                break;
            case 4:
                rspq_write_arg(&w, rsp_asm_lwc2(VLOAD_DOUBLE, dst_vreg, 0, cmd_offset>>3, cmd_ptr_reg));
                break;
            }

            cmd_offset += cmd_size;
        }
    }

    if (!arrays[ATTRIB_NORMAL].enabled) {
        rspq_write_arg(&w, rsp_asm_lw(norm_reg, 0x18, current_reg));
    } else {
        rspq_write_arg(&w, rsp_asm_lw(norm_reg, cmd_offset, cmd_ptr_reg));
    }

    rspq_write_end(&w);
}

static uint32_t get_vertex_cmd_size(const gl_array_t *arrays)
{
    uint32_t cmd_size = 4;

    for (uint32_t i = 0; i < ATTRIB_NORMAL; i++)
    {
        if (arrays[i].enabled) {
            cmd_size += arrays[i].size * 2;
        }
    }
    if (arrays[ATTRIB_NORMAL].enabled) {
        cmd_size += 4;
    }

    return ROUND_UP(cmd_size, 4);
}

static void gl_update_vertex_cmd_size(const gl_array_t *arrays)
{
    vtx_cmd_size = get_vertex_cmd_size(arrays);

    // TODO: This is dependent on the layout of data structures internal to rspq.
    //       How can we make it more robust?

    extern uint8_t rsp_queue_data_start[];
    extern uint8_t rsp_queue_data_end[0];
    extern uint8_t rsp_gl_pipeline_data_start[];

    uint32_t ovl_data_offset = rsp_queue_data_end - rsp_queue_data_start;
    uint8_t *rsp_gl_pipeline_ovl_header = rsp_gl_pipeline_data_start + ovl_data_offset;

    #define OVL_HEADER_SIZE 8
    #define CMD_DESC_SIZE   2

    uint16_t *cmd_descriptor = (uint16_t*)(rsp_gl_pipeline_ovl_header + OVL_HEADER_SIZE + GLP_CMD_SET_PRIM_VTX*CMD_DESC_SIZE);

    uint16_t patched_cmd_descriptor = (*cmd_descriptor & 0x3FF) | ((vtx_cmd_size & 0xFC) << 8);

    glpipe_set_vtx_cmd_size(patched_cmd_descriptor, cmd_descriptor);
}

static void gl_prepare_vtx_cmd(const gl_array_t *arrays)
{
    gl_asm_vtx_loader(arrays);
    gl_update_vertex_cmd_size(arrays);
}

static void gl_rsp_begin()
{
    glpipe_init();
    state.last_array_element = -1;
    immediate_type = IMMEDIATE_INDETERMINATE;
}

static void gl_rsp_end()
{
    int32_t index;
    if (check_last_array_element(&index)) {
        load_last_attributes(state.array_object->arrays, index);
    }

    if (state.immediate_active) {
        // TODO: Load from arrays
        gl_set_current_color(state.current_attributes.color);
        gl_set_current_texcoords(state.current_attributes.texcoord);
        gl_set_current_normal(state.current_attributes.normal);
        gl_set_current_mtx_index(state.current_attributes.mtx_index);
    }
}

static void gl_rsp_vertex(const void *value, GLenum type, uint32_t size)
{
    if (immediate_type != IMMEDIATE_VERTEX) {
        gl_prepare_vtx_cmd(dummy_arrays);
        immediate_type = IMMEDIATE_VERTEX;
    }

    static const int16_t default_values[] = { 0, 0, 0, 1 };

    uint8_t cache_index;
    if (gl_get_cache_index(next_prim_id(), &cache_index))
    {
        require_array_element(state.array_object->arrays);

        rsp_read_attrib_func read_func = rsp_read_funcs[ATTRIB_VERTEX][gl_type_to_index(type)];

        gl_cmd_stream_t s = write_vertex_begin(cache_index);
        read_func(&s, value, size);
        vtx_read_i16(&s, default_values + size, 4 - size);
        write_vertex_end(&s);
    }

    submit_vertex(cache_index);
}

static void gl_rsp_color(const void *value, GLenum type, uint32_t size)
{
    set_attrib(ATTRIB_COLOR, value, type, size);
}

static void gl_rsp_tex_coord(const void *value, GLenum type, uint32_t size)
{
    set_attrib(ATTRIB_TEXCOORD, value, type, size);
}

static void gl_rsp_normal(const void *value, GLenum type, uint32_t size)
{
    set_attrib(ATTRIB_NORMAL, value, type, size);
}

static void gl_rsp_mtx_index(const void *value, GLenum type, uint32_t size)
{
    //set_attrib(ATTRIB_MTX_INDEX, value, type, size);
}

static void gl_rsp_array_element(uint32_t index)
{
    if (immediate_type != IMMEDIATE_ARRAY_ELEMENT) {
        gl_prepare_vtx_cmd(state.array_object->arrays);
        immediate_type = IMMEDIATE_ARRAY_ELEMENT;
    }

    draw_vertex_from_arrays(state.array_object->arrays, index, index);
    state.last_array_element = index;
}

static void gl_rsp_draw_arrays(uint32_t first, uint32_t count)
{
    if (state.array_object->arrays[ATTRIB_VERTEX].enabled) {
        gl_prepare_vtx_cmd(state.array_object->arrays);
        for (uint32_t i = 0; i < count; i++)
        {
            draw_vertex_from_arrays(state.array_object->arrays, next_prim_id(), first + i);
        }
    }

    load_last_attributes(state.array_object->arrays, first + count - 1);
}

static void gl_rsp_draw_elements(uint32_t count, const void* indices, read_index_func read_index)
{
    gl_fill_all_attrib_defaults(state.array_object->arrays);

    if (state.array_object->arrays[ATTRIB_VERTEX].enabled) {
        gl_prepare_vtx_cmd(state.array_object->arrays);
        for (uint32_t i = 0; i < count; i++)
        {
            uint32_t index = read_index(indices, i);
            draw_vertex_from_arrays(state.array_object->arrays, index, index);
        }
    }

    load_last_attributes(state.array_object->arrays, read_index(indices, count - 1));
}

const gl_pipeline_t gl_rsp_pipeline = (gl_pipeline_t) {
    .begin = gl_rsp_begin,
    .end = gl_rsp_end,
    .vertex = gl_rsp_vertex,
    .color = gl_rsp_color,
    .tex_coord = gl_rsp_tex_coord,
    .normal = gl_rsp_normal,
    .mtx_index = gl_rsp_mtx_index,
    .array_element = gl_rsp_array_element,
    .draw_arrays = gl_rsp_draw_arrays,
    .draw_elements = gl_rsp_draw_elements,
};