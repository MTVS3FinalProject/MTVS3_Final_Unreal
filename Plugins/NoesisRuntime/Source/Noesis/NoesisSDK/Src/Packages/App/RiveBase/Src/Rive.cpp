////////////////////////////////////////////////////////////////////////////////////////////////////
// NoesisGUI - http://www.noesisengine.com
// Copyright (c) 2013 Noesis Technologies S.L. All Rights Reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////


//#undef NS_LOG_TRACE
//#define NS_LOG_TRACE(...) NS_LOG_(NS_LOG_LEVEL_TRACE, __VA_ARGS__)


#include <NsCore/Noesis.h>
#include <NsCore/Memory.h>
#include <NsCore/Math.h>
#include <NsCore/Atomic.h>
#include <NsCore/Vector.h>
#include <NsGui/CoreApi.h>
#include <NsApp/RiveControl.h>


NS_WARNING_PUSH
NS_MSVC_WARNING_DISABLE(4100 4458 4245 4389 5051 4201 4267 4355 4459 4060 4996 4018)
NS_MSVC_WARNING_DISABLE(4244 4456 4457 4702 4701 4703)
NS_GCC_CLANG_WARNING_DISABLE("-Wfloat-conversion")
NS_GCC_CLANG_WARNING_DISABLE("-Wshadow")
NS_GCC_WARNING_DISABLE("-Wsign-compare")
NS_GCC_WARNING_DISABLE("-Wtype-limits")
NS_GCC_WARNING_DISABLE("-Wclass-memaccess")
NS_GCC_WARNING_DISABLE("-Wsubobject-linkage")
NS_GCC_WARNING_DISABLE("-Wmaybe-uninitialized")
NS_GCC_WARNING_DISABLE("-Wsign-compare")
NS_GCC_WARNING_DISABLE("-Wattributes")
NS_CLANG_WARNING_DISABLE("-Wunused-private-field")
NS_CLANG_WARNING_DISABLE("-Wtautological-unsigned-zero-compare")
NS_CLANG_WARNING_DISABLE("-Wimplicit-int-conversion")
NS_CLANG_WARNING_DISABLE("-Wshorten-64-to-32")
NS_CLANG_WARNING_DISABLE("-Wcomma")
NS_CLANG_WARNING_DISABLE("-Wdocumentation")


#define _RIVE_INTERNAL_

#ifndef NS_USE_RIVE_TEXT
    #define NS_USE_RIVE_TEXT 1
#endif

#if NS_USE_RIVE_TEXT

#if !NS_EXPORT_HARFBUZZ
    #error NS_EXPORT_HARFBUZZ must be enabled for Rive text
#endif

#define WITH_RIVE_TEXT

NS_GUI_CORE_API void* noesis_hb_blob_create_or_fail(const char* data, unsigned int length, int mode,
    void* user_data, void* destroy);
NS_GUI_CORE_API void noesis_hb_blob_destroy(void* blob);
NS_GUI_CORE_API void* noesis_hb_face_create(void* blob, unsigned int index);
NS_GUI_CORE_API void noesis_hb_face_destroy(void* face);
NS_GUI_CORE_API unsigned int noesis_hb_ot_var_get_axis_count(void* face);
NS_GUI_CORE_API unsigned int noesis_hb_ot_var_get_axis_infos(void* face, unsigned int start_offset,
    unsigned int* axes_count, void* axes_array);
NS_GUI_CORE_API void* noesis_hb_font_create(void* face);
NS_GUI_CORE_API void noesis_hb_font_destroy(void* font);
NS_GUI_CORE_API void noesis_hb_ot_font_set_funcs(void* font);
NS_GUI_CORE_API void noesis_hb_font_set_scale(void* font, int x_scale, int y_scale);
NS_GUI_CORE_API bool noesis_hb_font_get_h_extents(void* font, void* extents);
NS_GUI_CORE_API void* noesis_hb_font_get_face(void* font);
NS_GUI_CORE_API void* noesis_hb_font_create_sub_font(void* parent);
NS_GUI_CORE_API void noesis_hb_font_set_variations(void* font, const void* variations,
    unsigned int variations_length);
NS_GUI_CORE_API void noesis_hb_font_get_glyph_shape(void* font, uint32_t glyph, void* dfuncs,
    void* draw_data);
NS_GUI_CORE_API bool noesis_hb_font_get_nominal_glyph(void* font, uint32_t unicode,
    uint32_t* glyph);
NS_GUI_CORE_API void* noesis_hb_draw_funcs_create();
NS_GUI_CORE_API void noesis_hb_draw_funcs_set_move_to_func(void* dfuncs, void* func,
    void* user_data, void* destroy);
NS_GUI_CORE_API void noesis_hb_draw_funcs_set_line_to_func(void* dfuncs, void* func,
    void* user_data, void* destroy);
NS_GUI_CORE_API void noesis_hb_draw_funcs_set_quadratic_to_func(void* dfuncs, void* func,
    void* user_data, void* destroy);
NS_GUI_CORE_API void noesis_hb_draw_funcs_set_cubic_to_func(void* dfuncs, void* func,
    void* user_data, void* destroy);
NS_GUI_CORE_API void noesis_hb_draw_funcs_set_close_path_func(void* dfuncs, void* func,
    void* user_data, void* destroy);
NS_GUI_CORE_API void noesis_hb_draw_funcs_make_immutable(void* dfuncs);
NS_GUI_CORE_API void noesis_hb_draw_funcs_destroy(void* dfuncs);
NS_GUI_CORE_API unsigned int noesis_hb_ot_layout_language_get_feature_tags(void* face,
    uint32_t table_tag, unsigned int script_index, unsigned int language_index,
    unsigned int start_offset, unsigned int* feature_count, uint32_t* feature_tags);
NS_GUI_CORE_API unsigned int noesis_hb_ot_layout_table_get_script_tags(void* face,
    uint32_t table_tag, unsigned int start_offset, unsigned int* script_count,
    uint32_t* script_tags);
NS_GUI_CORE_API unsigned int noesis_hb_ot_layout_script_get_language_tags(void* face,
    uint32_t table_tag, unsigned int script_index, unsigned int start_offset,
    unsigned int* language_count, uint32_t* language_tags);
NS_GUI_CORE_API void* noesis_hb_buffer_create();
NS_GUI_CORE_API void noesis_hb_buffer_add_utf32(void* buffer, const uint32_t* text,
    int text_length, unsigned int item_offset, int item_length);
NS_GUI_CORE_API void noesis_hb_buffer_set_direction(void* buffer, uint32_t direction);
NS_GUI_CORE_API void noesis_hb_buffer_set_script(void* buffer, uint32_t script);
NS_GUI_CORE_API const void* noesis_hb_language_get_default();
NS_GUI_CORE_API void noesis_hb_buffer_set_language(void* buffer, const void* language);
NS_GUI_CORE_API void* noesis_hb_buffer_get_glyph_infos(void* buffer, unsigned int* length);
NS_GUI_CORE_API void* noesis_hb_buffer_get_glyph_positions(void* buffer, unsigned int* length);
NS_GUI_CORE_API void noesis_hb_buffer_destroy(void* buffer);
NS_GUI_CORE_API void* noesis_hb_unicode_funcs_get_default();
NS_GUI_CORE_API uint32_t noesis_hb_unicode_script(void* ufuncs, uint32_t unicode);
NS_GUI_CORE_API uint32_t noesis_hb_unicode_general_category(void* ufuncs, uint32_t unicode);
NS_GUI_CORE_API void noesis_hb_shape(void* font, void* buffer, const void* features,
    unsigned int num_features);

#define HB_TAG(c1,c2,c3,c4) ((hb_tag_t)((((uint32_t)(c1)&0xFF)<<24)|(((uint32_t)(c2)&0xFF)<<16)| \
    (((uint32_t)(c3)&0xFF)<<8)|((uint32_t)(c4)&0xFF)))

#define HB_OT_TAG_GSUB HB_TAG('G','S','U','B')
#define HB_OT_TAG_GPOS HB_TAG('G','P','O','S')
#define HB_TAG_MAX_SIGNED HB_TAG(0x7f,0xff,0xff,0xff)
#define HB_OT_LAYOUT_DEFAULT_LANGUAGE_INDEX	0xFFFFu
#define HB_FEATURE_GLOBAL_START 0
#define HB_FEATURE_GLOBAL_END ((unsigned int) -1)

typedef uint32_t hb_mask_t;
typedef uint32_t hb_tag_t;
typedef uint32_t hb_codepoint_t;
typedef int32_t hb_position_t;
typedef unsigned int hb_ot_name_id_t;

struct hb_face_t;
struct hb_buffer_t;
struct hb_draw_state_t;
struct hb_unicode_funcs_t;
struct hb_ot_var_axis_info_t;

enum hb_memory_mode_t
{
    HB_MEMORY_MODE_DUPLICATE,
    HB_MEMORY_MODE_READONLY,
    HB_MEMORY_MODE_WRITABLE,
    HB_MEMORY_MODE_READONLY_MAY_MAKE_WRITABLE
};

enum hb_ot_var_axis_flags_t
{
    HB_OT_VAR_AXIS_FLAG_HIDDEN = 0x00000001u
};

enum hb_direction_t
{
    HB_DIRECTION_INVALID = 0,
    HB_DIRECTION_LTR = 4,
    HB_DIRECTION_RTL,
    HB_DIRECTION_TTB,
    HB_DIRECTION_BTT
};

enum hb_script_t
{
    HB_SCRIPT_COMMON = HB_TAG('Z', 'y', 'y', 'y'),
    HB_SCRIPT_INHERITED = HB_TAG('Z', 'i', 'n', 'h'),
    HB_SCRIPT_UNKNOWN = HB_TAG('Z', 'z', 'z', 'z')
};

