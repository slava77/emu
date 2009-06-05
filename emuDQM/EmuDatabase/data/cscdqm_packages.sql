
  CREATE OR REPLACE PACKAGE "CMS_VALDAS"."DQM_UPDATE" AS

  procedure run_remove(p_run_number number);
  procedure run_cfeb(p_run_number number, p_cha_id number, p_cfeb number, p_out_off_syn number);
  procedure run_cfeb(p_run_number number, p_cha_id number, p_cfeb number, p_dav_events number);
  procedure run_cfeb(p_run_number number, p_cha_id number, p_cfeb number, p_fifo_full number, p_start_timeout number, p_end_timeout number);
  procedure run_refresh(p_run_number number);
 
END DQM_UPDATE;
/
 

  CREATE OR REPLACE PACKAGE "CMS_VALDAS"."DQM_UTILS" AS

  function get_sum(p_arr in dqm_uint_array) return NUMBER;
  
  function check_error(p_n_all in INTEGER, p_n_obs in INTEGER, p_range_low in number, p_range_high in number, p_sig in NUMBER) RETURN NUMBER;
  
END DQM_UTILS;
/
 
