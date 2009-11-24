BEGIN{AF=0; BX=0; SE=0; CSC_TIMING=0}
$0!~/csc_timing/&&$0!~/\/BX/&&$0!~/\/AF/{print $0}
$0~/\/AF/{if(AF==0) print "&nbsp;&nbsp;&nbsp;Optical link error; <font color=red>please, check linked plot and note which link has error</font>:\n<br>"$0; else print $0;}
$0~/\/AF/{AF=1}
$0~/csc_timing/{if(CSC_TIMING==0) print "&nbsp;&nbsp;&nbsp; Trigger primitives from one or more chambers are out of time (non-expert shifter can ignore this error):\n<br>"$0; else print $0;}
$0~/csc_timing/{CSC_TIMING=1}
$0~/BX/{if(BX==0) print "&nbsp;&nbsp;&nbsp; Problem of synchronization in one of peripheral crates (non-expert shifter can ignore this error):\n<br>"$0; else print $0;}
$0~/BX/{BX=1}
$0~/SE/{if(SE==0) print "&nbsp;&nbsp;&nbsp; Communication problem between MPC and the Track-Finder; <font color=red>please, inform the CSCTF on-call expert</font>:\n<br>"$0; else $0;}
$0~/SE/{SE=1}