enum hb_unicode_general_category_t
{
  HB_UNICODE_GENERAL_CATEGORY_CONTROL,			/* Cc */
  HB_UNICODE_GENERAL_CATEGORY_FORMAT,			/* Cf */
  HB_UNICODE_GENERAL_CATEGORY_UNASSIGNED,		/* Cn */
  HB_UNICODE_GENERAL_CATEGORY_PRIVATE_USE,		/* Co */
  HB_UNICODE_GENERAL_CATEGORY_SURROGATE,		/* Cs */
  HB_UNICODE_GENERAL_CATEGORY_LOWERCASE_LETTER,		/* Ll */
  HB_UNICODE_GENERAL_CATEGORY_MODIFIER_LETTER,		/* Lm */
  HB_UNICODE_GENERAL_CATEGORY_OTHER_LETTER,		/* Lo */
  HB_UNICODE_GENERAL_CATEGORY_TITLECASE_LETTER,		/* Lt */
  HB_UNICODE_GENERAL_CATEGORY_UPPERCASE_LETTER,		/* Lu */
  HB_UNICODE_GENERAL_CATEGORY_SPACING_MARK,		/* Mc */
  HB_UNICODE_GENERAL_CATEGORY_ENCLOSING_MARK,		/* Me */
  HB_UNICODE_GENERAL_CATEGORY_NON_SPACING_MARK,		/* Mn */
  HB_UNICODE_GENERAL_CATEGORY_DECIMAL_NUMBER,		/* Nd */
  HB_UNICODE_GENERAL_CATEGORY_LETTER_NUMBER,		/* Nl */
  HB_UNICODE_GENERAL_CATEGORY_OTHER_NUMBER,		/* No */
  HB_UNICODE_GENERAL_CATEGORY_CONNECT_PUNCTUATION,	/* Pc */
  HB_UNICODE_GENERAL_CATEGORY_DASH_PUNCTUATION,		/* Pd */
  HB_UNICODE_GENERAL_CATEGORY_CLOSE_PUNCTUATION,	/* Pe */
  HB_UNICODE_GENERAL_CATEGORY_FINAL_PUNCTUATION,	/* Pf */
  HB_UNICODE_GENERAL_CATEGORY_INITIAL_PUNCTUATION,	/* Pi */
  HB_UNICODE_GENERAL_CATEGORY_OTHER_PUNCTUATION,	/* Po */
  HB_UNICODE_GENERAL_CATEGORY_OPEN_PUNCTUATION,		/* Ps */
  HB_UNICODE_GENERAL_CATEGORY_CURRENCY_SYMBOL,		/* Sc */
  HB_UNICODE_GENERAL_CATEGORY_MODIFIER_SYMBOL,		/* Sk */
  HB_UNICODE_GENERAL_CATEGORY_MATH_SYMBOL,		/* Sm */
  HB_UNICODE_GENERAL_CATEGORY_OTHER_SYMBOL,		/* So */
  HB_UNICODE_GENERAL_CATEGORY_LINE_SEPARATOR,		/* Zl */
  HB_UNICODE_GENERAL_CATEGORY_PARAGRAPH_SEPARATOR,	/* Zp */
  HB_UNICODE_GENERAL_CATEGORY_SPACE_SEPARATOR		/* Zs */
};

struct hb_font_extents_t
{
    hb_position_t ascender;
    hb_position_t descender;
    hb_position_t line_gap;
    /*< private >*/
    hb_position_t reserved9;
    hb_position_t reserved8;
    hb_position_t reserved7;
    hb_position_t reserved6;
    hb_position_t reserved5;
    hb_position_t reserved4;
    hb_position_t reserved3;
    hb_position_t reserved2;
    hb_position_t reserved1;
};

struct hb_variation_t
{
    hb_tag_t tag;
    float value;
};

struct hb_ot_var_axis_info_t
{
    unsigned int axis_index;
    hb_tag_t tag;
    hb_ot_name_id_t name_id;
    hb_ot_var_axis_flags_t flags;
    float min_value;
    float default_value;
    float max_value;
    /*< private >*/
    unsigned int reserved;
};

union hb_var_int_t
{
    uint32_t u32;
    int32_t i32;
    uint16_t u16[2];
    int16_t i16[2];
    uint8_t u8[4];
    int8_t i8[4];
};

struct hb_glyph_info_t
{
    hb_codepoint_t codepoint;
    /*< private >*/
    hb_mask_t  mask;
    /*< public >*/
    uint32_t cluster;

    /*< private >*/
    hb_var_int_t var1;
    hb_var_int_t var2;
};

struct hb_glyph_position_t
{
    hb_position_t x_advance;
    hb_position_t y_advance;
    hb_position_t x_offset;
    hb_position_t y_offset;

    /*< private >*/
    hb_var_int_t var;
};

struct hb_feature_t
{
    hb_tag_t tag;
    uint32_t value;
    unsigned int start;
    unsigned int end;
};
#endif

#include "Rive.h"

#include <NsGui/DrawingContext.h>
#include <NsGui/StreamGeometry.h>
#include <NsGui/StreamGeometryContext.h>
#include <NsGui/MatrixTransform.h>
#include <NsGui/MeshGeometry.h>
#include <NsGui/RectangleGeometry.h>
#include <NsGui/BitmapImage.h>
#include <NsGui/SolidColorBrush.h>
#include <NsGui/LinearGradientBrush.h>
#include <NsGui/RadialGradientBrush.h>
#include <NsGui/ImageBrush.h>
#include <NsGui/GradientStopCollection.h>
#include <NsGui/Pen.h>
#include <NsGui/Stream.h>
#include <NsGui/IntegrationAPI.h>
#include <NsDrawing/Color.h>
#include <NsDrawing/SVGPath.h>
#include <NsCore/Log.h>

