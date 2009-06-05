----------------------------------------------------------
-- Views
----------------------------------------------------------

create or replace view dqm_run_ddu_format_err as
select
  rdu_run_number run_number,
  rdu_ddu_number ddu_number,
  duf_bit bit,
  max(svr_severity) severity
from
  dqm_run_ddu 
    join dqm_run_ddu_format on (rdu_run_number = rdf_run_number and rdu_ddu_number = rdf_ddu_number)
      join dqm_ddu_format on (rdf_duf_bit = duf_bit)
        join dqm_severity_group on (svg_id = duf_svg_id)
          join dqm_severity_range on (svr_svg_id = svg_id)
where
  duf_svg_id is not null and
  dqm_utils.check_error(rdu_events, rdf_value, svr_range_low, svr_range_high, svg_sigfail) = 1
group by
  rdu_run_number,
  rdu_ddu_number,
  duf_bit;  

create or replace view dqm_run_ddu_trailer_err as
select
  rdu_run_number run_number,
  rdu_ddu_number ddu_number,
  dut_bit bit,
  max(svr_severity) severity
from
  dqm_run_ddu 
    join dqm_run_ddu_trailer on (rdu_run_number = rdt_run_number and rdu_ddu_number = rdt_ddu_number)
      join dqm_ddu_trailer on (rdt_dut_bit = dut_bit)
        join dqm_severity_group on (svg_id = dut_svg_id)
          join dqm_severity_range on (svr_svg_id = svg_id)
where
  dut_svg_id is not null and
  dqm_utils.check_error(rdu_events, rdt_value, svr_range_low, svr_range_high, svg_sigfail) = 1
group by
  rdu_run_number,
  rdu_ddu_number,
  dut_bit;  