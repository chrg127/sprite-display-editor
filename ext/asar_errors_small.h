#ifndef _ASAR_ERRORS_SMALL_H_
#define _ASAR_ERRORS_SMALL_H_

// RudeGuy: this is a very small version of the errors library in asar.
// It's very useful if you don't wanna copy all those necessary files
// to have the usual errors library work (which is what I needed).

// NOTE: Don't reorder these. That would change their ID.
// If you need to remove one, stub it out.
// If you need to add one, add it at the end (before errid_end).
// Keep in sync with asar_errors.

#define ASAR_ERROR_RANGE_START	5000

namespace smw {

enum asar_errid : int
{
	errid_start = ASAR_ERROR_RANGE_START,

	errid_limit_reached,
	errid_werror,

	errid_16mb_rom_limit,
	errid_buffer_too_small,
	errid_params_null,
	errid_params_invalid_size,
	errid_cmdl_define_invalid,
	errid_cmdl_define_override,
	errid_create_rom_failed,
	errid_open_rom_failed,
	errid_open_rom_not_smw_extension,
	errid_open_rom_not_smw_header,
	errid_stddefines_no_identifier,
	errid_stddefine_after_closing_quote,

	errid_failed_to_open_file,
	errid_file_not_found,
	errid_readfile_1_to_4_bytes,
	errid_canreadfile_0_bytes,
	errid_file_offset_out_of_bounds,
	errid_rep_at_file_end,

	errid_mismatched_parentheses,
	errid_invalid_hex_value,
	errid_invalid_binary_value,
	errid_invalid_character,
	errid_string_literal_not_terminated,
	errid_malformed_function_call,
	errid_invalid_number,
	errid_garbage_near_quoted_string,
	errid_mismatched_quotes,

	errid_rom_too_short,
	errid_rom_title_incorrect,

	errid_bank_border_crossed,

	errid_start_of_file,
	errid_xkas_asar_conflict,
	errid_invalid_version_number,
	errid_asar_too_old,

	errid_relative_branch_out_of_bounds,
	errid_snes_address_doesnt_map_to_rom,
	errid_snes_address_out_of_bounds,
	errid_invalid_tcall,
	errid_use_xplus,
	errid_opcode_length_too_long,
	errid_superfx_invalid_short_address,
	errid_superfx_invalid_register,
	errid_invalid_opcode_length,
	errid_invalid_mapper,

	errid_nan,
	errid_division_by_zero,
	errid_modulo_by_zero,
	errid_unknown_operator,
	errid_invalid_input,

	errid_invalid_function_name,
	errid_function_not_found,
	errid_function_redefined,
	errid_broken_function_declaration,
	errid_wrong_num_parameters,
	errid_invalid_param_name,
	errid_math_invalid_type,

	errid_invalid_label_name,
	errid_label_not_found,
	errid_label_redefined,
	errid_broken_label_definition,
	errid_label_cross_assignment,
	errid_macro_label_outside_of_macro,
	errid_label_on_third_pass,
	errid_label_moving,

	errid_invalid_namespace_name,
	errid_invalid_namespace_use,

	errid_invalid_struct_name,
	errid_struct_not_found,
	errid_struct_redefined,
	errid_struct_invalid_parent_name,
	errid_invalid_label_missing_closer,
	errid_multiple_subscript_operators,
	errid_invalid_subscript,
	errid_label_missing_parent,
	errid_array_invalid_inside_structs,
	errid_struct_without_endstruct,
	errid_nested_struct,
	errid_missing_struct_params,
	errid_too_many_struct_params,
	errid_missing_extends,
	errid_invalid_endstruct_count,
	errid_expected_align,
	errid_endstruct_without_struct,
	errid_alignment_too_small,

	errid_invalid_define_name,
	errid_define_not_found,
	errid_broken_define_declaration,
	errid_overriding_builtin_define,
	errid_define_label_math,
	errid_mismatched_braces,

	errid_invalid_macro_name,
	errid_macro_not_found,
	errid_macro_redefined,
	errid_broken_macro_declaration,
	errid_invalid_macro_param_name,
	errid_macro_param_not_found,
	errid_macro_param_redefined,
	errid_broken_macro_usage,
	errid_macro_wrong_num_params,
	errid_broken_macro_contents,
	errid_rep_at_macro_end,
	errid_nested_macro_definition,
	errid_misplaced_endmacro,
	errid_unclosed_macro,

	errid_label_in_conditional,
	errid_broken_conditional,
	errid_invalid_condition,
	errid_misplaced_elseif,
	errid_elseif_in_while,
	errid_elseif_in_singleline,
	errid_misplaced_endif,
	errid_misplaced_else,
	errid_else_in_while_loop,
	errid_unclosed_if,

	errid_unknown_command,
	errid_command_disabled,

	errid_broken_incbin,
	errid_incbin_64kb_limit,
	errid_recursion_limit,
	errid_command_in_non_root_file,
	errid_cant_be_main_file,
	errid_no_labels_here,

	errid_invalid_freespace_request,
	errid_no_banks_with_ram_mirrors,
	errid_no_freespace_norom,
	errid_static_freespace_autoclean,
	errid_static_freespace_growing,
	errid_no_freespace_in_mapped_banks,
	errid_no_freespace,
	errid_freespace_limit_reached,

	errid_prot_not_at_freespace_start,
	errid_prot_too_many_entries,

	errid_autoclean_in_freespace,
	errid_autoclean_label_at_freespace_end,
	errid_broken_autoclean,

	errid_pulltable_without_table,
	errid_invalid_table_file,

	errid_pad_in_freespace,

	errid_org_label_invalid,
	errid_org_label_forward,

	errid_skip_label_invalid,

	errid_spc700_inline_no_base,
	errid_base_label_invalid,

	errid_rep_label,

	errid_pushpc_without_pullpc,
	errid_pullpc_without_pushpc,
	errid_pullpc_different_arch,
	errid_pullbase_without_pushbase,

	errid_invalid_math,
	errid_invalid_warn,
	errid_invalid_check,

	errid_warnpc_in_freespace,
	errid_warnpc_broken_param,
	errid_warnpc_failed,
	errid_warnpc_failed_equal,

	errid_assertion_failed,

	errid_error_command,

	errid_invalid_print_function_syntax,
	errid_unknown_variable,

	errid_invalid_warning_id,

	errid_pushwarnings_without_pullwarnings,
	errid_pullwarnings_without_pushwarnings,

	errid_failed_to_open_file_access_denied,
	errid_failed_to_open_not_regular_file,

	errid_end,
	errid_count = errid_end - errid_start - 1,
};

}

#endif