#include "rive/src/animation/animation_reset.cpp"
#include "rive/src/animation/animation_reset_factory.cpp"
#include "rive/src/animation/animation_state_instance.cpp"
#include "rive/src/animation/animation_state.cpp"
#include "rive/src/animation/blend_animation_1d.cpp"
#include "rive/src/animation/blend_animation_direct.cpp"
#include "rive/src/animation/blend_animation.cpp"
#include "rive/src/animation/blend_state_1d_instance.cpp"
#include "rive/src/animation/blend_state_1d.cpp"
#include "rive/src/animation/blend_state_direct_instance.cpp"
#include "rive/src/animation/blend_state_direct.cpp"
#include "rive/src/animation/blend_state_transition.cpp"
#include "rive/src/animation/blend_state.cpp"
#include "rive/src/animation/cubic_ease_interpolator.cpp"
#include "rive/src/animation/cubic_interpolator_component.cpp"
#include "rive/src/animation/cubic_interpolator_solver.cpp"
#include "rive/src/animation/cubic_interpolator.cpp"
#include "rive/src/animation/cubic_value_interpolator.cpp"
#include "rive/src/animation/elastic_ease.cpp"
#include "rive/src/animation/elastic_interpolator.cpp"
#include "rive/src/animation/interpolating_keyframe.cpp"
#include "rive/src/animation/keyed_object.cpp"
#include "rive/src/animation/keyed_property.cpp"
#include "rive/src/animation/keyframe_bool.cpp"
#include "rive/src/animation/keyframe_callback.cpp"
#include "rive/src/animation/keyframe_color.cpp"
#include "rive/src/animation/keyframe_double.cpp"
#include "rive/src/animation/keyframe_id.cpp"
#include "rive/src/animation/keyframe_interpolator.cpp"
#include "rive/src/animation/keyframe_string.cpp"
#include "rive/src/animation/keyframe_uint.cpp"
#include "rive/src/animation/keyframe.cpp"
#include "rive/src/animation/layer_state.cpp"
#include "rive/src/animation/linear_animation_instance.cpp"
#include "rive/src/animation/linear_animation.cpp"
#include "rive/src/animation/listener_action.cpp"
#include "rive/src/animation/listener_align_target.cpp"
#include "rive/src/animation/listener_bool_change.cpp"
#include "rive/src/animation/listener_fire_event.cpp"
#include "rive/src/animation/listener_input_change.cpp"
#include "rive/src/animation/listener_number_change.cpp"
#include "rive/src/animation/listener_trigger_change.cpp"
#include "rive/src/animation/listener_viewmodel_change.cpp"
#include "rive/src/animation/nested_animation.cpp"
#include "rive/src/animation/nested_bool.cpp"
#include "rive/src/animation/nested_linear_animation.cpp"
#include "rive/src/animation/nested_number.cpp"
#include "rive/src/animation/nested_remap_animation.cpp"
#include "rive/src/animation/nested_simple_animation.cpp"
#include "rive/src/animation/nested_state_machine.cpp"
#include "rive/src/animation/nested_trigger.cpp"
#include "rive/src/animation/state_instance.cpp"
#include "rive/src/animation/state_machine_input_instance.cpp"
#include "rive/src/animation/state_machine_input.cpp"
#include "rive/src/animation/state_machine_instance.cpp"
#include "rive/src/animation/state_machine_layer.cpp"
#include "rive/src/animation/state_machine_listener.cpp"
#include "rive/src/animation/state_machine.cpp"
#include "rive/src/animation/state_transition.cpp"
#include "rive/src/animation/system_state_instance.cpp"
#include "rive/src/animation/transition_bool_condition.cpp"
#include "rive/src/animation/transition_comparator.cpp"
#include "rive/src/animation/transition_condition.cpp"
#include "rive/src/animation/transition_input_condition.cpp"
#include "rive/src/animation/transition_number_condition.cpp"
#include "rive/src/animation/transition_property_artboard_comparator.cpp"
#include "rive/src/animation/transition_property_comparator.cpp"
#include "rive/src/animation/transition_property_viewmodel_comparator.cpp"
#include "rive/src/animation/transition_trigger_condition.cpp"
#include "rive/src/animation/transition_value_boolean_comparator.cpp"
#include "rive/src/animation/transition_value_color_comparator.cpp"
#include "rive/src/animation/transition_value_enum_comparator.cpp"
#include "rive/src/animation/transition_value_number_comparator.cpp"
#include "rive/src/animation/transition_value_string_comparator.cpp"
#include "rive/src/animation/transition_viewmodel_condition.cpp"
#include "rive/src/assets/audio_asset.cpp"
#include "rive/src/assets/file_asset_contents.cpp"
#include "rive/src/assets/file_asset_referencer.cpp"
#include "rive/src/assets/file_asset.cpp"
#include "rive/src/assets/font_asset.cpp"
#include "rive/src/assets/image_asset.cpp"
#include "rive/src/bones/bone.cpp"
#include "rive/src/bones/root_bone.cpp"
#include "rive/src/bones/skin.cpp"
#include "rive/src/bones/skinnable.cpp"
#include "rive/src/bones/tendon.cpp"
#include "rive/src/bones/weight.cpp"
#include "rive/src/constraints/constraint.cpp"
#include "rive/src/constraints/distance_constraint.cpp"
#include "rive/src/constraints/follow_path_constraint.cpp"
#include "rive/src/constraints/ik_constraint.cpp"
#include "rive/src/constraints/rotation_constraint.cpp"
#include "rive/src/constraints/scale_constraint.cpp"
#include "rive/src/constraints/targeted_constraint.cpp"
#include "rive/src/constraints/transform_constraint.cpp"
#include "rive/src/constraints/translation_constraint.cpp"
#include "rive/src/core/field_types/core_bool_type.cpp"
#include "rive/src/core/field_types/core_bytes_type.cpp"
#include "rive/src/core/field_types/core_color_type.cpp"
#include "rive/src/core/field_types/core_double_type.cpp"
#include "rive/src/core/field_types/core_string_type.cpp"
#include "rive/src/core/field_types/core_uint_type.cpp"
#include "rive/src/core/binary_reader.cpp"
#include "rive/src/core/binary_data_reader.cpp"
#include "rive/src/core/binary_writer.cpp"
#include "rive/src/generated/animation/animation_base.cpp"
#include "rive/src/generated/animation/animation_state_base.cpp"
#include "rive/src/generated/animation/any_state_base.cpp"
#include "rive/src/generated/animation/blend_animation_1d_base.cpp"
#include "rive/src/generated/animation/blend_animation_direct_base.cpp"
#include "rive/src/generated/animation/blend_state_1d_base.cpp"
#include "rive/src/generated/animation/blend_state_direct_base.cpp"
#include "rive/src/generated/animation/blend_state_transition_base.cpp"
#include "rive/src/generated/animation/cubic_ease_interpolator_base.cpp"
#include "rive/src/generated/animation/cubic_interpolator_component_base.cpp"
#include "rive/src/generated/animation/cubic_value_interpolator_base.cpp"
#include "rive/src/generated/animation/elastic_interpolator_base.cpp"
#include "rive/src/generated/animation/entry_state_base.cpp"
#include "rive/src/generated/animation/exit_state_base.cpp"
#include "rive/src/generated/animation/keyed_object_base.cpp"
#include "rive/src/generated/animation/keyed_property_base.cpp"
#include "rive/src/generated/animation/keyframe_bool_base.cpp"
#include "rive/src/generated/animation/keyframe_callback_base.cpp"
#include "rive/src/generated/animation/keyframe_color_base.cpp"
#include "rive/src/generated/animation/keyframe_double_base.cpp"
#include "rive/src/generated/animation/keyframe_id_base.cpp"
#include "rive/src/generated/animation/keyframe_string_base.cpp"
#include "rive/src/generated/animation/keyframe_uint_base.cpp"
#include "rive/src/generated/animation/linear_animation_base.cpp"
#include "rive/src/generated/animation/listener_align_target_base.cpp"
#include "rive/src/generated/animation/listener_bool_change_base.cpp"
#include "rive/src/generated/animation/listener_fire_event_base.cpp"
#include "rive/src/generated/animation/listener_number_change_base.cpp"
#include "rive/src/generated/animation/listener_trigger_change_base.cpp"
#include "rive/src/generated/animation/listener_viewmodel_change_base.cpp"
#include "rive/src/generated/animation/nested_bool_base.cpp"
#include "rive/src/generated/animation/nested_number_base.cpp"
#include "rive/src/generated/animation/nested_remap_animation_base.cpp"
#include "rive/src/generated/animation/nested_simple_animation_base.cpp"
#include "rive/src/generated/animation/nested_state_machine_base.cpp"
#include "rive/src/generated/animation/nested_trigger_base.cpp"
#include "rive/src/generated/animation/state_machine_base.cpp"
#include "rive/src/generated/animation/state_machine_bool_base.cpp"
#include "rive/src/generated/animation/state_machine_fire_event_base.cpp"
#include "rive/src/generated/animation/state_machine_fire_event.cpp"
#include "rive/src/generated/animation/state_machine_layer_base.cpp"
#include "rive/src/generated/animation/state_machine_listener_base.cpp"
#include "rive/src/generated/animation/state_machine_number_base.cpp"
#include "rive/src/generated/animation/state_machine_trigger_base.cpp"
#include "rive/src/generated/animation/state_transition_base.cpp"
#include "rive/src/generated/animation/transition_artboard_condition_base.cpp"
#include "rive/src/generated/animation/transition_bool_condition_base.cpp"
#include "rive/src/generated/animation/transition_number_condition_base.cpp"
#include "rive/src/generated/animation/transition_property_artboard_comparator_base.cpp"
#include "rive/src/generated/animation/transition_property_viewmodel_comparator_base.cpp"
#include "rive/src/generated/animation/transition_trigger_condition_base.cpp"
#include "rive/src/generated/animation/transition_value_boolean_comparator_base.cpp"
#include "rive/src/generated/animation/transition_value_color_comparator_base.cpp"
#include "rive/src/generated/animation/transition_value_enum_comparator_base.cpp"
#include "rive/src/generated/animation/transition_value_number_comparator_base.cpp"
#include "rive/src/generated/animation/transition_value_string_comparator_base.cpp"
#include "rive/src/generated/animation/transition_value_trigger_comparator_base.cpp"
#include "rive/src/generated/animation/transition_viewmodel_condition_base.cpp"
#include "rive/src/generated/assets/audio_asset_base.cpp"
#include "rive/src/generated/assets/file_asset_contents_base.cpp"
#include "rive/src/generated/assets/folder_base.cpp"
#include "rive/src/generated/assets/font_asset_base.cpp"
#include "rive/src/generated/assets/image_asset_base.cpp"
#include "rive/src/generated/bones/bone_base.cpp"
#include "rive/src/generated/bones/cubic_weight_base.cpp"
#include "rive/src/generated/bones/root_bone_base.cpp"
#include "rive/src/generated/bones/skin_base.cpp"
#include "rive/src/generated/bones/tendon_base.cpp"
#include "rive/src/generated/bones/weight_base.cpp"
#include "rive/src/generated/constraints/distance_constraint_base.cpp"
#include "rive/src/generated/constraints/follow_path_constraint_base.cpp"
#include "rive/src/generated/constraints/ik_constraint_base.cpp"
#include "rive/src/generated/constraints/rotation_constraint_base.cpp"
#include "rive/src/generated/constraints/scale_constraint_base.cpp"
#include "rive/src/generated/constraints/transform_constraint_base.cpp"
#include "rive/src/generated/constraints/translation_constraint_base.cpp"
#include "rive/src/generated/data_bind/bindable_property_boolean_base.cpp"
#include "rive/src/generated/data_bind/bindable_property_color_base.cpp"
#include "rive/src/generated/data_bind/bindable_property_enum_base.cpp"
#include "rive/src/generated/data_bind/bindable_property_number_base.cpp"
#include "rive/src/generated/data_bind/bindable_property_string_base.cpp"
#include "rive/src/generated/data_bind/bindable_property_trigger_base.cpp"
#include "rive/src/generated/data_bind/data_bind_base.cpp"
#include "rive/src/generated/data_bind/data_bind_context_base.cpp"
#include "rive/src/generated/data_bind/converters/data_converter_group_base.cpp"
#include "rive/src/generated/data_bind/converters/data_converter_group_item_base.cpp"
#include "rive/src/generated/data_bind/converters/data_converter_operation_base.cpp"
#include "rive/src/generated/data_bind/converters/data_converter_operation_value_base.cpp"
#include "rive/src/generated/data_bind/converters/data_converter_operation_viewmodel_base.cpp"
#include "rive/src/generated/data_bind/converters/data_converter_rounder_base.cpp"
#include "rive/src/generated/data_bind/converters/data_converter_system_degs_to_rads_base.cpp"
#include "rive/src/generated/data_bind/converters/data_converter_system_normalizer_base.cpp"
#include "rive/src/generated/data_bind/converters/data_converter_to_string_base.cpp"
#include "rive/src/generated/data_bind/converters/data_converter_trigger_base.cpp"
#include "rive/src/data_bind/data_bind.cpp"
#include "rive/src/data_bind/data_bind_context.cpp"
#include "rive/src/data_bind/data_context.cpp"
#include "rive/src/data_bind/context/context_value.cpp"
#include "rive/src/data_bind/context/context_value_boolean.cpp"
#include "rive/src/data_bind/context/context_value_color.cpp"
#include "rive/src/data_bind/context/context_value_enum.cpp"
#include "rive/src/data_bind/context/context_value_list.cpp"
#include "rive/src/data_bind/context/context_value_list_item.cpp"
#include "rive/src/data_bind/context/context_value_number.cpp"
#include "rive/src/data_bind/context/context_value_string.cpp"
#include "rive/src/data_bind/context/context_value_trigger.cpp"
#include "rive/src/data_bind/converters/data_converter.cpp"
#include "rive/src/data_bind/converters/data_converter_group.cpp"
#include "rive/src/data_bind/converters/data_converter_group_item.cpp"
#include "rive/src/data_bind/converters/data_converter_operation.cpp"
#include "rive/src/data_bind/converters/data_converter_operation_value.cpp"
#include "rive/src/data_bind/converters/data_converter_operation_viewmodel.cpp"
#include "rive/src/data_bind/converters/data_converter_rounder.cpp"
#include "rive/src/data_bind/converters/data_converter_system_degs_to_rads.cpp"
#include "rive/src/data_bind/converters/data_converter_system_normalizer.cpp"
#include "rive/src/data_bind/converters/data_converter_to_string.cpp"
#include "rive/src/data_bind/converters/data_converter_trigger.cpp"
#include "rive/src/generated/viewmodel/data_enum_base.cpp"
#include "rive/src/generated/viewmodel/data_enum_custom_base.cpp"
#include "rive/src/generated/viewmodel/data_enum_system_base.cpp"
#include "rive/src/generated/viewmodel/data_enum_value_base.cpp"
#include "rive/src/generated/viewmodel/viewmodel_base.cpp"
#include "rive/src/generated/viewmodel/viewmodel_component_base.cpp"
#include "rive/src/generated/viewmodel/viewmodel_instance_base.cpp"
#include "rive/src/generated/viewmodel/viewmodel_instance_boolean_base.cpp"
#include "rive/src/generated/viewmodel/viewmodel_instance_color_base.cpp"
#include "rive/src/generated/viewmodel/viewmodel_instance_enum_base.cpp"
#include "rive/src/generated/viewmodel/viewmodel_instance_list_base.cpp"
#include "rive/src/generated/viewmodel/viewmodel_instance_list_item_base.cpp"
#include "rive/src/generated/viewmodel/viewmodel_instance_number_base.cpp"
#include "rive/src/generated/viewmodel/viewmodel_instance_string_base.cpp"
#include "rive/src/generated/viewmodel/viewmodel_instance_trigger_base.cpp"
#include "rive/src/generated/viewmodel/viewmodel_instance_viewmodel_base.cpp"
#include "rive/src/generated/viewmodel/viewmodel_property_base.cpp"
#include "rive/src/generated/viewmodel/viewmodel_property_boolean_base.cpp"
#include "rive/src/generated/viewmodel/viewmodel_property_color_base.cpp"
#include "rive/src/generated/viewmodel/viewmodel_property_enum_base.cpp"
#include "rive/src/generated/viewmodel/viewmodel_property_enum_custom_base.cpp"
#include "rive/src/generated/viewmodel/viewmodel_property_enum_system_base.cpp"
#include "rive/src/generated/viewmodel/viewmodel_property_list_base.cpp"
#include "rive/src/generated/viewmodel/viewmodel_property_number_base.cpp"
#include "rive/src/generated/viewmodel/viewmodel_property_string_base.cpp"
#include "rive/src/generated/viewmodel/viewmodel_property_trigger_base.cpp"
#include "rive/src/generated/viewmodel/viewmodel_property_viewmodel_base.cpp"
#include "rive/src/viewmodel/data_enum.cpp"
#include "rive/src/viewmodel/data_enum_value.cpp"
#include "rive/src/viewmodel/viewmodel.cpp"
#include "rive/src/viewmodel/viewmodel_instance.cpp"
#include "rive/src/viewmodel/viewmodel_instance_boolean.cpp"
#include "rive/src/viewmodel/viewmodel_instance_color.cpp"
#include "rive/src/viewmodel/viewmodel_instance_enum.cpp"
#include "rive/src/viewmodel/viewmodel_instance_list.cpp"
#include "rive/src/viewmodel/viewmodel_instance_list_item.cpp"
#include "rive/src/viewmodel/viewmodel_instance_number.cpp"
#include "rive/src/viewmodel/viewmodel_instance_string.cpp"
#include "rive/src/viewmodel/viewmodel_instance_trigger.cpp"
#include "rive/src/viewmodel/viewmodel_instance_value.cpp"
#include "rive/src/viewmodel/viewmodel_instance_viewmodel.cpp"
#include "rive/src/viewmodel/viewmodel_property.cpp"
#include "rive/src/viewmodel/viewmodel_property_enum.cpp"
#include "rive/src/generated/shapes/paint/dash_base.cpp"
#include "rive/src/generated/shapes/paint/dash_path_base.cpp"
#include "rive/src/generated/shapes/paint/fill_base.cpp"
#include "rive/src/generated/shapes/paint/gradient_stop_base.cpp"
#include "rive/src/generated/shapes/paint/linear_gradient_base.cpp"
#include "rive/src/generated/shapes/paint/radial_gradient_base.cpp"
#include "rive/src/generated/shapes/paint/solid_color_base.cpp"
#include "rive/src/generated/shapes/paint/stroke_base.cpp"
#include "rive/src/generated/shapes/paint/trim_path_base.cpp"
#include "rive/src/generated/shapes/clipping_shape_base.cpp"
#include "rive/src/generated/shapes/contour_mesh_vertex_base.cpp"
#include "rive/src/generated/shapes/cubic_asymmetric_vertex_base.cpp"
#include "rive/src/generated/shapes/cubic_detached_vertex_base.cpp"
#include "rive/src/generated/shapes/cubic_mirrored_vertex_base.cpp"
#include "rive/src/generated/shapes/ellipse_base.cpp"
#include "rive/src/generated/shapes/image_base.cpp"
#include "rive/src/generated/shapes/mesh_base.cpp"
#include "rive/src/generated/shapes/mesh_vertex_base.cpp"
#include "rive/src/generated/shapes/points_path_base.cpp"
#include "rive/src/generated/shapes/polygon_base.cpp"
#include "rive/src/generated/shapes/rectangle_base.cpp"
#include "rive/src/generated/shapes/shape_base.cpp"
#include "rive/src/generated/shapes/star_base.cpp"
#include "rive/src/generated/shapes/straight_vertex_base.cpp"
#include "rive/src/generated/shapes/triangle_base.cpp"
#include "rive/src/generated/text/text_base.cpp"
#include "rive/src/generated/text/text_modifier_group_base.cpp"
#include "rive/src/generated/text/text_modifier_range_base.cpp"
#include "rive/src/generated/text/text_style_axis_base.cpp"
#include "rive/src/generated/text/text_style_base.cpp"
#include "rive/src/generated/text/text_style_feature_base.cpp"
#include "rive/src/generated/text/text_value_run_base.cpp"
#include "rive/src/generated/text/text_variation_modifier_base.cpp"
#include "rive/src/generated/artboard_base.cpp"
#include "rive/src/generated/audio_event_base.cpp"
#include "rive/src/generated/backboard_base.cpp"
#include "rive/src/generated/custom_property_boolean_base.cpp"
#include "rive/src/generated/custom_property_number_base.cpp"
#include "rive/src/generated/custom_property_string_base.cpp"
#include "rive/src/generated/draw_rules_base.cpp"
#include "rive/src/generated/draw_target_base.cpp"
#include "rive/src/generated/event_base.cpp"
#include "rive/src/generated/foreground_layout_drawable_base.cpp"
#include "rive/src/generated/joystick_base.cpp"
#include "rive/src/generated/layout_component_base.cpp"
#include "rive/src/generated/nested_artboard_base.cpp"
#include "rive/src/generated/nested_artboard_layout_base.cpp"
#include "rive/src/generated/nested_artboard_leaf_base.cpp"
#include "rive/src/generated/node_base.cpp"
#include "rive/src/generated/open_url_event_base.cpp"
#include "rive/src/generated/solo_base.cpp"
#include "rive/src/importers/artboard_importer.cpp"
#include "rive/src/importers/backboard_importer.cpp"
#include "rive/src/importers/bindable_property_importer.cpp"
#include "rive/src/importers/data_converter_group_importer.cpp"
#include "rive/src/importers/enum_importer.cpp"
#include "rive/src/importers/file_asset_importer.cpp"
#include "rive/src/importers/keyed_object_importer.cpp"
#include "rive/src/importers/keyed_property_importer.cpp"
#include "rive/src/importers/layer_state_importer.cpp"
#include "rive/src/importers/linear_animation_importer.cpp"
#include "rive/src/importers/state_machine_importer.cpp"
#include "rive/src/importers/state_machine_layer_component_importer.cpp"
#include "rive/src/importers/state_machine_layer_importer.cpp"
#include "rive/src/importers/state_machine_listener_importer.cpp"
#include "rive/src/importers/state_transition_importer.cpp"
#include "rive/src/importers/transition_viewmodel_condition_importer.cpp"
#include "rive/src/importers/viewmodel_importer.cpp"
#include "rive/src/importers/viewmodel_instance_importer.cpp"
#include "rive/src/importers/viewmodel_instance_list_importer.cpp"
#include "rive/src/math/aabb.cpp"
#include "rive/src/math/bit_field_loc.cpp"
#include "rive/src/math/contour_measure.cpp"
#include "rive/src/math/hit_test.cpp"
#include "rive/src/math/mat2d_find_max_scale.cpp"
#include "rive/src/math/n_slicer_helpers.cpp"
#include "rive/src/math/path_measure.cpp"
#include "rive/src/math/mat2d.cpp"
#include "rive/src/math/raw_path_utils.cpp"
#include "rive/src/math/raw_path.cpp"
#include "rive/src/math/vec2d.cpp"
#include "rive/src/shapes/paint/color.cpp"
#include "rive/src/shapes/paint/dash.cpp"
#include "rive/src/shapes/paint/dash_path.cpp"
#include "rive/src/shapes/paint/fill.cpp"
#include "rive/src/shapes/paint/gradient_stop.cpp"
#include "rive/src/shapes/paint/linear_gradient.cpp"
#include "rive/src/shapes/paint/radial_gradient.cpp"
#include "rive/src/shapes/paint/shape_paint_mutator.cpp"
#include "rive/src/shapes/paint/shape_paint.cpp"
#include "rive/src/shapes/paint/solid_color.cpp"
#include "rive/src/shapes/paint/stroke.cpp"
#include "rive/src/shapes/paint/trim_path.cpp"
#include "rive/src/shapes/clipping_shape.cpp"
#include "rive/src/shapes/cubic_asymmetric_vertex.cpp"
#include "rive/src/shapes/cubic_detached_vertex.cpp"
#include "rive/src/shapes/cubic_mirrored_vertex.cpp"
#include "rive/src/shapes/cubic_vertex.cpp"
#include "rive/src/shapes/deformer.cpp"
#include "rive/src/shapes/ellipse.cpp"
#include "rive/src/shapes/image.cpp"
#include "rive/src/shapes/mesh_vertex.cpp"
#include "rive/src/shapes/mesh.cpp"
#include "rive/src/shapes/parametric_path.cpp"
#include "rive/src/shapes/path_composer.cpp"
#include "rive/src/shapes/path_vertex.cpp"
#include "rive/src/shapes/path.cpp"
#include "rive/src/shapes/points_path.cpp"
#include "rive/src/shapes/polygon.cpp"
#include "rive/src/shapes/rectangle.cpp"
#include "rive/src/shapes/shape_paint_container.cpp"
#include "rive/src/shapes/slice_mesh.cpp"
#include "rive/src/shapes/shape.cpp"
#include "rive/src/shapes/star.cpp"
#include "rive/src/shapes/straight_vertex.cpp"
#include "rive/src/shapes/triangle.cpp"
#include "rive/src/shapes/vertex.cpp"
#include "rive/src/text/font_hb.cpp"
#include "rive/src/text/glyph_lookup.cpp"
#include "rive/src/text/line_breaker.cpp"
#include "rive/src/text/raw_text.cpp"
#include "rive/src/text/text_modifier_group.cpp"
#include "rive/src/text/text_modifier_range.cpp"
#include "rive/src/text/text_modifier.cpp"
#include "rive/src/text/text_style_axis.cpp"
#include "rive/src/text/text_style_feature.cpp"
#include "rive/src/text/text_style.cpp"
#include "rive/src/text/text_value_run.cpp"
#include "rive/src/text/text_variation_modifier.cpp"
#include "rive/src/text/text.cpp"
#include "rive/src/text/utf.cpp"
#include "rive/src/advancing_component.cpp"
#include "rive/src/artboard.cpp"
#include "rive/src/audio_event.cpp"
#include "rive/src/component.cpp"
#include "rive/src/container_component.cpp"
#include "rive/src/dependency_sorter.cpp"
#include "rive/src/draw_rules.cpp"
#include "rive/src/draw_target.cpp"
#include "rive/src/drawable.cpp"
#include "rive/src/event.cpp"
#include "rive/src/factory.cpp"
#include "rive/src/file.cpp"
#include "rive/src/foreground_layout_drawable.cpp"
#include "rive/src/hittest_command_path.cpp"
#include "rive/src/intrinsically_sizeable.cpp"
#include "rive/src/joystick.cpp"
#include "rive/src/layout.cpp"
#include "rive/src/layout_component.cpp"
#include "rive/src/nested_artboard.cpp"
#include "rive/src/nested_artboard_layout.cpp"
#include "rive/src/nested_artboard_leaf.cpp"
#include "rive/src/node.cpp"
#include "rive/src/renderer.cpp"
#include "rive/src/scene.cpp"
#include "rive/src/simple_array.cpp"
#include "rive/src/solo.cpp"
#include "rive/src/static_scene.cpp"
#include "rive/src/transform_component.cpp"
#include "rive/src/world_transform_component.cpp"
#include "rive/src/layout/axis.cpp"
#include "rive/src/layout/axis_x.cpp"
#include "rive/src/layout/axis_y.cpp"
#include "rive/src/layout/layout_component_style.cpp"
#include "rive/src/layout/n_sliced_node.cpp"
#include "rive/src/layout/n_slicer.cpp"
#include "rive/src/layout/n_slicer_details.cpp"
#include "rive/src/layout/n_slicer_tile_mode.cpp"
#include "rive/src/generated/layout/axis_x_base.cpp"
#include "rive/src/generated/layout/axis_y_base.cpp"
#include "rive/src/generated/layout/layout_component_style_base.cpp"
#include "rive/src/generated/layout/n_sliced_node_base.cpp"
#include "rive/src/generated/layout/n_slicer_base.cpp"
#include "rive/src/generated/layout/n_slicer_tile_mode_base.cpp"

