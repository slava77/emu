#include "emu/step/TestParameters.h"

#include "emu/utils/IO.h"
#include "emu/utils/DOM.h"
#include "emu/utils/String.h"
#include "emu/utils/System.h"

#include "xcept/tools.h"

using namespace emu::utils;

emu::step::TestParameters::TestParameters( const string& id, 
					   const string& testParametersXML,
					   Logger*       pLogger )
  : bsem_( toolbox::BSem::EMPTY ) // locked
  , id_( id )
  , nEvents_( 0 )
  , pLogger_( pLogger ){
  try{
    extractParameters( testParametersXML );
    bsem_.give();
  }
  catch ( xcept::Exception &e ){
    stringstream ss;
    ss << "Failed to create emu::step::TestParameters for test " << id_ << ".";
    bsem_.give();
    XCEPT_RETHROW( xcept::Exception, ss.str(), e );
  }
}

void emu::step::TestParameters::extractParameters( const string& testParametersXML ){

  stringstream xpath;
  xpath << "//STEP_tests/test_config[translate(test,' ','')='" << id_ << "']/*[name()!='test']";
  cout << "testParametersXML" << endl << testParametersXML << endl << "xpath" << endl << xpath.str() << endl << flush;
  vector< pair< string, string > > par = utils::getSelectedNodesValues( testParametersXML, xpath.str() );

  for ( vector< pair< string, string > >::const_iterator p = par.begin(); p != par.end(); ++p ){
    parameters_[p->first] = utils::stringTo<int>( p->second );
  }

  calculateNEvents();
}

// void emu::step::TestParameters::calculateNEvents(){
//   if      ( id_ == "11" ){
//     nEvents_ = parameters_["events_total"];
//   }
//   else if ( id_ == "12" ){
//     const uint64_t nStrips = 6; // strips to scan, never changes
//     nEvents_ = parameters_["events_per_strip"] * nStrips;
//   }
//   else if ( id_ == "13" ){
//     nEvents_ = parameters_["events_per_threshold"] * parameters_["thresholds_per_tpamp"] * parameters_["tpamps_per_run"];
//   }
//   else if ( id_ == "14" ){
//     nEvents_ = parameters_["events_per_delay"] * parameters_["delays_per_run"];
//   }
//   else if ( id_ == "15" ){
//     nEvents_ = parameters_["events_total"];
//   }
//   else if ( id_ == "16" ){
//     const uint64_t nLayerPairs = 3; // Pairs of layers to scan, never changes. (Scans 2 layers at a time.)
//     nEvents_ = parameters_["events_per_layer"] * nLayerPairs;
//   }
//   else if ( id_ == "17" ){
//     nEvents_ = parameters_["events_per_delay"] * parameters_["delays_per_strip"] * parameters_["strips_per_run"];
//   }
//   else if ( id_ == "17b"){
//     nEvents_ = parameters_["events_per_pulsedac"] * parameters_["pulse_dac_settings"] * parameters_["strips_per_run"];
//   }
//   else if ( id_ == "18" ){
//     nEvents_ = parameters_["events_total"];
//   }
//   else if ( id_ == "19" ){
//     nEvents_ = parameters_["events_per_thresh"] * parameters_["threshs_per_tpamp"] * parameters_["dmb_tpamps_per_strip"] * parameters_["strips_per_run"];
//   }
//   else if ( id_ == "21" ){
//     nEvents_ = parameters_["events_per_hstrip"] * parameters_["hstrips_per_run"];
//   }
//   else if ( id_ == "25" ){
//     nEvents_ = parameters_["trig_settings"] * parameters_["events_per_trig_set"];
//   }
//   else {
//     XCEPT_RAISE( xcept::Exception, "Unknown test id '" + id_ + "'." );
//   }
// }

void emu::step::TestParameters::calculateNEvents(){
  // For passive tests (i.e., those without pulses), get the number of events to be collected.
  if      ( id_ == "18" ){
    nEvents_ = parameters_["events_total"];
  }
  else if ( id_ == "27" ){
    nEvents_ = parameters_["events_total"];
  }
  // For active (i.e., pulsed) tests, the number of events to be collected will be calculated at the beginning of the test
  // as it depends on the number of chambers to be tested. For the time being, keep nEvents_ = 0 for active (pulsed or timed) tests.
}
