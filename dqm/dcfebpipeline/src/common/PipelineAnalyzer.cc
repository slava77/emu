#include "emu/dqm/dcfebpipeline/PipelineAnalyzer.h"

#include "emu/utils/String.h"
#include "emu/utils/System.h"

#include "EventFilter/CSCRawToDigi/interface/CSCDDUEventData.h"
#include "EventFilter/CSCRawToDigi/interface/CSCCFEBData.h"
#include "DataFormats/CSCDigi/interface/CSCStripDigi.h"
#include "DataFormats/CSCDigi/interface/CSCComparatorDigi.h"
#include "DataFormats/MuonDetId/interface/CSCDetId.h"

#include "pstream.h" // for redi::ipstream

#include <iomanip>
#include <cmath> // for isnan

/// See EventFilter/CSCRawToDigi/src/CSCDCCExaminer.cc for ExaminerMaskType bits
const ExaminerMaskType PipelineAnalyzer::examinerMask_ = binaryTo<ExaminerMaskType>( "0111 1111 0000 0110 0000" );

PipelineAnalyzer::PipelineAnalyzer( const string& dataDir )
  : dataDir_( dataDir ){
  openResults();
  analyze();
  closeResults();
}

void PipelineAnalyzer::analyze(){

  // Expects data files in the following directory structure:
  // dataDir_/CC/SSS/PPP/RRRR.raw
  // CC:   crate number
  // SS:   ODMB slot
  // PPP:  set pipeline depth
  // RRRR: readout pass

  // Loop over crate directories
  vector<string> crateDirs( execShellCommand( "ls -1d " + dataDir_ + "/??" ) );
  for ( vector<string>::const_iterator c = crateDirs.begin(); c!=crateDirs.end(); ++c ){
    unsigned int crate = emu::utils::stringTo<unsigned int>( c->substr( c->find_last_of( "/" ) + 1 ) );

    // Loop over (O)DMB directories
    vector<string> dmbDirs( execShellCommand( "ls -1d " + *c + "/??" ) );
    for ( vector<string>::const_iterator d = dmbDirs.begin(); d!=dmbDirs.end(); ++d ){
      unsigned int dmbSlot = emu::utils::stringTo<unsigned int>( d->substr( d->find_last_of( "/" ) + 1 ) );

      // Loop over the pipeline depths directories and collect all the files for this DMB.
      multimap<unsigned int, string> allFileNames; // pipeline depth --> data files for this DMB
      vector<string> depthDirs( execShellCommand( "ls -1d " + *d + "/???" ) );
      for ( vector<string>::const_iterator pd = depthDirs.begin(); pd!=depthDirs.end(); ++pd ){
	unsigned int depth = emu::utils::stringTo<unsigned int>( pd->substr( pd->find_last_of( "/" ) + 1 ) );
	// Loop over files at this pipeline depth
	vector<string> fileNames( execShellCommand( "ls -1 " + *pd + "/*.raw" ) );
	for ( vector<string>::const_iterator f = fileNames.begin(); f!=fileNames.end(); ++f ){
	  allFileNames.insert( pair<unsigned int, string>( depth, *f ) );
	}
      }

      // Open a log file for debugging
      ostringstream logFileName;
      logFileName << *d << "/" 
		  << "crate" << setw(2) << setfill('0') << fixed << crate
		  << "_slot" << setw(2) << setfill('0') << fixed << dmbSlot
		  << ".log";
      logFile_.open( logFileName.str().c_str(), ofstream::trunc );

      // Get samples from this DMB
      map<DCFEBSampleId,DCFEBSample> samples( getADCCounts( allFileNames ) );

      // Loop over CFEBs
      for ( unsigned int cfeb=0; cfeb<maxCFEBs_; ++cfeb ){

	PipelineDepthFinder p;
	p.setId( crate, dmbSlot, cfeb ).setLogDir( *d );
	    
	// Loop over the samples. These may contain more than one CFEB.
	for ( std::map<DCFEBSampleId,DCFEBSample>::const_iterator s=samples.begin(); s!=samples.end(); ++s ){

	  // Add the samples from this CFEB only.
	  if ( s->first.cfeb == cfeb ) p.addDepth( s->first.pipelineDepth, s->second.getNSamples(), s->second.getAverageSample() );

	}
	
	double bestDepth = p.getBestDepth();

	if ( isnan( bestDepth ) || bestDepth <= 0.) addToResults( crate, dmbSlot, cfeb,       -1. );
	else                                        addToResults( crate, dmbSlot, cfeb, bestDepth );

	logFile_ << "\nFitting all measurements at different pipeline depth settings:"
		 << "   crate: "      << crate
		 << "   slot: "       << dmbSlot
		 << "   cfeb: "       << cfeb
		 << "   best depth: " << bestDepth
		 << "\n";

	logFile_.close();

      }
    }
  }
}