NS_WARNING_POP

#ifndef NS_USE_RIVE_UNIQUE_FILENAME
    #define NS_USE_RIVE_UNIQUE_FILENAME 1
#endif

#if !NS_USE_RIVE_UNIQUE_FILENAME
std::string GetFileAssetFilename(rive::FileAsset& asset)
{
    // remove final extension
    std::string filename = asset.name();
    std::size_t finalDot = filename.rfind('.');

    if (finalDot != std::string::npos)
    {
        filename = filename.substr(0, finalDot);
    }
    return filename + "." + asset.fileExtension();
}
#endif

using namespace NoesisApp;


////////////////////////////////////////////////////////////////////////////////////////////////////
static const Noesis::PenLineJoin gLineJoin[3] =
{
    Noesis::PenLineJoin_Miter,
    Noesis::PenLineJoin_Round,
    Noesis::PenLineJoin_Bevel
};

static const Noesis::PenLineCap gLineCap[3] =
{
    Noesis::PenLineCap_Flat,
    Noesis::PenLineCap_Round,
    Noesis::PenLineCap_Square
};

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
struct RiveRenderShader: public rive::RenderShader
{
    enum Type { Solid, Linear, Radial };
    Type mType;

    Noesis::Ptr<Noesis::Brush> mBrush;

