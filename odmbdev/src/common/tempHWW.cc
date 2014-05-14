// addr = 0x780000; data = 0x0000; 
// printf("Calling:  vme_controller(%d,%06x,&%04x,{%02x,%02x}) \n ", irdwr, (addr & 0xffffff), (data & 0xffff), (rcv[0] & 0xff), (rcv[1] & 0xff)); 
// crate_->vmeController()->vme_controller(irdwr,addr,&data,rcv); 


// Reading from file: /home/cscme11/TriDAS/emu/odmbdev/commands/test_realdata_all7.txt
// Calling this line:  W  3010 1       Reprogram DCFEB FIFO
// Calling:   vme_controller(3,783010,&0001,{b8,1f})  
irdwr = 3; addr = 0x783010; data = 0x0001; 
printf("Calling:  vme_controller(%d,%06x,&%04x,{%02x,%02x}) \n ", irdwr, (addr & 0xffffff), (data & 0xffff), (rcv[0] & 0xff), (rcv[1] & 0xff)); 
crate_->vmeController()->vme_controller(irdwr,addr,&data,rcv); 
// Calling this line:  W  3000 100     Reset ODMB Registers
// Calling:   vme_controller(3,783000,&0100,{b8,1f})  
irdwr = 3; addr = 0x783000; data = 0x0100; 
printf("Calling:  vme_controller(%d,%06x,&%04x,{%02x,%02x}) \n ", irdwr, (addr & 0xffffff), (data & 0xffff), (rcv[0] & 0xff), (rcv[1] & 0xff)); 
crate_->vmeController()->vme_controller(irdwr,addr,&data,rcv); 
// Calling this line:  W  3000  300		   Reset
// Calling:   vme_controller(3,783000,&0300,{b8,1f})  
irdwr = 3; addr = 0x783000; data = 0x0300; 
printf("Calling:  vme_controller(%d,%06x,&%04x,{%02x,%02x}) \n ", irdwr, (addr & 0xffffff), (data & 0xffff), (rcv[0] & 0xff), (rcv[1] & 0xff)); 
crate_->vmeController()->vme_controller(irdwr,addr,&data,rcv); 
// Calling this line:  W  20   18	Select FIFO 4 [DDU data]
// Calling:   vme_controller(3,780020,&0018,{b8,1f})  
irdwr = 3; addr = 0x780020; data = 0x0018; 
printf("Calling:  vme_controller(%d,%06x,&%04x,{%02x,%02x}) \n ", irdwr, (addr & 0xffffff), (data & 0xffff), (rcv[0] & 0xff), (rcv[1] & 0xff)); 
crate_->vmeController()->vme_controller(irdwr,addr,&data,rcv); 
// Calling this line:  W  3000  200		   Set real data and internal triggers
// Calling:   vme_controller(3,783000,&0200,{b8,1f})  
irdwr = 3; addr = 0x783000; data = 0x0200; 
printf("Calling:  vme_controller(%d,%06x,&%04x,{%02x,%02x}) \n ", irdwr, (addr & 0xffffff), (data & 0xffff), (rcv[0] & 0xff), (rcv[1] & 0xff)); 
crate_->vmeController()->vme_controller(irdwr,addr,&data,rcv); 
// Calling this line:  R  441C  0			   Read KILL
// Calling:   vme_controller(2,78441c,&0000,{b8,1f})    ==> rcv[1,0] = 01 ff
irdwr = 2; addr = 0x78441C; data = 0x0000; 
printf("Calling:  vme_controller(%d,%06x,&%04x,{%02x,%02x}) \n ", irdwr, (addr & 0xffffff), (data & 0xffff), (rcv[0] & 0xff), (rcv[1] & 0xff)); 
crate_->vmeController()->vme_controller(irdwr,addr,&data,rcv); 
// Calling this line:  W  401C  0			     Set KILL
// Calling:   vme_controller(3,78401c,&0000,{ff,01})  
irdwr = 3; addr = 0x78401C; data = 0x0000; 
printf("Calling:  vme_controller(%d,%06x,&%04x,{%02x,%02x}) \n ", irdwr, (addr & 0xffffff), (data & 0xffff), (rcv[0] & 0xff), (rcv[1] & 0xff)); 
crate_->vmeController()->vme_controller(irdwr,addr,&data,rcv); 
// Calling this line:  W  3010  10			   Send test L1A(_MATCH) to all DCFEBs
// Calling:   vme_controller(3,783010,&0010,{ff,01})  
irdwr = 3; addr = 0x783010; data = 0x0010; 
printf("Calling:  vme_controller(%d,%06x,&%04x,{%02x,%02x}) \n ", irdwr, (addr & 0xffffff), (data & 0xffff), (rcv[0] & 0xff), (rcv[1] & 0xff)); 
crate_->vmeController()->vme_controller(irdwr,addr,&data,rcv); 
// Calling this line:  R  33FC			    0	Read L1A_COUNTER
// Calling:   vme_controller(2,7833fc,&0000,{ff,01})    ==> rcv[1,0] = 00 01
irdwr = 2; addr = 0x7833FC; data = 0x0000; 
printf("Calling:  vme_controller(%d,%06x,&%04x,{%02x,%02x}) \n ", irdwr, (addr & 0xffffff), (data & 0xffff), (rcv[0] & 0xff), (rcv[1] & 0xff)); 
crate_->vmeController()->vme_controller(irdwr,addr,&data,rcv); 
// Calling this line:  R  327C  0			     Read L1A_MATCH_CNT(7)
// Calling:   vme_controller(2,78327c,&0000,{01,00})    ==> rcv[1,0] = 00 01
irdwr = 2; addr = 0x78327C; data = 0x0000; 
printf("Calling:  vme_controller(%d,%06x,&%04x,{%02x,%02x}) \n ", irdwr, (addr & 0xffffff), (data & 0xffff), (rcv[0] & 0xff), (rcv[1] & 0xff)); 
crate_->vmeController()->vme_controller(irdwr,addr,&data,rcv); 
// Calling this line:  R  337C  0			     Read LCT_L1A_GAP(7)
// Calling:   vme_controller(2,78337c,&0000,{01,00})    ==> rcv[1,0] = 00 63
irdwr = 2; addr = 0x78337C; data = 0x0000; 
printf("Calling:  vme_controller(%d,%06x,&%04x,{%02x,%02x}) \n ", irdwr, (addr & 0xffffff), (data & 0xffff), (rcv[0] & 0xff), (rcv[1] & 0xff)); 
crate_->vmeController()->vme_controller(irdwr,addr,&data,rcv); 
// Calling this line:  R  347C  0			     Number of received packets [DCFEB 7]
// Calling:   vme_controller(2,78347c,&0000,{63,00})    ==> rcv[1,0] = 00 01
irdwr = 2; addr = 0x78347C; data = 0x0000; 
printf("Calling:  vme_controller(%d,%06x,&%04x,{%02x,%02x}) \n ", irdwr, (addr & 0xffffff), (data & 0xffff), (rcv[0] & 0xff), (rcv[1] & 0xff)); 
crate_->vmeController()->vme_controller(irdwr,addr,&data,rcv); 
// Calling this line:  W  5010 7			     Select DCFEB FIFO 7
// Calling:   vme_controller(3,785010,&0007,{01,00})  
irdwr = 3; addr = 0x785010; data = 0x0007; 
printf("Calling:  vme_controller(%d,%06x,&%04x,{%02x,%02x}) \n ", irdwr, (addr & 0xffffff), (data & 0xffff), (rcv[0] & 0xff), (rcv[1] & 0xff)); 
crate_->vmeController()->vme_controller(irdwr,addr,&data,rcv); 
// Calling this line:  R  5014 0			   Read which DCFEB FIFO is selected
// Calling:   vme_controller(2,785014,&0000,{01,00})    ==> rcv[1,0] = 00 07
irdwr = 2; addr = 0x785014; data = 0x0000; 
printf("Calling:  vme_controller(%d,%06x,&%04x,{%02x,%02x}) \n ", irdwr, (addr & 0xffffff), (data & 0xffff), (rcv[0] & 0xff), (rcv[1] & 0xff)); 
crate_->vmeController()->vme_controller(irdwr,addr,&data,rcv); 
// Calling this line:  R  500C 0			     Read word count of DCFEB FIFO
// Calling:   vme_controller(2,78500c,&0000,{07,00})    ==> rcv[1,0] = 03 22
irdwr = 2; addr = 0x78500C; data = 0x0000; 
printf("Calling:  vme_controller(%d,%06x,&%04x,{%02x,%02x}) \n ", irdwr, (addr & 0xffffff), (data & 0xffff), (rcv[0] & 0xff), (rcv[1] & 0xff)); 
crate_->vmeController()->vme_controller(irdwr,addr,&data,rcv); 
// Calling this line:  R  5000 0			     Read DCFEB FIFO
// Calling:   vme_controller(2,785000,&0000,{22,03})    ==> rcv[1,0] = 00 01
irdwr = 2; addr = 0x785000; data = 0x0000; 
printf("Calling:  vme_controller(%d,%06x,&%04x,{%02x,%02x}) \n ", irdwr, (addr & 0xffffff), (data & 0xffff), (rcv[0] & 0xff), (rcv[1] & 0xff)); 
crate_->vmeController()->vme_controller(irdwr,addr,&data,rcv); 
// Calling this line:  R  5000 0			     Read DCFEB FIFO
// Calling:   vme_controller(2,785000,&0000,{01,00})    ==> rcv[1,0] = 00 01
irdwr = 2; addr = 0x785000; data = 0x0000; 
printf("Calling:  vme_controller(%d,%06x,&%04x,{%02x,%02x}) \n ", irdwr, (addr & 0xffffff), (data & 0xffff), (rcv[0] & 0xff), (rcv[1] & 0xff)); 
crate_->vmeController()->vme_controller(irdwr,addr,&data,rcv); 
// Calling this line:  R  5000 0			     Read DCFEB FIFO
// Calling:   vme_controller(2,785000,&0000,{01,00})    ==> rcv[1,0] = 44 03
irdwr = 2; addr = 0x785000; data = 0x0000; 
printf("Calling:  vme_controller(%d,%06x,&%04x,{%02x,%02x}) \n ", irdwr, (addr & 0xffffff), (data & 0xffff), (rcv[0] & 0xff), (rcv[1] & 0xff)); 
crate_->vmeController()->vme_controller(irdwr,addr,&data,rcv); 
// Calling this line:  R  5000 0			     Read DCFEB FIFO
// Calling:   vme_controller(2,785000,&0000,{03,44})    ==> rcv[1,0] = 44 33
irdwr = 2; addr = 0x785000; data = 0x0000; 
printf("Calling:  vme_controller(%d,%06x,&%04x,{%02x,%02x}) \n ", irdwr, (addr & 0xffffff), (data & 0xffff), (rcv[0] & 0xff), (rcv[1] & 0xff)); 
crate_->vmeController()->vme_controller(irdwr,addr,&data,rcv); 
// Calling this line:  R  5000 0			     Read DCFEB FIFO
// Calling:   vme_controller(2,785000,&0000,{33,44})    ==> rcv[1,0] = 44 13
irdwr = 2; addr = 0x785000; data = 0x0000; 
printf("Calling:  vme_controller(%d,%06x,&%04x,{%02x,%02x}) \n ", irdwr, (addr & 0xffffff), (data & 0xffff), (rcv[0] & 0xff), (rcv[1] & 0xff)); 
crate_->vmeController()->vme_controller(irdwr,addr,&data,rcv); 
// Calling this line:  W  5020 7f      Reset DCFEB FIFOs
// Calling:   vme_controller(3,785020,&007f,{13,44})  
irdwr = 3; addr = 0x785020; data = 0x007F; 
printf("Calling:  vme_controller(%d,%06x,&%04x,{%02x,%02x}) \n ", irdwr, (addr & 0xffffff), (data & 0xffff), (rcv[0] & 0xff), (rcv[1] & 0xff)); 
crate_->vmeController()->vme_controller(irdwr,addr,&data,rcv); 




