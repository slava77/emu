
  CREATE OR REPLACE PACKAGE BODY "CMS_VALDAS"."DQM_UPDATE" AS

  procedure run_remove(p_run_number number) AS
    l_exists NUMBER := 0;
  BEGIN
    
    begin
      select run_number into l_exists from dqm_run where run_number = p_run_number;
    exception when NO_DATA_FOUND then
      return;
    end;

    delete from dqm_run_ddu_input where rdi_run_number = p_run_number;
    delete from dqm_run_ddu_trailer where rdt_run_number = p_run_number;
    delete from dqm_run_ddu_format where rdf_run_number = p_run_number;
    delete from dqm_run_ddu where rdu_run_number = p_run_number;
    
    delete from dqm_run_cfeb_layer where rcl_run_number = p_run_number;
    delete from dqm_run_chamber_layer where lay_run_number = p_run_number;
    delete from dqm_run_cfeb where rcc_run_number = p_run_number;
    delete from dqm_run_chamber_format where rcf_run_number = p_run_number;
    delete from dqm_run_chamber where rch_run_number = p_run_number;
    
    delete from dqm_run where run_number = p_run_number;
    
  END run_remove;

  procedure run_cfeb(p_run_number number, p_cha_id number, p_cfeb number, p_out_off_syn number) as
    l_ret number := 0;
  begin
    update DQM_RUN_CFEB set RCC_L1A_OUT_OFF_SYN = p_out_off_syn where rcc_run_number = p_run_number and rcc_cha_id = p_cha_id and rcc_cfeb = p_cfeb returning rcc_run_number into l_ret;
    if l_ret <> p_run_number then
      insert into DQM_RUN_CFEB (RCC_L1A_OUT_OFF_SYN, rcc_run_number, rcc_cha_id, rcc_cfeb) values (p_out_off_syn, p_run_number, p_cha_id, p_cfeb);
    end if;
  end;

  procedure run_cfeb(p_run_number number, p_cha_id number, p_cfeb number, p_dav_events number) as
    l_ret number := 0;
  begin
    update DQM_RUN_CFEB set RCC_DAV_EVENTS = p_dav_events where rcc_run_number = p_run_number and rcc_cha_id = p_cha_id and rcc_cfeb = p_cfeb returning rcc_run_number into l_ret;
    if l_ret <> p_run_number then
      insert into DQM_RUN_CFEB (rcc_dav_events, rcc_run_number, rcc_cha_id, rcc_cfeb) values (p_dav_events, p_run_number, p_cha_id, p_cfeb);
    end if;
  end;
  
  procedure run_cfeb(p_run_number number, p_cha_id number, p_cfeb number, p_fifo_full number, p_start_timeout number, p_end_timeout number) as
    l_ret number := 0;
  begin
    update DQM_RUN_CFEB set rcc_fifo_full = p_fifo_full, rcc_start_timeout = p_start_timeout, rcc_end_timeout = p_end_timeout where rcc_run_number = p_run_number and rcc_cha_id = p_cha_id and rcc_cfeb = p_cfeb returning rcc_run_number into l_ret;
    if l_ret <> p_run_number then
      insert into DQM_RUN_CFEB (rcc_fifo_full, rcc_start_timeout, rcc_end_timeout, rcc_run_number, rcc_cha_id, rcc_cfeb) values (p_fifo_full, p_start_timeout, p_end_timeout, p_run_number, p_cha_id, p_cfeb);
    end if;
  end;

  procedure run_refresh(p_run_number number) as
  begin
  
    update 
      dqm_run_cfeb_layer 
    set 
      rcl_sca_sum = dqm_utils.get_sum(rcl_sca_channel_data) 
    where 
      rcl_run_number = p_run_number and 
      rcl_sca_channel_data is not null;
      
    update 
      dqm_run_cfeb_layer 
    set 
      rcl_cmp_sum = dqm_utils.get_sum(rcl_cmp_channel_data) 
    where 
      rcl_run_number = p_run_number and 
      rcl_cmp_channel_data is not null;
    
    update 
      dqm_run_chamber_layer t
    set 
      lay_sca_sum = (select sum(rcl_sca_sum) from dqm_run_cfeb_layer s where s.rcl_run_number = t.lay_run_number and s.rcl_cha_id = t.lay_cha_id and s.rcl_layer = t.lay_layer)
    where
      t.lay_run_number = p_run_number;

    update 
      dqm_run_chamber_layer t
    set 
      lay_cmp_sum = (select sum(rcl_cmp_sum) from dqm_run_cfeb_layer s where s.rcl_run_number = t.lay_run_number and s.rcl_cha_id = t.lay_cha_id and s.rcl_layer = t.lay_layer)
    where
      t.lay_run_number = p_run_number;

    update 
      dqm_run_ddu_format t
    set 
      t.rdf_iserr = nvl((select severity from dqm_run_ddu_format_err s where s.run_number = t.rdf_run_number and s.ddu_number = t.rdf_ddu_number and s.bit = t.rdf_duf_bit), 0)
    where
      t.rdf_run_number = p_run_number;

    update 
      dqm_run_ddu_trailer t
    set 
      t.rdt_iserr = nvl((select severity from dqm_run_ddu_trailer_err s where s.run_number = t.rdt_run_number and s.ddu_number = t.rdt_ddu_number and s.bit = t.rdt_dut_bit), 0)
    where
      t.rdt_run_number = p_run_number;

  end;

END DQM_UPDATE;
/
 
