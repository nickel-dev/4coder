#include <string>

// Source: https://github.com/clearfeld/4coder-clearfeld/blob/master/clearfeld/custom_commands/rect_operations.cpp
CUSTOM_COMMAND_SIG(kill_rect)
CUSTOM_DOC("Cuts the content between the mark and cursor as a rectangle.")
{
    Scratch_Block scratch(app);
    View_ID view_id = get_active_view(app, Access_ReadVisible);
    Buffer_ID buffer_id = view_get_buffer(app, view_id, Access_ReadVisible);
	
    i64 cursor_pos = view_get_cursor_pos(app, view_id);
    i64 mark_pos = view_get_mark_pos(app, view_id);
	
    Buffer_Cursor cursor = view_compute_cursor(app, view_id, seek_pos(cursor_pos));
    Buffer_Cursor mark = view_compute_cursor(app, view_id, seek_pos(mark_pos));
	
	i64 start_line, end_line;
	i64 start_pos, end_pos;
	
	if (cursor.line > mark.line) {
		start_line = mark.line;
		end_line = cursor.line;
	} else {
		start_line = cursor.line;
		end_line = mark.line;
	}
	
	if (cursor.col > mark.col) {
		start_pos = mark.col;
		end_pos = cursor.col;
	} else {
		start_pos = cursor.col;
		end_pos = mark.col;
	}
	
	--end_pos;
	
	std::string a;
	
	for (i64 i = start_line; i < end_line + 1; ++i) {
		String_Const_u8 current_line = push_buffer_line(app, scratch, buffer_id, i);
		
		view_set_mark(app, view_id, seek_line_col(i, start_pos));
		view_set_cursor(app, view_id, seek_line_col(i, end_pos));
		
		// NOTE: start_pos - 1 to capute the char under the mark/cursor
		for (i64 j = start_pos - 1; j < end_pos; ++j) {
			if (current_line.size != 0) {
				if (current_line.str[j] == '\0') {
					a += "";
				}
				else {
					a += current_line.str[j];
				}
			}
		}
		
		a += "\n";
	}
	
	History_Group hg = history_group_begin(app, buffer_id);
	for (i64 i = start_line; i < end_line + 1; ++i) {
		String_Const_u8 current_line = push_buffer_line(app, scratch, buffer_id, i);
		if (current_line.size != 0) {
			view_set_mark(app, view_id, seek_line_col(i, start_pos));
			view_set_cursor(app, view_id, seek_line_col(i, end_pos));
			delete_range(app);
			delete_char(app);
		}
	}
	history_group_end(hg);
	
	clipboard_post(app, 0, SCu8((char*)a.c_str()));
}

// NOTE(nickel): New file headers
BUFFER_HOOK_SIG(nickel_new_file_hook)
{
	Scratch_Block scratch(app);
	
	String_Const_u8 file_name = push_buffer_base_name(app, scratch, buffer_id);
	if (!string_match(string_postfix(file_name, 2), string_u8_litexpr(".h"))) {
		return(0);
	}
	
	List_String_Const_u8 guard_list = {};
	for (u64 i = 0; i < file_name.size; ++i){
		u8 c[2] = {};
		u64 c_size = 1;
		u8 ch = file_name.str[i];
		if ('A' <= ch && ch <= 'Z'){
			c_size = 2;
			c[0] = '_';
			c[1] = ch;
		}
		else if ('0' <= ch && ch <= '9'){
			c[0] = ch;
		}
		else if ('a' <= ch && ch <= 'z'){
			c[0] = ch - ('a' - 'A');
		}
		else{
			c[0] = '_';
		}
		String_Const_u8 part = push_string_copy(scratch, SCu8(c, c_size));
		string_list_push(scratch, &guard_list, part);
	}
	String_Const_u8 guard = string_list_flatten(scratch, guard_list);
	
	Date_Time date_time = system_now_date_time_universal();
	String_Const_u8 date_string = date_time_format(scratch, "month day yyyy h:mimi ampm", &date_time);
	
	String_Const_u8 header_text = push_u8_stringf
	(scratch,
	 "//**********************************************************\n"
	 "// Date: %.*s\n"
	 "// Creator: Daniel Nickel\n"
	 "// Notice: Copyright (C) Daniel Nickel, All Rights Reserved.\n"
	 "// File: %.*s\n"
	 "//**********************************************************\n"
	 "\n"
	 "#ifndef __%.*s_\n"
	 "#define __%.*s_\n"
	 "\n\n\n"
	 "#endif // __%.*s_",
	 string_expand(date_string), string_expand(file_name),
	 string_expand(guard), string_expand(guard), string_expand(guard)
	 );
	
	buffer_replace_range(app, buffer_id, Ii64(0, 0), header_text);
	
	return(0);
}

//~ NOTE(nickel): my custom command stuff, also adds \" to the ned opf the command
function void
nickel_execute_command(Application_Links* app, const char* command)
{
	Scratch_Block scratch(app);
	Query_Bar_Group group(app);
	
	// TODO(daniel): fix it
	out_buffer_space[0] = '*';
	out_buffer_space[1] = 'c';
	out_buffer_space[2] = 'o';
	out_buffer_space[3] = 'm';
	out_buffer_space[4] = 'm';
	out_buffer_space[5] = 'a';
	out_buffer_space[6] = 'n';
	out_buffer_space[7] = 'd';
	out_buffer_space[8] = '*';
	out_buffer_space[9] = 0;
	
	u8 command_value[1024] = { 0 };
	u8 command_full[1024] = { 0 };
	u64 command_full_size = strlen(command);
	memcpy(command_full, command, command_full_size);
	u64 i = 0;
	
	Query_Bar bar_cmd = {};
	bar_cmd.prompt = string_u8_litexpr("Command: ");
	bar_cmd.string = SCu8(command_value, (u64)0);
	bar_cmd.string_capacity = sizeof(command_value);
	if (!query_user_string(app, &bar_cmd)) return;
	bar_cmd.string.size = clamp_top(bar_cmd.string.size, sizeof(command_value) - 1);
	
	while (command_value[i] != 0)
	{
		command_full[i + command_full_size] = command_value[i];
		++i;
	}
	i = 0;
	
	while (command_full[i] != 0)
	{
		command_space[i] = command_full[i];
		++i;
	}
	command_space[i] = (u8)'\"';
	
	String_Const_u8 hot = push_hot_directory(app, scratch);
	{
		u64 size = clamp_top(hot.size, sizeof(hot_directory_space));
		block_copy(hot_directory_space, hot.str, size);
		hot_directory_space[hot.size] = 0;
	}
	
	execute_previous_cli(app);
}

// NOTE(nickel): man page command
CUSTOM_COMMAND_SIG(nickel_open_man_docs)
CUSTOM_DOC("Opens man page of the command you type in")
{
	nickel_execute_command(app, "wsl -e bash -c \"man ");
}

// NOTE(nickel): man page command
CUSTOM_COMMAND_SIG(nickel_search_with_grep)
CUSTOM_DOC("Searches with ripgrep (rg.exe) for the string typed in")
{
	nickel_execute_command(app, "rg -i \"");
}