map<DCFEBSampleId,DCFEBSample> 
PipelineAnalyzer::getADCCounts( const multimap<unsigned int, string>& fileNames ){

  map<DCFEBSampleId,DCFEBSample> samples;

  ostringstream summary;
  summary << "\n\nCFEB depth   ACD counts                              sum of ADC counts\n";

  for ( multimap<unsigned int, string>::const_iterator f=fileNames.begin(); f!=fileNames.end(); ++f ){

    unsigned int pipelineDepth = f->first;

    logFile_ << "======================================================================\n" << f->second << "\n";

    ifstream inputFile( f->second.c_str(), ifstream::in | ifstream::binary );
    // get pointer to associated buffer object
    filebuf* pbuf = inputFile.rdbuf();
    // get file size using buffer's members
    size_t size = pbuf->pubseekoff ( 0, inputFile.end, inputFile.in );
    pbuf->pubseekpos ( 0, inputFile.in );
    // allocate memory to contain file data
    char* buf = new char[size];
    // get file data
    pbuf->sgetn( buf, size );
    
    vector<unsigned short*> headers;
    vector<unsigned short*> trailers;
    unsigned short* shorts = (unsigned short*) buf;
    for ( size_t i=0; i<size/sizeof(unsigned short); i+=4 ){
      if ( ( ( shorts[i+0] & 0xf000 ) == 0x9000 ) && 
	   ( ( shorts[i+1] & 0xf000 ) == 0x9000 ) && 
	   ( ( shorts[i+2] & 0xf000 ) == 0x9000 ) && 
	   ( ( shorts[i+3] & 0xf000 ) == 0x9000 ) && 
	   ( ( shorts[i+4] & 0xf000 ) == 0xa000 ) && 
	   ( ( shorts[i+5] & 0xf000 ) == 0xa000 ) && 
	   ( ( shorts[i+6] & 0xf000 ) == 0xa000 ) && 
	   ( ( shorts[i+7] & 0xf000 ) == 0xa000 )    ){
	headers.push_back( shorts + i );
	// 	logFile_ << hex 
	// 		 << *(shorts+i+0) << *(shorts+i+1) << *(shorts+i+2) << *(shorts+i+3) 
	// 		 << *(shorts+i+4) << *(shorts+i+5) << *(shorts+i+6) << *(shorts+i+7) 
	// 		 << dec
	// 		 << "  at " << (void*)( shorts+i )
	// 		 << "\n";
      }
      else if ( ( ( shorts[i+0] & 0xf000 ) == 0xf000 ) && 
		( ( shorts[i+1] & 0xf000 ) == 0xf000 ) && 
		( ( shorts[i+2] & 0xf000 ) == 0xf000 ) && 
		( ( shorts[i+3] & 0xf000 ) == 0xf000 ) && 
		( ( shorts[i+4] & 0xf000 ) == 0xe000 ) && 
		( ( shorts[i+5] & 0xf000 ) == 0xe000 ) && 
		( ( shorts[i+6] & 0xf000 ) == 0xe000 ) && 
		( ( shorts[i+7] & 0xf000 ) == 0xe000 )    ){
	trailers.push_back( shorts + i );
	// 	logFile_ << hex 
	// 		 << *(shorts+i+0) << *(shorts+i+1) << *(shorts+i+2) << *(shorts+i+3) 
	// 		 << *(shorts+i+4) << *(shorts+i+5) << *(shorts+i+6) << *(shorts+i+7) 
	// 		 << dec
	// 		 << "  at " << (void*)( shorts+i )
	// 		 << "\n";
      }
    }
    logFile_ << headers .size() << " headers : " << headers  << "\n";
    logFile_ << trailers.size() << " trailers: " << trailers << "\n";
    
    const uint16_t format_version = 2013; // just re-map the ME11 strips differently
    
    vector<unsigned short*>::const_iterator h=headers .begin();
    vector<unsigned short*>::const_iterator t=trailers.begin();
    while( h!=headers.end() && t!=trailers.end() ){
      
      // Check DMB header/trailer consistency
      if ( *h < *t && ( h+1 == headers.end() || *t < *(h+1) ) ){
	
	logFile_ << "------------------------------------------------ Event index " << h-headers.begin() << "\n";
	
	// Use the examiner to spot and skip corrupted events
	CSCDCCExaminer examiner( examinerMask_ );
	examiner.output1().hide(); // suppress output
	examiner.output2().hide(); // suppress output
	const int32_t trailerSize( 8 );
	const uint16_t *data  = reinterpret_cast<const uint16_t *>( *h ); // CSCDCCExaminer::check will change this! Do not use afterwards.
	examiner.check( data, (int32_t)( *t - *h + trailerSize ) ); // This changes the data pointer!!!
	if ( ( examiner.errors() & examinerMask_ ) > 0 ){
	  // Skip this event, but log what was wrong with it:
	  logFile_ << "Examiner mask : " << binaryFrom<ExaminerMaskType>( examiner.getMask() )  << "\n";
	  logFile_ << "Examiner found: " << binaryFrom<ExaminerStatusType>( examiner.errors() ) << "\n";
	  logFile_ << "Header  at " << (void*)(*h) << "\n";
	  logFile_ << "Trailer at " << (void*)(*t) << "\n";
	  logFile_ << "Event size: " << (int32_t)( *t - *h + trailerSize ) << " short words\n";
	}
	else{
	  CSCEventData event( *h, format_version );

	  // Sometimes it happens that nclct in nonzero, yet tmb data is not there... Check both:
	  if ( event.nclct() > 0 && event.tmbData() != NULL ){
	    for ( int iCFEB=0; iCFEB<7; ++iCFEB ){
	      CSCCFEBData *cfebData = event.cfebData( iCFEB );
	      for ( int iLayer=1; iLayer<=6; iLayer++ ){
		vector<CSCStripDigi> stripDigis;
		if ( cfebData ){
		  CSCDetId layer(1, 1, 1, 1, iLayer);
		  cfebData->digis( layer.rawId(), stripDigis );
		}
		// try{
		vector<CSCComparatorDigi> compDigis( event.clctData()->comparatorDigis( iLayer, iCFEB ) );
		for ( vector<CSCComparatorDigi>::iterator cd=compDigis.begin(); cd!=compDigis.end(); ++cd ){
		  logFile_ << "cfeb "   << iCFEB 
			   << " layer " << iLayer 
			   << " strip " << setw(3) << cd->getStrip() 
			   << " comp "  << cd->getComparator()
			   << " times " << binaryFrom<uint16_t>( cd->getTimeBinWord() )
			   << "\n";
		  for ( vector<CSCStripDigi>::iterator sd=stripDigis.begin(); sd!=stripDigis.end(); ++sd ){
		    if ( sd->getStrip() == cd->getStrip() ){
		      // sd->print();
		      logFile_ << "   ADC counts for strip " << *sd << "\n";
		      vector<int> adcCounts( sd->getADCCounts() );
		      if ( adcCounts.size() > 0 ) {
			summary << setw(4) << iCFEB << " " << pipelineDepth << "   ";
			DCFEBSampleId id( iCFEB, pipelineDepth );
			logFile_ << "   id(" << iCFEB << "," << pipelineDepth << ") ";
			map<DCFEBSampleId,DCFEBSample>::iterator s = samples.find( id );
			if ( s != samples.end() ){
			  s->second.add( adcCounts );
			  logFile_ << "added new id(" << s->first.cfeb << "," << s->first.pipelineDepth 
				   << ") nSamples=" << s->second.getNSamples() << "\n"; 
			}
			else{
			  samples[id] = DCFEBSample( adcCounts );
			  logFile_ << "added new sample to id(}" << id.cfeb << "," << id.pipelineDepth 
				   << ") nSamples=" << samples[id].getNSamples() << "\n";
			}
			int minimum = *std::min_element( adcCounts.begin(), adcCounts.end() );
			for ( vector<int>::const_iterator c=adcCounts.begin(); c!=adcCounts.end(); ++c ){
			  summary << setw(4) << *c - minimum << " ";
			}
			summary << "       " << std::accumulate( adcCounts.begin(), adcCounts.end(), 0 ) - adcCounts.size() * minimum << "\n";
		      }
		    }
		  }
		}
		// }
		// catch( cms::Exception& e ){
		// 	logFile_ << "Caught exception '" << e.what() << "'. Continuing.";
		// 	continue;
		// }
	      }
	    }
	  }
	}
      }
      ++h;
      ++t;
    }
    
    inputFile.close();
    delete[] buf;
    
  }

  logFile_ << summary.str();

  return samples;
}

