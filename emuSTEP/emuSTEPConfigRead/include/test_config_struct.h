
#ifndef _test_config_struct_h_
#define _test_config_struct_h_

typedef struct test_configuration_11
{
	int events_total;
} 
test_config_11;

typedef struct test_configuration_12
{
	int events_per_strip;
	int alct_test_pulse_amp_11;
	int alct_test_pulse_amp_12;
	int alct_test_pulse_amp_13;
	int alct_test_pulse_amp_21;
	int alct_test_pulse_amp_22;
	int alct_test_pulse_amp_31;
	int alct_test_pulse_amp_32;
	int alct_test_pulse_amp_41;
	int alct_test_pulse_amp_42;
	
} 
test_config_12;

typedef struct test_configuration_13
{
	int events_per_threshold;
	int thresholds_per_tpamp;
	int threshold_first;	 
	int threshold_step;	 
	int tpamps_per_run;	 
	int tpamp_first;     
	int tpamp_step;	     
} 
test_config_13;

typedef struct test_configuration_14
{
	int alct_test_pulse_amp;
	int events_per_delay;
	int delays_per_run; 
	int delay_first;	 
	int delay_step;	 
} 
test_config_14;

typedef struct test_configuration_15
{
	int events_total;
} 
test_config_15;

typedef struct test_configuration_16
{
	int events_per_layer;
	int alct_test_pulse_amp;
} 
test_config_16;

typedef struct test_configuration_17
{
	int dmb_test_pulse_amp;
	int events_per_delay; 
	int delays_per_strip; 
	int delay_first;      
	int delay_step;       
	int strips_per_run;   
	int strip_first;      
	int strip_step;       
} 
test_config_17;


typedef struct test_configuration_17b
{
	int dmb_test_pulse_amp;
	int events_per_pulsedac; 
	int pulse_dac_settings; 
	int dac_first;      
	int dac_step;       
	int strips_per_run;   
	int strip_first;      
	int strip_step;       
} 
test_config_17b;

typedef struct test_configuration_18
{
	int events_total;
} 
test_config_18;

typedef struct test_configuration_19
{
	int scale_turnoff;
	int range_turnoff;
	int events_per_thresh;
	int threshs_per_tpamp;
	int thresh_first;
	int thresh_step;
	int dmb_tpamps_per_strip;
	int dmb_tpamp_first;
	int dmb_tpamp_step;
	int strips_per_run;
	int strip_first;
	int strip_step;
} 
test_config_19;

typedef struct test_configuration_21
{
	int dmb_test_pulse_amp;
	int cfeb_threshold;    
	int events_per_hstrip; 
	int hstrips_per_run;   
	int hstrip_first;      
	int hstrip_step;         
} 
test_config_21;

typedef struct test_configuration_25
{
        int trig_settings;
        int events_per_trig_set;            
} 
test_config_25;

typedef struct test_configuration_30
{
	int events_per_delay; 
	int tmb_l1a_delays_per_run;
	int tmb_l1a_delay_first;
	int tmb_l1a_delay_step;
} 
test_config_30;

typedef struct test_config_structure
{
	test_config_11 t11;
	test_config_12 t12;
	test_config_13 t13;
	test_config_14 t14;
	test_config_15 t15;
	test_config_16 t16;
	test_config_17 t17;
	test_config_17b t17b;
	test_config_18 t18;
	test_config_19 t19;
	test_config_21 t21;
	test_config_25 t25;
	test_config_30 t30;
}
test_config_struct;

#endif
