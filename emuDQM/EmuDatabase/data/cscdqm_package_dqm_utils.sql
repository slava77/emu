
  CREATE OR REPLACE PACKAGE BODY "CMS_VALDAS"."DQM_UTILS" AS

  function get_sum(p_arr in dqm_uint_array) return NUMBER AS
    l_sum number := 0;
  BEGIN
    for i in p_arr.first..p_arr.last loop
      l_sum := l_sum + i;
    end loop;
    RETURN l_sum;
  END get_sum;

  function sigfail(p_n_all in INTEGER, p_n_obs in INTEGER, p_rate in NUMBER, p_sig in NUMBER) RETURN NUMBER AS
    l_rate  NUMBER := p_rate;
    l_mrate NUMBER := p_n_obs / p_n_all;
    l_a     BINARY_DOUBLE := 1.0;
    l_b     BINARY_DOUBLE := 1.0;
  begin

    if l_rate <= 0 then 
      l_rate := 0.00001; 
    elsif l_rate >= 1 then
      l_rate := 0.9999;
    end if;

    if p_n_obs > 0 then
      l_a := l_mrate / l_rate;
      l_a := power(l_a, p_n_obs);
    end if;

    if l_a = binary_double_infinity then
      return 1;
    end if;

    if p_n_all > p_n_obs then
      l_b := (1 - l_mrate) / (1 - l_rate);
      l_b := power(l_b, p_n_all - p_n_obs);
    end if;

    if l_b = binary_double_infinity then
      return 1;
    end if;

    if p_sig < sqrt(2 * log(10, l_a * l_b)) then
      return 1;
    end if;
    
    return 0;

  end;
  
  function sigfail(p_n_all in INTEGER, p_n_obs in INTEGER, p_sig in NUMBER) RETURN NUMBER as
    l_a   binary_double := 1.0;
    l_sig binary_double := 0;
  begin
  
    if p_n_all > p_n_obs or p_n_all = 0 then 
      return 0; 
    end if;
  
    l_a := log(10, p_n_obs / p_n_all);
    l_sig := sqrt(2.0 * (p_n_obs * (l_a - 1) + p_n_all));
    
    if l_sig = binary_double_nan then
      return 0;
    end if;
    
    if l_sig > p_sig then
      return 1;
    end if;
  
    return 0;
  end;

  function check_error(p_n_all in INTEGER, p_n_obs in INTEGER, p_range_low in number, p_range_high in number, p_sig in NUMBER) RETURN NUMBER as
    l_range      number := p_n_obs / p_n_all;
  begin
    
    if (l_range > p_range_low or p_range_low is null) and (l_range <= p_range_high or p_range_high is null) then
      
      l_range := p_range_high;
      if l_range is null then
        l_range := p_range_low;
      end if;
      
      if sigfail(p_n_all, p_n_obs, l_range, p_sig) = 1 then
        return 1;
      end if;
      
    end if;
    
    return 0;
  end;

END DQM_UTILS;
/
 