vector<string> PipelineAnalyzer::execShellCommand( const string &shellCommand ) const {
  vector<string> replyLines;
  redi::ipstream command( shellCommand.c_str() );
  string reply;
  while ( getline( command, reply ) ) replyLines.push_back(reply);
  return replyLines;
}

void PipelineAnalyzer::openResults(){
  resultsTextFile_.open( ( dataDir_ + "/results.txt" ).c_str(), ofstream::trunc );
  resultsTextFile_ << "Results from " << dataDir_ << "\n"
		   << "Crate     DMB slot   CFEB  Pipeline depth  pipeline_depth  fine_delay\n";

  resultsHTMLFile_.open( ( dataDir_ + "/results.html" ).c_str(), ofstream::trunc );
  resultsHTMLFile_ << "<input type=\"button\" value=\"dismiss\" title=\"Dismiss table of results. (Reload page to redisplay it.)\"  id=\"removePipelineDepths\" onclick=\"var t=document.getElementById('pipelineDepths'); if ( t ) t.parentNode.removeChild( t ); var i=document.getElementById('removePipelineDepths'); if ( i ) i.parentNode.removeChild( i );\"/>\n"
		   << "<table class=\"pipelineDepths\" id=\"pipelineDepths\">\n"
		   << "  <caption>Results from " << dataDir_ << "</caption>\n"
		   << "  <tr><th>Crate</th><th>DMB slot</th><th>CFEB</th><th>Pipeline depth</th><th style=\"font-style:italic\">pipeline_depth</th><th style=\"font-style:italic\">fine_delay</th></tr>\n";

  resultsXSLTFile_.open( ( dataDir_ + "/results.xsl" ).c_str(), ofstream::trunc );
  resultsXSLTFile_ << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
		   << "<!-- Results from " << dataDir_ << " -->\n"
		   << "\n"
		   << "<xsl:transform xmlns:xsl=\"http://www.w3.org/1999/XSL/Transform\" xmlns:es=\"http://cms.cern.ch/emu/step\" version=\"1.0\">\n"
		   << "\n"
		   << "  <xsl:output method=\"xml\" indent=\"yes\"/>\n"
		   << "\n"
		   << "  <!-- Change this to 'yes' to set CFEB fine_delay, too: -->\n"
		   << "  <xsl:param name=\"SET_FINE_DELAY\">no</xsl:param>\n"
		   << "\n"
		   << "  <xsl:template match=\"*|@*|comment()\">\n"
		   << "    <!-- Copy everything but text fields as all values are in attributes in our XML. -->\n"
		   << "    <xsl:copy >\n"
		   << "      <xsl:apply-templates select=\"*|@*|comment()\"/>\n"
		   << "    </xsl:copy>\n"
		   << "  </xsl:template>\n"
		   << "\n"
		   << "  <!-- =============================== -->\n";
}