    RiveRenderShader(Type type, Noesis::Brush* brush): mType(type), mBrush(brush) { }

    /// Redirect allocations to Noesis memory manager
    static void* operator new(size_t size) { return Noesis::Alloc(size); }
    static void operator delete(void* ptr) { Noesis::Dealloc(ptr); }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
struct RiveRenderPaint: public rive::RenderPaint
{
    RiveFactory* mFactory;
    Noesis::Ptr<Noesis::Brush> mBrush;
    Noesis::Ptr<Noesis::Pen> mPen;
    rive::rcp<rive::RenderShader> mShader;
    rive::BlendMode mBlendMode;

    RiveRenderPaint(RiveFactory* factory): mFactory(factory), mBlendMode(rive::BlendMode::srcOver)
    {
    }

    /// Redirect allocations to Noesis memory manager
    static void* operator new(size_t size) { return Noesis::Alloc(size); }
    static void operator delete(void* ptr) { Noesis::Dealloc(ptr); }

    void style(rive::RenderPaintStyle value) override
    {
        if (value == rive::RenderPaintStyle::stroke)
        {
            mPen = Noesis::MakePtr<Noesis::Pen>();
        }
    }

    void thickness(float value) override
    {
        NS_ASSERT(mPen != nullptr);
        mPen->SetThickness(value);
    }

    void cap(rive::StrokeCap value) override
    {
        NS_ASSERT(mPen != nullptr);
        Noesis::PenLineCap cap = gLineCap[(std::underlying_type<rive::StrokeCap>::type)value];
        mPen->SetStartLineCap(cap);
        mPen->SetEndLineCap(cap);
        mPen->SetDashCap(cap);
    }

    void join(rive::StrokeJoin value) override
    {
        NS_ASSERT(mPen != nullptr);
        mPen->SetLineJoin(gLineJoin[(std::underlying_type<rive::StrokeJoin>::type)value]);
    }

    void blendMode(rive::BlendMode value) override
    {
        mBlendMode = value;

        TweakOpacity();
    }

    void color(unsigned int value) override
    {
        shader(mFactory->makeSolid(value));
    }

    void shader(rive::rcp<rive::RenderShader> value) override
    {
        mFactory->recycleShader(mShader);
        mShader = value;

        RiveRenderShader* shader = (RiveRenderShader*)value.get();
        if (mPen != nullptr)
        {
            mPen->SetBrush(shader->mBrush);
        }
        else
        {
            mBrush = shader->mBrush;
        }

        TweakOpacity();
    }

    void invalidateStroke() override
    {
    }

