#ifndef _event_display_common_h_
#define _event_display_common_h_

void wire_geom_();
void atrig_wire_geom_();
void init_display_();
void read_clct_patterns_();
void read_alct_patterns_();

void unpack_data_();

void display_wires_strips_(int* autoscale);
void display_alct_time_();
void display_alct_time_cc();
void j_display_cc();
void display_clct_time_();
void display_sca_();
void display_atrig_();
void display_ctrig_();
void display_scint_wires_();
void no_active_mess_();

void make_wires_strips_hardcopy_(int* autoscale);
void make_alct_time_hardcopy_();
void make_clct_time_hardcopy_();
void make_sca_hardcopy_();
void make_atrig_hardcopy_();
void make_ctrig_hardcopy_();
void make_scint_wires_hardcopy_();

void open_hardcopy_file_(int *status);
void close_hardcopy_file_();

#endif /* event_display_common_h_ */
