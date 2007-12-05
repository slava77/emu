#include "EmuTFfiller.h"
#include "TH1.h"
#include "TH2.h"

#include "EventFilter/CSCTFRawToDigi/src/CSCTFEvent.cc"
#include "EventFilter/CSCTFRawToDigi/src/CSCSPEvent.cc"
#include "EventFilter/CSCTFRawToDigi/src/CSCSPRecord.cc"
#include "EventFilter/CSCTFRawToDigi/src/CSCSPTrailer.h"
#include "IORawData/CSCCommissioning/src/FileReaderDDU.h"

#include "EmuTFtiming.cc"

#include <math.h>
#include <iostream>
using namespace std;

void EmuTFfiller::fill(const unsigned short *buffer, unsigned int size, unsigned int flag) throw() {
	//
	event_status = CLEAR;

	if(!tf.isBooked() ){
		tf.book();
		std::cout<<"Booking of general histograms"<<std::endl;
	}

	// Basic checks for DDU-level corruptions
	if( flag != FileReaderDDU::Type1 ){
		TH2F *integrity = (TH2F*)tf.get("integrity");
		if( integrity ){
			if( flag&FileReaderDDU::Type2 ) integrity->Fill(nevents,4);
			if( flag&FileReaderDDU::Type3 ) integrity->Fill(nevents,5);
			if( flag&FileReaderDDU::Type4 ) integrity->Fill(nevents,6);
			if( flag&FileReaderDDU::Type5 || flag&FileReaderDDU::Type6 ) integrity->Fill(nevents,8);
			if( flag&FileReaderDDU::Type7 ) integrity->Fill(nevents,9);
			if( nevents%100==0 ) integrity->SetAxisRange(0, nevents+100);
		}
		nevents++;
		event_status |= DDU_CORRUPTION;
		return;
	}

	// DDU Status word (FMM)
	TH2F *DDU_status = (TH2F*)tf.get("DDU_status");
	if( DDU_status ){
		DDU_status->Fill(nevents,(buffer[size-4]&0x00F0)>>4);
		if( nevents%100==0 ) DDU_status->SetAxisRange(0, nevents);
	}

	// Swep out C-words
	unsigned short event[size];

	unsigned int index1=12, index2=12;
	if( size>=12 ){
		memcpy(event,buffer,12*sizeof(unsigned short));
		while( index1 < size-12 ){
			if( (buffer[index1]&0xF000)!=0xC000 ){
				event[index2] = buffer[index1];
				index1++;
				index2++;
			} else {
				index1++;
			}
		}
		memcpy(&event[index2],&buffer[index1],12*sizeof(unsigned short));
		size = index2+12;
	} else {
		memcpy(event,buffer,size*sizeof(unsigned short));
	}

	if( index1!=index2 ){
		TH2F *integrity = (TH2F*)tf.get("integrity");
		if( integrity ){
			integrity->Fill(nevents,7);
			if( nevents%100==0 ) integrity->SetAxisRange(0, nevents);
		}
		if( nevents%100==0 ) integrity->SetAxisRange(0, nevents);
		event_status |= C_WORDS;
	}

	CSCTFEvent tfEvent;
	unsigned int error = tfEvent.unpack(event,size);

	TH2F *integrity = (TH2F*)tf.get("integrity");
	if( integrity ){
		if( error&CSCTFEvent::MISSING_HEADER ) integrity->Fill(nevents,0);
		if( error&CSCTFEvent::MISSING_TRAILER) integrity->Fill(nevents,1);
		if( error&CSCTFEvent::OUT_OF_BUFFER  ) integrity->Fill(nevents,2);
		if( error&CSCTFEvent::WORD_COUNT     ) integrity->Fill(nevents,3);
		if( nevents%100==0 ) integrity->SetAxisRange(0, nevents);
	}

	if( error ){
		nevents++;
		event_status |= SP_CORRUPTION;
		return;
	}


	vector<CSCSPEvent> SPs = tfEvent.SPs();

	for(vector<CSCSPEvent>::const_iterator spPtr=SPs.begin(); spPtr!=SPs.end(); spPtr++){
		// Container for storing all LCTs from SP event 
		EmuTFtiming shared_hits;

		unsigned short sp = spPtr->header().sector() + ( spPtr->header().endcap() ? 0 : 6 );
		if(!tf.isBooked(sp) ){
			tf.book(sp);
			std::cout<<"Booking histograms for SP: "<<sp<<" (sector="<<spPtr->header().sector()<<" & endcap="<<spPtr->header().endcap()<<")"<<std::endl;
		}

		TH1F *L1A_increment  = (TH1F*)tf.get("L1A_increment",sp);
		if( L1A_increment && prev_l1a[sp]>=0 ) L1A_increment->Fill(spPtr->header().L1A()-prev_l1a[sp]);
		prev_l1a[sp] = spPtr->header().L1A();

		TH2F *L1A_queue_size = (TH2F*)tf.get("L1A_queue_size",sp);
		if( L1A_queue_size ){
			L1A_queue_size->Fill(nevents,spPtr->trailer().l1a_queue_size() + 256*spPtr->trailer().l1a_fifo_full());
			if( nevents%100==0 ) L1A_queue_size->SetAxisRange(0, nevents);
		}

		TH1F *L1A_BXN    = (TH1F*)tf.get("L1A_BXN",   sp);
		if( L1A_BXN ) L1A_BXN->Fill(spPtr->header().BXN());

		TH2F *FMM_status = (TH2F*)tf.get("FMM_status",sp);
		if( FMM_status ){
			if( spPtr->header().status()&CSCSPHeader::WOF   ) FMM_status->Fill(nevents,0);
			if( spPtr->header().status()&CSCSPHeader::OSY   ) FMM_status->Fill(nevents,1);
			if( spPtr->header().status()&CSCSPHeader::BUZY  ) FMM_status->Fill(nevents,2);
			if( spPtr->header().status()&CSCSPHeader::READY ) FMM_status->Fill(nevents,3);
			if( spPtr->header().status()&CSCSPHeader::FA_OSY) FMM_status->Fill(nevents,4);
			if( spPtr->header().status()&CSCSPHeader::SP_OSY) FMM_status->Fill(nevents,5);
			if( nevents%10==0 ) FMM_status->SetAxisRange(0, nevents);
		}

		TH1F *TrackCounter = (TH1F*)tf.get("TrackCounter",sp);
		if( TrackCounter ) TrackCounter->Fill(spPtr->counters().track_counter());
		
		TH1F *OrbitCounter = (TH1F*)tf.get("OrbitCounter",sp);
		if( OrbitCounter ) OrbitCounter->Fill(spPtr->counters().orbit_counter());

		for(unsigned int tbin=0; tbin<spPtr->header().nTBINs(); tbin++){

			TH2F *SE = (TH2F*)tf.get("SE",sp);
			if( SE ){
				if( spPtr->record(tbin).SEs()&0x1   ) SE->Fill(0);
				if( spPtr->record(tbin).SEs()&0x2   ) SE->Fill(1);
				if( spPtr->record(tbin).SEs()&0x4   ) SE->Fill(2);
				if( spPtr->record(tbin).SEs()&0x8   ) SE->Fill(3);
				if( spPtr->record(tbin).SEs()&0x10  ) SE->Fill(4);
				if( spPtr->record(tbin).SEs()&0x20  ) SE->Fill(5);
				if( spPtr->record(tbin).SEs()&0x40  ) SE->Fill(6);
				if( spPtr->record(tbin).SEs()&0x80  ) SE->Fill(7);
				if( spPtr->record(tbin).SEs()&0x100 ) SE->Fill(8);
				if( spPtr->record(tbin).SEs()&0x200 ) SE->Fill(9);
				if( spPtr->record(tbin).SEs()&0x400 ) SE->Fill(10);
				if( spPtr->record(tbin).SEs()&0x800 ) SE->Fill(11);
				if( spPtr->record(tbin).SEs()&0x1000) SE->Fill(12);
				if( spPtr->record(tbin).SEs()&0x2000) SE->Fill(13);
				if( spPtr->record(tbin).SEs()&0x4000) SE->Fill(14);
				if( spPtr->record(tbin).SEs()&0x8000) SE->Fill(15);
			}
			TH2F *SM = (TH2F*)tf.get("SM",sp);
			if( SM ){
				if( spPtr->record(tbin).SMs()&0x1   ) SM->Fill(0);
				if( spPtr->record(tbin).SMs()&0x2   ) SM->Fill(1);
				if( spPtr->record(tbin).SMs()&0x4   ) SM->Fill(2);
				if( spPtr->record(tbin).SMs()&0x8   ) SM->Fill(3);
				if( spPtr->record(tbin).SMs()&0x10  ) SM->Fill(4);
				if( spPtr->record(tbin).SMs()&0x20  ) SM->Fill(5);
				if( spPtr->record(tbin).SMs()&0x40  ) SM->Fill(6);
				if( spPtr->record(tbin).SMs()&0x80  ) SM->Fill(7);
				if( spPtr->record(tbin).SMs()&0x100 ) SM->Fill(8);
				if( spPtr->record(tbin).SMs()&0x200 ) SM->Fill(9);
				if( spPtr->record(tbin).SMs()&0x400 ) SM->Fill(10);
				if( spPtr->record(tbin).SMs()&0x800 ) SM->Fill(11);
				if( spPtr->record(tbin).SMs()&0x1000) SM->Fill(12);
				if( spPtr->record(tbin).SMs()&0x2000) SM->Fill(13);
				if( spPtr->record(tbin).SMs()&0x4000) SM->Fill(14);
				if( spPtr->record(tbin).SMs()&0x8000) SM->Fill(15);
			}
			TH2F *AF = (TH2F*)tf.get("AF",sp);
			if( AF ){
				if( spPtr->record(tbin).AFs()&0x1   ) AF->Fill(0);
				if( spPtr->record(tbin).AFs()&0x2   ) AF->Fill(1);
				if( spPtr->record(tbin).AFs()&0x4   ) AF->Fill(2);
				if( spPtr->record(tbin).AFs()&0x8   ) AF->Fill(3);
				if( spPtr->record(tbin).AFs()&0x10  ) AF->Fill(4);
				if( spPtr->record(tbin).AFs()&0x20  ) AF->Fill(5);
				if( spPtr->record(tbin).AFs()&0x40  ) AF->Fill(6);
				if( spPtr->record(tbin).AFs()&0x80  ) AF->Fill(7);
				if( spPtr->record(tbin).AFs()&0x100 ) AF->Fill(8);
				if( spPtr->record(tbin).AFs()&0x200 ) AF->Fill(9);
				if( spPtr->record(tbin).AFs()&0x400 ) AF->Fill(10);
				if( spPtr->record(tbin).AFs()&0x800 ) AF->Fill(11);
				if( spPtr->record(tbin).AFs()&0x1000) AF->Fill(12);
				if( spPtr->record(tbin).AFs()&0x2000) AF->Fill(13);
				if( spPtr->record(tbin).AFs()&0x4000) AF->Fill(14);
				if( spPtr->record(tbin).AFs()&0x8000) AF->Fill(15);
				if( spPtr->record(tbin).AFs()&0x10000) AF->Fill(16);
				if( spPtr->record(tbin).AFs()&0x20000) AF->Fill(17);
			}
			TH2F *BX = (TH2F*)tf.get("BX",sp);
			if( BX ){
				if( spPtr->record(tbin).BXs()&0x1   ) BX->Fill(0);
				if( spPtr->record(tbin).BXs()&0x2   ) BX->Fill(1);
				if( spPtr->record(tbin).BXs()&0x4   ) BX->Fill(2);
				if( spPtr->record(tbin).BXs()&0x8   ) BX->Fill(3);
				if( spPtr->record(tbin).BXs()&0x10  ) BX->Fill(4);
				if( spPtr->record(tbin).BXs()&0x20  ) BX->Fill(5);
				if( spPtr->record(tbin).BXs()&0x40  ) BX->Fill(6);
				if( spPtr->record(tbin).BXs()&0x80  ) BX->Fill(7);
				if( spPtr->record(tbin).BXs()&0x100 ) BX->Fill(8);
				if( spPtr->record(tbin).BXs()&0x200 ) BX->Fill(9);
				if( spPtr->record(tbin).BXs()&0x400 ) BX->Fill(10);
				if( spPtr->record(tbin).BXs()&0x800 ) BX->Fill(11);
				if( spPtr->record(tbin).BXs()&0x1000) BX->Fill(12);
				if( spPtr->record(tbin).BXs()&0x2000) BX->Fill(13);
				if( spPtr->record(tbin).BXs()&0x4000) BX->Fill(14);
				if( spPtr->record(tbin).BXs()&0x8000) BX->Fill(15);
				if( spPtr->record(tbin).BXs()&0x10000) BX->Fill(16);
				if( spPtr->record(tbin).BXs()&0x20000) BX->Fill(17);
			}

			vector<CSCSP_MEblock> LCTs = spPtr->record(tbin).LCTs();
			for(vector<CSCSP_MEblock>::const_iterator lct=LCTs.begin(); lct!=LCTs.end(); lct++){
				//unsigned short mpc = lct->mpc()+1;        // 1-60
				unsigned short mpc =(lct->spInput()-1)/3+1; // 1-5
				unsigned short csc = lct->csc();            // 1-9

				if(!tf.isBooked(sp,mpc) ){
					tf.book(sp,mpc);
					std::cout<<"Booking histograms for SP:"<<sp<<" MPC: "<<mpc<<std::endl;
				}

				TH1F *csc_ = (TH1F*)tf.get("csc",sp,mpc);
				if( csc_ ) csc_->Fill(lct->csc());

				if( nevents%100==0 ){
					TH2F *epc = (TH2F*)tf.get("epc",sp,mpc);
					if( epc ) epc->SetBinContent(1,csc,lct->errCnt());
				}
				TH2F *inputs = (TH2F*)tf.get("inputs",sp,mpc);
				if( inputs ) inputs->Fill((lct->spInput()-1)%3+(mpc-1)*3+1,lct->link()+(mpc-1)*3);

				TH2F *mpc_id = (TH2F*)tf.get("mpc_id",sp,mpc);
				if( mpc_id ) mpc_id->Fill(lct->mpc(),mpc);

				if( csc==0 || csc>9 ) continue;

				if(!tf.isBooked(sp,mpc,csc) ){
					tf.book(sp,mpc,csc);
					std::cout<<"Booking histograms for SP:"<<sp<<" MPC: "<<mpc<<" CSC: "<<csc<<std::endl;
				}

				TH1F *strips = (TH1F*)tf.get("strips",sp,mpc,csc);
				if( strips ) strips->Fill(lct->strip());

				TH2F *stripsVSpatterns = (TH2F*)tf.get("stripsVSpatterns",sp,mpc,csc);
				if( stripsVSpatterns ) stripsVSpatterns->Fill(lct->strip(),lct->pattern()|(lct->l_r()<<4));

				TH1F *WireGroup = (TH1F*)tf.get("WireGroup",sp,mpc,csc);
				if( lct->wireGroup() && WireGroup ) WireGroup->Fill(lct->wireGroup());

				TH1F *clct = (TH1F*)tf.get("CLCT",sp,mpc,csc);
				if( clct ) clct->Fill(lct->pattern());

				TH1F *lctQuality = (TH1F*)tf.get("LCTquality",sp,mpc,csc);
				if( lctQuality ) lctQuality->Fill(lct->quality());

				TH2F *synch = (TH2F*)tf.get("synch",sp,mpc,csc);
				if( synch ) synch->Fill(spPtr->header().BXN()%(int)synch->GetXaxis()->GetXmax(),lct->BXN()%(int)synch->GetYaxis()->GetXmax());

				TH1F *sp_bits = (TH1F*)tf.get("sp_bits",sp,mpc,csc);
				if( sp_bits ){
					sp_bits->Fill(lct->receiver_status_frame1());
					sp_bits->Fill(lct->receiver_status_frame2()+4);
					if( lct->aligment_fifo() )
						sp_bits->Fill(lct->aligment_fifo()+7);
				}

				TH1F *lct_bits = (TH1F*)tf.get("lct_bits",sp,mpc,csc);
				if( lct_bits ){
					double norm = lct_bits->GetEntries();
					if( lct->bc0() ) lct_bits->Fill(0);
					if( lct->bx0() ) lct_bits->Fill(1);
					if( lct->l_r() ) lct_bits->Fill(2);
					lct_bits->SetEntries(++norm);
				}

				TH1F *wg0quality = (TH1F*)tf.get("wg0quality",sp,mpc,csc);
				if( wg0quality && lct->wireGroup()==0 ) wg0quality->Fill(lct->quality());

				TH1F *strip0quality = (TH1F*)tf.get("strip0quality",sp,mpc,csc);
				if( strip0quality && lct->strip()==0 ) strip0quality->Fill(lct->quality());

				TH1F *time_bin = (TH1F*)tf.get("time_bin",sp,mpc,csc);
				if( time_bin ) time_bin->Fill(lct->tbin());

				// Store hits for later timing correlation analysis:
				shared_hits.fill(tbin,mpc,csc,*lct);
			}

			vector<CSCSP_MBblock> dt_stubs = spPtr->record(tbin).mbStubs();
			for(vector<CSCSP_MBblock>::const_iterator dt_stub=dt_stubs.begin(); dt_stub!=dt_stubs.end(); dt_stub++){
                                TH1F *dt_bits = (TH1F*)tf.get("dt_bits",sp);
                                if( dt_bits ){
					dt_bits->Fill(dt_stub->flag()>>0);
					dt_bits->Fill(dt_stub->cal()>>1);
					dt_bits->Fill(dt_stub->bc0()>>2);
					dt_bits->Fill(dt_stub->BXN()>>3);
				}

				TH1F *dt_quality = (TH1F*)tf.get("dt_quality",sp);
				if( dt_quality ) dt_quality->Fill(dt_stub->quality());

				TH1F *dt_phi_bend = (TH1F*)tf.get("dt_phi_bend",sp);
				if( dt_phi_bend ) dt_phi_bend->Fill(dt_stub->phi_bend());

				TH1F *dt_phi = (TH1F*)tf.get("dt_phi",sp);
                                if( dt_phi ) dt_phi->Fill(dt_stub->phi());

				TH2F *dt_synch = (TH2F*)tf.get("dt_synch",sp);
				if( dt_synch ) dt_synch->Fill(spPtr->header().BXN()%(int)dt_synch->GetXaxis()->GetXmax(),dt_stub->BXN()%(int)dt_synch->GetYaxis()->GetXmax());
			}


			vector<CSCSP_SPblock> tracks = spPtr->record(tbin).tracks();

			TH1F *nTracks = (TH1F*)tf.get("nTracks",sp);
			if( nTracks ) nTracks->Fill(tracks.size());

			for(vector<CSCSP_SPblock>::const_iterator track=tracks.begin(); track!=tracks.end(); track++){

				TH2F *mode_vs_station = (TH2F*)tf.get("mode_vs_station",sp);
				if( mode_vs_station ){
					unsigned int stations =0;
					if( track->ME1_id() ) stations|=1;
					if( track->ME2_id() ) stations|=2;
					if( track->ME3_id() ) stations|=4;
					if( track->ME4_id() ) stations|=8;
					mode_vs_station->Fill(track->mode(),stations);
				}

				TH2F *stations_vs_lcts = (TH2F*)tf.get("stations_vs_lcts",sp);
				if( stations_vs_lcts ){
					unsigned int nStations=0;
					if( track->ME1_id() ) nStations++;
					if( track->ME2_id() ) nStations++;
					if( track->ME3_id() ) nStations++;
					if( track->ME4_id() ) nStations++;
					stations_vs_lcts->Fill(track->LCTs().size(),nStations);
				}

				TH1F *tbins = (TH1F*)tf.get("tbins",sp);
				if( tbins ){
					tbins->Fill(track->ME1_tbin()+0);
					tbins->Fill(track->ME2_tbin()+7);
					tbins->Fill(track->ME3_tbin()+14);
					tbins->Fill(track->ME4_tbin()+21);
					tbins->Fill(track->MB_tbin ()+28);
				}

				TH1F *track_time = (TH1F*)tf.get("track_time",sp);
				if( track_time ) track_time->Fill(track->tbin());

			}
		}

		// Timing correlation analysis
		///if( shared_hits.nHits()<=300 ){ // showering prevention
			map<pair<unsigned int,unsigned int>,int> deltaBX = shared_hits.diffBX();
			map<pair<unsigned int,unsigned int>,int>::const_iterator iter = deltaBX.begin();
			while(iter!=deltaBX.end()){
				unsigned short mpc1 = iter->first.first/10;
				unsigned short csc1 = iter->first.first%10;
				if( !tf.isBooked(sp,mpc1,csc1) ) tf.book(sp,mpc1,csc1);
				// following mapping is strictly defined by Y labels of the timing histogram
				const int id2bin[50] = {
					-1,-1, -1, -1, -1, -1, -1, -1, -1, -1,
					-1, 1,  2,  3,  4,  5,  6,  7,  8,  9,
					-1,10, 11, 12, 13, 14, 15, 16, 17, 18,
					-1,19, 20, 21, 22, 23, 24, 25, 26, 27,
					-1,28, 29, 30, 31, 32, 33, 34, 35, 36
				};
				TH2F *csc_timing = (TH2F*)tf.get("csc_timing",sp,mpc1,csc1);
				if( csc_timing && id2bin[iter->first.second]>0 ) csc_timing->Fill(iter->second,id2bin[iter->first.second]);
				unsigned short mpc2 = iter->first.second/10;
				unsigned short csc2 = iter->first.second%10;
				if( !tf.isBooked(sp,mpc2,csc2) ) tf.book(sp,mpc2,csc2);
				csc_timing = (TH2F*)tf.get("csc_timing",sp,mpc2,csc2);
				if( csc_timing && id2bin[iter->first.first]>0 ) csc_timing->Fill(-iter->second,id2bin[iter->first.first]);
				iter++;
			}
		///}
	}

	if(nevents%1000==0) cout<<"Event: "<<nevents<<endl;
	nevents++;
}