void PipelineAnalyzer::addToResults( const unsigned int crate, const unsigned int slot, const unsigned int cfeb, const double depth ){
  // The naive fine delay would be:
  int pipeline_depth( depth >= 0 ? int(   depth                         ) : -1 );
  int fine_delay    ( depth >= 0 ? int( ( depth - pipeline_depth ) * 8. ) : -1 );
  // But it is interpreted differently:
  // value   meaning
  //     0         0
  //     1         1
  //     2         2
  //     3         3
  //     4        -4
  //     5        -3
  //     6        -2
  //     7        -1
  // The next pipeline depth needs therefore to be taken for fine delay values above 3 (meaning negative fine delay actually).
  if ( fine_delay >= 4 ) pipeline_depth = pipeline_depth + 1;

  resultsTextFile_ << ""               << setw(2) << setfill(' ') << crate
		   << "        "       << setw(2) << setfill(' ') << slot
		   << "         "      << setw(1) << setfill(' ') << cfeb
		   << "    "           << setw(6) << setfill(' ') << setprecision(2) << fixed << depth
		   << "          "     << setw(3) << setfill(' ') << pipeline_depth
		   << "              " << setw(1) << setfill(' ') << fine_delay << "\n";

  resultsHTMLFile_ << "  <tr style=\"text-align:right\">"
		   << "<td>" << crate << "</td>"
		   << "<td>" << slot  << "</td>"
		   << "<td>" << cfeb  << "</td>"
		   << "<td>" << setprecision(2) << fixed << depth << "</td>"
		   << "<td>" << pipeline_depth << "</td>"
		   << "<td>" << fine_delay << "</td>"
		   << "</tr>\n";

  resultsXSLTFile_ << ( depth < 0 ? "  <!-- " : "  " )
		   << "<xsl:template match=\"//PeripheralCrate[@crateID='"           << crate
		   << "']/CSC/DAQMB[@slot='"                                         << slot 
		   << "']/CFEB[@cfeb_number='"                                       << cfeb 
		   << "']/@pipeline_depth\"><xsl:attribute name=\"pipeline_depth\">" << pipeline_depth
		   << "</xsl:attribute></xsl:template>"
		   << ( depth < 0 ? " -->\n  <!-- " : "\n  " )
		   << "<xsl:template match=\"//PeripheralCrate[@crateID='"           << crate
		   << "']/CSC/DAQMB[@slot='"                                         << slot 
		   << "']/CFEB[@cfeb_number='"                                       << cfeb 
		   << "']/@fine_delay\"><xsl:if test=\"$SET_FINE_DELAY='yes'\">"
		   << "<xsl:attribute name=\"fine_delay\">"                          << fine_delay
		   << "</xsl:attribute></xsl:if></xsl:template>"
		   << ( depth < 0 ? " -->\n" : "\n" );
}

void PipelineAnalyzer::closeResults(){
  resultsTextFile_.close();

  resultsHTMLFile_ << "</table>\n";
  resultsHTMLFile_.close();

  resultsXSLTFile_ << "</xsl:transform>\n";
  resultsXSLTFile_.close();
}