    // TODO: This is a hack trying to simulate the blend modes we don't have implemented
    void TweakOpacity()
    {
        switch (mBlendMode)
        {
            case rive::BlendMode::overlay:
            case rive::BlendMode::darken:
            case rive::BlendMode::lighten:
            case rive::BlendMode::hardLight:
            case rive::BlendMode::softLight:
            {
                Noesis::Brush* brush = mPen != nullptr ? mPen->GetBrush() : mBrush;
                if (brush != nullptr)
                {
                    float opacity = brush->GetOpacity();
                    brush->SetOpacity(opacity * 0.5f);
                }
                break;
            }
            case rive::BlendMode::difference:
                NS_LOG_WARNING("BlendMode 'Difference' not supported");
                break;
            case rive::BlendMode::exclusion:
                NS_LOG_WARNING("BlendMode 'Exclusion' not supported");
                break;
            case rive::BlendMode::hue:
                NS_LOG_WARNING("BlendMode 'Hue' not supported");
                break;
            case rive::BlendMode::saturation:
                NS_LOG_WARNING("BlendMode 'Saturation' not supported");
                break;
            case rive::BlendMode::color:
                NS_LOG_WARNING("BlendMode 'Color' not supported");
                break;
            case rive::BlendMode::luminosity:
                NS_LOG_WARNING("BlendMode 'Luminosity' not supported");
                break;
            default: break;
        }
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
struct RiveRenderImage: public rive::RenderImage
{
    RiveRenderImage(const Noesis::Uri& uri)
        : mImageSource(Noesis::MakePtr<Noesis::BitmapImage>(uri))
    {
        NS_LOG_TRACE("[%p] RenderImage.ctor", this);
    }

    /// Redirect allocations to Noesis memory manager
    static void* operator new(size_t size) { return Noesis::Alloc(size); }
    static void operator delete(void* ptr) { Noesis::Dealloc(ptr); }

    Noesis::Ptr<Noesis::ImageSource> mImageSource;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
struct RiveRenderPath: public rive::RenderPath
{
    Noesis::Ptr<Noesis::StreamGeometry> mStreamGeometry;
    Noesis::StreamGeometryContext mContext;
    bool mClosed;

    RiveRenderPath(): mStreamGeometry(Noesis::MakePtr<Noesis::StreamGeometry>()),
        mContext(mStreamGeometry->Open()), mClosed(false)
    {
        NS_LOG_TRACE("[%p] RenderPath.ctor", this);
    }

    ~RiveRenderPath()
    {
        NS_LOG_TRACE("[%p] RenderPath.dtor", this);
    }

    /// Redirect allocations to Noesis memory manager
    static void* operator new(size_t size) { return Noesis::Alloc(size); }
    static void operator delete(void* ptr) { Noesis::Dealloc(ptr); }

    void rewind() override
    {
        NS_LOG_TRACE("[%p] Rewind()", this);

        mContext = mStreamGeometry->Open();
        mClosed = false;
    }

    void fillRule(rive::FillRule value) override
    {
        NS_LOG_TRACE("[%p] FillRule(%s)", this, value == rive::FillRule::nonZero ?
            "NonZero" : "EvenOdd");

        mStreamGeometry->SetFillRule(value == rive::FillRule::nonZero ?
            Noesis::FillRule_Nonzero : Noesis::FillRule_EvenOdd);
    }

    void addRenderPath(rive::RenderPath* path_, const rive::Mat2D& transform_) override
    {
        RiveRenderPath* path = (RiveRenderPath*)path_;
        path->prepareGeometry();

        NS_LOG_TRACE("[%p] AddPath(%p)", this, path);

        // Ignore paths with invalid transform
        Noesis::Transform2 transform(transform_.values());
        float det = Noesis::Determinant(transform.Rotation());
        if (Noesis::IsZero(det) || Noesis::IsNaN(det)) return;

        // Append path commands
        Noesis::PathCommandIterator it = path->mStreamGeometry->GetCommands();
        while (!it.End())
        {
            auto cmd = it.Next();

            switch (cmd.figureCommand)
            {
                case Noesis::SVGPath::FigureCommand_Move:
                {
                    Noesis::Point p(cmd.move.x, cmd.move.y);
                    Noesis::Point pt = p * transform;
                    mContext.BeginFigure(pt, false);
                    break;
                }
                case Noesis::SVGPath::FigureCommand_Line:
                {
                    Noesis::Point p(cmd.line.x, cmd.line.y);
                    Noesis::Point pt = p * transform;
                    mContext.LineTo(pt);
                    break;
                }
                case Noesis::SVGPath::FigureCommand_Cubic:
                {
                    Noesis::Point p(cmd.cubic.x0, cmd.cubic.y0);
                    Noesis::Point pt = p * transform;
                    Noesis::Point q(cmd.cubic.x1, cmd.cubic.y1);
                    Noesis::Point qt = q * transform;
                    Noesis::Point r(cmd.cubic.x2, cmd.cubic.y2);
                    Noesis::Point rt = r * transform;
                    mContext.CubicTo(pt, qt, rt);
                    break;
                }
                case Noesis::SVGPath::FigureCommand_Close:
                {
                    mContext.SetIsClosed(true);
                    break;
                }
                default: NS_ASSERT_UNREACHABLE;
            }
        }
    }

    void moveTo(float x, float y) override
    {
        if (Noesis::IsNaN(x)) x = 0.0f;
        if (Noesis::IsNaN(y)) y = 0.0f;
        mContext.BeginFigure(Noesis::Point(x, y), false);
        NS_LOG_TRACE("[%p] MoveTo(%.4f,%.4f)", this, x, y);
    }

    void lineTo(float x, float y) override
    {
        if (Noesis::IsNaN(x)) x = 0.0f;
        if (Noesis::IsNaN(y)) y = 0.0f;
        mContext.LineTo(Noesis::Point(x, y));
        NS_LOG_TRACE("[%p] LineTo(%.4f,%.4f)", this, x, y);
    }

    void cubicTo(float ox, float oy, float ix, float iy, float x, float y) override
    {
        if (Noesis::IsNaN(ox)) ox = 0.0f;
        if (Noesis::IsNaN(oy)) oy = 0.0f;
        if (Noesis::IsNaN(ix)) ix = 0.0f;
        if (Noesis::IsNaN(iy)) iy = 0.0f;
        if (Noesis::IsNaN(x)) x = 0.0f;
        if (Noesis::IsNaN(y)) y = 0.0f;
        mContext.CubicTo(Noesis::Point(ox, oy), Noesis::Point(ix, iy), Noesis::Point(x, y));
        NS_LOG_TRACE("[%p] CubicTo(%.4f,%.4f;%.4f,%.4f;%.4f,%.4f)", this, ox, oy, ix, iy, x, y);
    }

    void close() override
    {
        mContext.SetIsClosed(true);
        NS_LOG_TRACE("[%p] Close()", this);
    }

    void prepareGeometry()
    {
        if (!mClosed)
        {
            mContext.Close();
            mClosed = true;
        }
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
class RiveRenderBuffer : public rive::RenderBuffer
{
public:

    RiveRenderBuffer(rive::RenderBufferType type, rive::RenderBufferFlags flags, size_t sizeInBytes)
        : rive::RenderBuffer(type, flags, sizeInBytes), mData((uint32_t)sizeInBytes, 0)
    {
        NS_LOG_TRACE("[%p] RenderBuffer.ctor", this);
    }

    const uint8_t* GetData() const
    {
        return mData.Data();
    }

protected:

    void* onMap() override
    {
        NS_LOG_TRACE("[%p] Map", this);
        return mData.Data();
    }

    void onUnmap() override
    {
        NS_LOG_TRACE("[%p] Unmap", this);
    }

private:
    Noesis::Vector<uint8_t> mData;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
RiveFileAssetLoader::RiveFileAssetLoader(const char* baseUri)
    : mBaseUri(baseUri)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool RiveFileAssetLoader::loadContents(rive::FileAsset& asset, rive::Span<const uint8_t> /*inBandBytes*/,
    rive::Factory* /*factory*/)
{
    if (asset.is<rive::ImageAsset>())
    {
#if NS_USE_RIVE_UNIQUE_FILENAME
        auto filename = asset.uniqueFilename();
#else
        auto filename = GetFileAssetFilename(asset);
#endif
        Noesis::Uri uri(mBaseUri, filename.c_str());

        auto image = rive::make_rcp<RiveRenderImage>(uri);

        auto imageAsset = asset.as<rive::ImageAsset>();
        imageAsset->renderImage(image);

        return true;
    }
  #ifdef WITH_RIVE_TEXT
    else if (asset.is<rive::FontAsset>())
    {
#if NS_USE_RIVE_UNIQUE_FILENAME
        auto filename = asset.uniqueFilename();
#else
        auto filename = GetFileAssetFilename(asset);
#endif
        Noesis::Uri uri(mBaseUri, filename.c_str());

        auto stream = Noesis::GUI::LoadXamlResource(uri);
        if (stream != nullptr)
        {
            auto streamPtr = (uint8_t*)stream->GetMemoryBase();
            auto streamSize = stream->GetLength();
            Noesis::Vector<uint8_t> buffer;
            if (streamPtr == nullptr)
            {
                buffer.Resize(streamSize);
                streamPtr = buffer.Data();
                stream->Read(streamPtr, streamSize);
            }
            auto span = rive::Span<const uint8_t>(streamPtr, (size_t)streamSize);
            auto font = HBFont::Decode(Noesis::MoveArg(span));

            auto fontAsset = asset.as<rive::FontAsset>();
            fontAsset->font(font);

            return true;
        }
    }
  #endif

    return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
RiveFactory::RiveFactory()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////
RiveFactory::~RiveFactory()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void RiveFactory::recycleShader(const rive::rcp<rive::RenderShader>& shader)
{
    if (shader.get() == nullptr) return;

    RiveRenderShader* shader_ = (RiveRenderShader*)shader.get();
    switch (shader_->mType)
    {
        case RiveRenderShader::Solid:
        {
            mRecycledSolid.PushBack(shader);
            break;
        }
        case RiveRenderShader::Linear:
        {
            mRecycledLinear.PushBack(shader);
            break;
        }
        case RiveRenderShader::Radial:
        {
            mRecycledRadial.PushBack(shader);
            break;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
rive::rcp<rive::RenderShader> RiveFactory::makeSolid(unsigned int color)
{
    rive::rcp<rive::RenderShader> shader;

    Noesis::Ptr<Noesis::SolidColorBrush> brush;
    if (mRecycledSolid.Empty())
    {
        brush = Noesis::MakePtr<Noesis::SolidColorBrush>();
        shader.reset(new RiveRenderShader(RiveRenderShader::Solid, brush));
    }
    else
    {
        shader = mRecycledSolid.Back();
        mRecycledSolid.PopBack();
        brush.Reset((Noesis::SolidColorBrush*)((RiveRenderShader*)shader.get())->mBrush.GetPtr());
    }

    brush->SetOpacity(1.0f);
    brush->SetColor(Noesis::Color::FromPackedBGRA(color));

    return shader;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
static void UpdateBrushStops(Noesis::GradientStopCollection* brushStops,
    size_t numStops, const float stops[], const rive::ColorInt colors[],
    Noesis::Vector<Noesis::Ptr<Noesis::GradientStop>>& recycledStops)
{
    for (int i = 0; i < brushStops->Count(); ++i)
    {
        recycledStops.EmplaceBack().Reset(brushStops->Get(i));
    }
    brushStops->Clear();

    for (size_t i = 0; i < numStops; ++i)
    {
        Noesis::Ptr<Noesis::GradientStop> stop;
        if (recycledStops.Empty())
        {
            stop = Noesis::MakePtr<Noesis::GradientStop>();
        }
        else
        {
            stop = recycledStops.Back();
            recycledStops.PopBack();
        }

        stop->SetColor(Noesis::Color::FromPackedBGRA(colors[i]));
        stop->SetOffset(stops[i]);

        brushStops->Add(stop);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
rive::rcp<rive::RenderShader> RiveFactory::makeLinearGradient(float sx, float sy, float ex, float ey,
    const rive::ColorInt colors[], const float stops[], size_t count)
{
    rive::rcp<rive::RenderShader> shader;

    Noesis::Ptr<Noesis::LinearGradientBrush> brush;
    if (mRecycledLinear.Empty())
    {
        brush = Noesis::MakePtr<Noesis::LinearGradientBrush>();
        shader.reset(new RiveRenderShader(RiveRenderShader::Linear,brush));
    }
    else
    {
        shader = mRecycledLinear.Back();
        mRecycledLinear.PopBack();
        brush.Reset((Noesis::LinearGradientBrush*)((RiveRenderShader*)shader.get())->mBrush.GetPtr());
    }

    brush->SetOpacity(1.0f);
    brush->SetStartPoint(Noesis::Point(sx, sy));
    brush->SetEndPoint(Noesis::Point(ex, ey));
    brush->SetMappingMode(Noesis::BrushMappingMode_Absolute);
    brush->SetSpreadMethod(Noesis::GradientSpreadMethod_Pad);

    UpdateBrushStops(brush->GetGradientStops(), count, stops, colors, mRecycledStops);

    return shader;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
rive::rcp<rive::RenderShader> RiveFactory::makeRadialGradient(float cx, float cy, float radius,
    const rive::ColorInt colors[], const float stops[], size_t count)
{
    rive::rcp<rive::RenderShader> shader;

    Noesis::Ptr<Noesis::RadialGradientBrush> brush;
    if (mRecycledRadial.Empty())
    {
        brush = Noesis::MakePtr<Noesis::RadialGradientBrush>();
        shader.reset(new RiveRenderShader(RiveRenderShader::Radial, brush));
    }
    else
    {
        shader = mRecycledRadial.Back();
        mRecycledRadial.PopBack();
        brush.Reset((Noesis::RadialGradientBrush*)((RiveRenderShader*)shader.get())->mBrush.GetPtr());
    }

    brush->SetOpacity(1.0f);
    brush->SetCenter(Noesis::Point(cx, cy));
    brush->SetGradientOrigin(Noesis::Point(cx, cy));
    brush->SetRadiusX(radius);
    brush->SetRadiusY(radius);
    brush->SetMappingMode(Noesis::BrushMappingMode_Absolute);
    brush->SetSpreadMethod(Noesis::GradientSpreadMethod_Pad);

    UpdateBrushStops(brush->GetGradientStops(), count, stops, colors, mRecycledStops);

    return shader;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
rive::rcp<rive::RenderPaint> RiveFactory::makeRenderPaint()
{
    return rive::rcp<rive::RenderPaint>(new RiveRenderPaint(this));
}

////////////////////////////////////////////////////////////////////////////////////////////////////
rive::rcp<rive::RenderPath> RiveFactory::makeRenderPath(rive::RawPath& path,
    rive::FillRule /*fillRule*/)
{
    rive::rcp<rive::RenderPath> renderPath(new RiveRenderPath);
    rive::RenderPath* renderPath_ = renderPath.get();

    int i = 0;
    rive::Span<const rive::Vec2D> points = path.points();

    for (auto verb : path.verbs())
    {
        switch (verb)
        {
            case rive::PathVerb::move:
            {
                const rive::Vec2D& p = points[i++];
                renderPath_->moveTo(p.x, p.y);
                NS_LOG_TRACE("[%p] MoveTo(%.4f,%.4f)", renderPath_, p.x, p.y);
                break;
            }
            case rive::PathVerb::line:
            {
                const rive::Vec2D& p = points[i++];
                renderPath_->lineTo(p.x, p.y);
                NS_LOG_TRACE("[%p] LineTo(%.4f,%.4f)", renderPath_, p.x, p.y);
                break;
            }
            case rive::PathVerb::cubic:
            {
                const rive::Vec2D& p1 = points[i++];
                const rive::Vec2D& p2 = points[i++];
                const rive::Vec2D& p3 = points[i++];
                renderPath_->cubicTo(p1.x, p1.y, p2.x, p2.y, p3.x, p3.y);
                NS_LOG_TRACE("[%p] CubicTo(%.4f,%.4f;%.4f,%.4f;%.4f,%.4f)", renderPath_,
                    p1.x, p1.y, p2.x, p2.y, p3.x, p3.y);
                break;
            }
            case rive::PathVerb::close:
            {
                renderPath_->close();
                NS_LOG_TRACE("[%p] Close()", this);
                break;
            }

            case rive::PathVerb::quad:
            default:
                NS_ASSERT_UNREACHABLE;
        }
    }

    return renderPath;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
rive::rcp<rive::RenderPath> RiveFactory::makeEmptyRenderPath()
{
    return rive::rcp<rive::RenderPath>(new RiveRenderPath);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
rive::rcp<rive::RenderImage> RiveFactory::decodeImage(rive::Span<const uint8_t> /*bytes*/)
{
    return nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
rive::rcp<rive::RenderBuffer> RiveFactory::makeRenderBuffer(rive::RenderBufferType type,
    rive::RenderBufferFlags flags, size_t sizeInBytes)
{
    return rive::make_rcp<RiveRenderBuffer>(type, flags, sizeInBytes);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
RiveRenderer::RiveRenderer(): mContext(nullptr), mContextStackDepth(0), mUsedTransforms(0)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void RiveRenderer::Reset(Noesis::DrawingContext* context)
{
    mContext = context;
    mContextStackCounts.Clear();
    mContextStackDepth = 0;
    mUsedTransforms = 0;
    mUsedMeshes = 0;
    mUsedRectangles = 0;
    mUsedImageBrushes = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
Noesis::Ptr<Noesis::MatrixTransform> RiveRenderer::AllocateTransform()
{
    if (mUsedTransforms < mTransformPool.Size())
    {
        return mTransformPool[mUsedTransforms++];
    }

    mUsedTransforms++;
    Noesis::Ptr<Noesis::MatrixTransform> transform = Noesis::MakePtr<Noesis::MatrixTransform>();
    mTransformPool.PushBack(transform);
    return transform;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
Noesis::Ptr<Noesis::MeshGeometry> RiveRenderer::AllocateMesh()
{
    if (mUsedMeshes < mMeshPool.Size())
    {
        return mMeshPool[mUsedMeshes++];
    }

    mUsedMeshes++;
    Noesis::Ptr<Noesis::MeshGeometry> mesh = Noesis::MakePtr<Noesis::MeshGeometry>();
    mMeshPool.PushBack(mesh);
    return mesh; 
}

////////////////////////////////////////////////////////////////////////////////////////////////////
Noesis::Ptr<Noesis::RectangleGeometry> RiveRenderer::AllocateRectangle()
{
    if (mUsedRectangles < mRectanglePool.Size())
    {
        return mRectanglePool[mUsedRectangles++];
    }

    mUsedRectangles++;
    Noesis::Ptr<Noesis::RectangleGeometry> rectangle = Noesis::MakePtr<Noesis::RectangleGeometry>();
    mRectanglePool.PushBack(rectangle);
    return rectangle;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
Noesis::Ptr<Noesis::ImageBrush> RiveRenderer::AllocateImageBrush()
{
    if (mUsedImageBrushes < mImageBrushPool.Size())
    {
        return mImageBrushPool[mUsedImageBrushes++];
    }

    mUsedImageBrushes++;
    Noesis::Ptr<Noesis::ImageBrush> brush = Noesis::MakePtr<Noesis::ImageBrush>();
    mImageBrushPool.PushBack(brush);
    return brush;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void RiveRenderer::save()
{
    mContextStackCounts.PushBack(mContextStackDepth);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void RiveRenderer::restore()
{
    uint32_t savedDepth = mContextStackCounts.Back();
    mContextStackCounts.PopBack();
    uint32_t count = mContextStackDepth - savedDepth;
    for (uint32_t i = 0; i < count; ++i)
    {
        mContext->Pop();
    }
    mContextStackDepth = savedDepth;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void RiveRenderer::transform(const rive::Mat2D& transform)
{
    auto matrixTransform = AllocateTransform();
    matrixTransform->SetMatrix(Noesis::Transform2(transform.values()));
    mContext->PushTransform(matrixTransform);
    ++mContextStackDepth;
}

static const char* GetBlendModeName(rive::BlendMode blendMode)
{
    switch (blendMode)
    {
        case rive::BlendMode::srcOver:
            return "SrcOver";
        case rive::BlendMode::screen:
        case rive::BlendMode::colorDodge:
        case rive::BlendMode::lighten:
        case rive::BlendMode::softLight:
            return "Screen";
        case rive::BlendMode::multiply:
        case rive::BlendMode::colorBurn:
        case rive::BlendMode::darken:
            return "Multiply";
        case rive::BlendMode::overlay:
        case rive::BlendMode::hardLight:
            return "Multiply+Screen";
        default: break;
    }
    return "Unknown";
}

////////////////////////////////////////////////////////////////////////////////////////////////////
template<class DrawFunction>
static void DrawGeometry(rive::BlendMode blendMode, Noesis::DrawingContext* context,
    const DrawFunction& drawFunction)
{
    switch (blendMode)
    {
        case rive::BlendMode::srcOver:
        {
            drawFunction(context);
            break;
        }
        case rive::BlendMode::screen:
        case rive::BlendMode::colorDodge:
        case rive::BlendMode::lighten:
        case rive::BlendMode::softLight:
        {
            context->PushBlendingMode(Noesis::BlendingMode_Screen);
            drawFunction(context);
            context->Pop();
            break;
        }
        case rive::BlendMode::multiply:
        case rive::BlendMode::colorBurn:
        case rive::BlendMode::darken:
        {
            context->PushBlendingMode(Noesis::BlendingMode_Multiply);
            drawFunction(context);
            context->Pop();
            break;
        }
        case rive::BlendMode::overlay:
        case rive::BlendMode::hardLight:
        {
            context->PushBlendingMode(Noesis::BlendingMode_Multiply);
            drawFunction(context);
            context->Pop();

            context->PushBlendingMode(Noesis::BlendingMode_Screen);
            drawFunction(context);
            context->Pop();

            break;
        }
        default: break;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void RiveRenderer::drawPath(rive::RenderPath* path_, rive::RenderPaint* paint_)
{
    RiveRenderPaint* paint = (RiveRenderPaint*)paint_;
    Noesis::Brush* brush = paint->mBrush;
    Noesis::Pen* pen = paint->mPen;

    RiveRenderPath* path = (RiveRenderPath*)path_;
    path->prepareGeometry();

    Noesis::Geometry* geometry = path->mStreamGeometry;
    auto blendMode = paint->mBlendMode;
    if (!geometry->GetRenderBounds(pen).IsEmpty())
    {
        NS_LOG_TRACE("DrawPath(Path=%p, Paint=%p, Blend=%s)", path, paint, GetBlendModeName(blendMode));
        DrawGeometry(blendMode, mContext, [brush, pen, geometry](Noesis::DrawingContext* context)
        {
            context->DrawGeometry(brush, pen, geometry);
        });
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void RiveRenderer::clipPath(rive::RenderPath* path_)
{
    RiveRenderPath* path = (RiveRenderPath*)path_;
    path->prepareGeometry();
    Noesis::Geometry* geometry = path->mStreamGeometry;

    NS_LOG_TRACE("ClipPath(Path=%p)", path);

    mContext->PushClip(geometry);
    ++mContextStackDepth;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void RiveRenderer::drawImage(const rive::RenderImage* image_, rive::BlendMode blendMode,
    float opacity)
{
    RiveRenderImage* image = (RiveRenderImage*)image_;
    Noesis::ImageSource* imageSource = image->mImageSource;
    auto imageBrush = AllocateImageBrush();
    imageBrush->SetImageSource(imageSource);
    imageBrush->SetOpacity(opacity);

    auto rectGeometry = AllocateRectangle();
    rectGeometry->SetRect(Noesis::Rect(-0.5f * imageSource->GetWidth(), -0.5f * imageSource->GetHeight(),
        0.5f * imageSource->GetWidth(), 0.5f * imageSource->GetHeight()));

    NS_LOG_TRACE("DrawImage(Image=%p, Blend=%s)", image, GetBlendModeName(blendMode));
    DrawGeometry(blendMode, mContext, [imageBrush, rectGeometry](Noesis::DrawingContext* context)
    {
        // Using DrawGeometry instead of DrawImage to support opacity
        context->DrawGeometry(imageBrush, nullptr, rectGeometry);
    });
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void RiveRenderer::drawImageMesh(const rive::RenderImage* image_,
    rive::rcp<rive::RenderBuffer> vertices_,
    rive::rcp<rive::RenderBuffer> uvCoords_,
    rive::rcp<rive::RenderBuffer> indices_,
    uint32_t vertexCount,
    uint32_t indexCount,
    rive::BlendMode blendMode,
    float opacity)
{
    RiveRenderImage* image = (RiveRenderImage*)image_;
    Noesis::ImageSource* imageSource = image->mImageSource;
    auto imageBrush = AllocateImageBrush();
    imageBrush->SetImageSource(imageSource);
    imageBrush->SetOpacity(opacity);

    auto meshGeometry = AllocateMesh();
    meshGeometry->SetNumVertices(vertexCount);
    meshGeometry->SetNumUVs(vertexCount);
    meshGeometry->SetNumIndices(indexCount);

    RiveRenderBuffer* vertices = (RiveRenderBuffer*)vertices_.get();
    NS_ASSERT(vertexCount * sizeof(Noesis::Point) == vertices->sizeInBytes());
    Noesis::Point* meshVertices = meshGeometry->GetVertices();
    memcpy(meshVertices, vertices->GetData(), vertexCount * sizeof(Noesis::Point));

    RiveRenderBuffer* uvCoords = (RiveRenderBuffer*)uvCoords_.get();
    NS_ASSERT(vertexCount * sizeof(Noesis::Point) == uvCoords->sizeInBytes());
    Noesis::Point* meshUVs = meshGeometry->GetUVs();
    memcpy(meshUVs, uvCoords->GetData(), vertexCount * sizeof(Noesis::Point));

    RiveRenderBuffer* indices = (RiveRenderBuffer*)indices_.get();
    NS_ASSERT(indexCount * sizeof(uint16_t) == indices->sizeInBytes());
    memcpy(meshGeometry->GetIndices(), indices->GetData(), indexCount * sizeof(uint16_t));

    float minX = FLT_INF;
    float minY = FLT_INF;
    float maxX = -FLT_INF;
    float maxY = -FLT_INF;
    for (uint32_t vertexIndex = 0; vertexIndex < vertexCount; ++vertexIndex)
    {
        const Noesis::Point& vertex = meshVertices[vertexIndex];
        minX = fminf(minX, vertex.x);
        minY = fminf(minY, vertex.y);
        maxX = fmaxf(maxX, vertex.x);
        maxY = fmaxf(maxY, vertex.y);
    }
    meshGeometry->SetBounds(Noesis::Rect(minX, minY, maxX, maxY));

    meshGeometry->Update();

    NS_LOG_TRACE("DrawImageMesh(Image=%p, Blend=%s)", image, GetBlendModeName(blendMode));
    DrawGeometry(blendMode, mContext, [imageBrush, meshGeometry](Noesis::DrawingContext* context)
    {
        context->DrawGeometry(imageBrush, nullptr, meshGeometry);
    });
}

////////////////////////////////////////////////////////////////////////////////////////////////////
class RiveDependencyFileAssetLoader: public rive::FileAssetLoader
{
public:
    RiveDependencyFileAssetLoader(const char* baseUri): mBaseUri(baseUri) {}
    ~RiveDependencyFileAssetLoader() {}

    bool loadContents(rive::FileAsset& asset, rive::Span<const uint8_t>, rive::Factory*) override
    {
#if NS_USE_RIVE_UNIQUE_FILENAME
        auto filename = asset.uniqueFilename();
#else
        auto filename = GetFileAssetFilename(asset);
#endif
        Noesis::Uri uri(mBaseUri, filename.c_str());

        if (asset.is<rive::ImageAsset>())
        {
            mImageDependencies.PushBack(uri);
        }
#ifdef WITH_RIVE_TEXT
        else if (asset.is<rive::FontAsset>())
        {
            mFontDependencies.PushBack(uri);
        }
#endif

        return false;
    }

    Noesis::Uri mBaseUri;
    Noesis::Vector<Noesis::Uri> mImageDependencies;
    Noesis::Vector<Noesis::Uri> mFontDependencies;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
void NoesisApp::GetRiveDependencies(Noesis::Stream* stream, const Noesis::Uri& uri,
    Noesis::Vector<Noesis::Uri>& images, Noesis::Vector<Noesis::Uri>& fonts)
{
    if (stream != nullptr)
    {
        RiveFactory riveFactory;

        Noesis::String baseUri = uri.ToString();
        baseUri.Erase(baseUri.FindLast("/") + 1);
        rive::ImportResult result;
        RiveDependencyFileAssetLoader loader(baseUri.Str());

        auto streamPtr = (uint8_t*)stream->GetMemoryBase();
        auto streamSize = stream->GetLength();
        Noesis::Vector<uint8_t> buffer;
        if (streamPtr == nullptr)
        {
            buffer.Resize(streamSize);
            streamPtr = buffer.Data();
            stream->Read(streamPtr, streamSize);
        }
        auto span = rive::Span<const uint8_t>(streamPtr, (size_t)streamSize);
        rive::File::import(span, &riveFactory, &result, &loader);

        images = Noesis::MoveArg(loader.mImageDependencies);
        fonts = Noesis::MoveArg(loader.mFontDependencies);
    }
}
