#include "emu/step/Configuration.h"

#include "emu/utils/IO.h"
#include "emu/utils/DOM.h"
#include "emu/utils/Xalan.h"
#include "emu/utils/String.h"
#include "emu/utils/System.h"

#include "xcept/Exception.h"

using namespace emu::utils;

emu::step::Configuration::Configuration( const string& XMLnamespace, 
					 xdata::String& configurationXSLFileName,
					 xdata::String& testParametersFileName,
					 const map<string,string>& pCrateSettingsFileNames )
  : bsem_( toolbox::BSem::FULL ) // unlocked
  , namespace_( XMLnamespace )
{
  bsem_.take();

  xslt_ = emu::utils::readFile( emu::utils::performExpansions( configurationXSLFileName.toString() ) );
  if ( xslt_.size() == 0 ){
    XCEPT_RAISE( xcept::Exception, configurationXSLFileName.toString() + " could not be read in or is empty." );
  }

  testParametersXML_ = emu::utils::readFile( emu::utils::performExpansions( testParametersFileName ) );
  if ( testParametersXML_.size() == 0 ){
    XCEPT_RAISE( xcept::Exception, testParametersFileName.toString() + " could not be read in or is empty." );
  }
  for ( map<string,string>::const_iterator p = pCrateSettingsFileNames.begin(); p != pCrateSettingsFileNames.end(); ++p )
    {
      if ( p->first.size() > 0 && p->second.size() > 0 )
	{
	  pCrateSettingsXMLs_[p->first] = emu::utils::readFile( emu::utils::performExpansions( p->second ) );
	  if ( pCrateSettingsXMLs_[p->first].size() == 0 ){
	    XCEPT_RAISE( xcept::Exception, p->second + " could not be read in or is empty." );
	  }
	}
    }
  bsem_.give();

  createXML();
}

void emu::step::Configuration::createXML()
{
  stringstream source;
  stringstream xsl;
  stringstream crates;
  stringstream tests;

  bsem_.take();

  modificationTime_ = emu::utils::getDateTime();

  xml_ = "<es:configuration xmlns:es=\"" + namespace_ + "\" modificationTime=\"" + modificationTime_ + "\"/>";

  source.str( testParametersXML_ );
  xsl.str( xslt_ );
  emu::utils::transformStreams( source, xsl, tests );
  xml_ = emu::utils::appendToSelectedNode( xml_, "/es:configuration", tests.str() );

  for ( map<string,string>::const_iterator p = pCrateSettingsXMLs_.begin(); p != pCrateSettingsXMLs_.end(); ++p )
    {
      map<string,string> param;
      param["GROUP"] = "'" + p->first + "'";
      source.clear(); // reset control state to good after previous operation
      source.str( p->second ); // refill buffer
      xsl.clear(); // reset control state to good after previous operation
      xsl.str( xslt_ ); // refill buffer
      crates.clear(); // reset control state to good after previous operation
      crates.str(""); // reset buffer
      emu::utils::transformWithParams( source, xsl, crates, param );
      xml_ = emu::utils::appendToSelectedNode( xml_, "/es:configuration", crates.str() );
    }

  bsem_.give();
}


xdata::Vector<xdata::String> emu::step::Configuration::getCrateIds( const string& group ) const
{
  bsem_.take();
  vector< pair<string,string> > ids = emu::utils::getSelectedNodesValues( xml_, "//es:peripheralCrate[@selected='yes' and ../@group='" + group + "' ]/@id" );
  bsem_.give();  

  // cout << "Selected crate ids for group " << group << ":     "<< ids << endl;

  xdata::Vector<xdata::String> crateIds;
  for ( vector< pair<string,string> >::const_iterator i = ids.begin(); i != ids.end(); ++i ) crateIds.push_back( i->second );

  return crateIds;
}

xdata::Vector<xdata::String> emu::step::Configuration::getChamberLabels( const string& group ) const
{
  bsem_.take();
  vector< pair<string,string> > labels = emu::utils::getSelectedNodesValues( xml_, "//es:peripheralCrate[../@group='" + group + "' ]/es:chamber[@selected='yes']/@label" );
  bsem_.give();  

  // cout << "Selected chambers for group " << group << ":     "<< labels << endl;

  xdata::Vector<xdata::String> chamberLabels;
  for ( vector< pair<string,string> >::const_iterator l = labels.begin(); l != labels.end(); ++l ) chamberLabels.push_back( l->second );

  return chamberLabels;
}

xdata::Vector<xdata::String> emu::step::Configuration::getChamberLabels() const
{
  bsem_.take();
  vector< pair<string,string> > labels = emu::utils::getSelectedNodesValues( xml_, "//es:peripheralCrate/es:chamber[@selected='yes']/@label" );
  bsem_.give();  

  // cout << "Selected chambers:     " << labels << endl;

  xdata::Vector<xdata::String> chamberLabels;
  for ( vector< pair<string,string> >::const_iterator l = labels.begin(); l != labels.end(); ++l ) chamberLabels.push_back( l->second );

  return chamberLabels;
}


xdata::Vector<xdata::String> emu::step::Configuration::getTestIds() const
{
  bsem_.take();
  vector< pair<string,string> > ids = emu::utils::getSelectedNodesValues( xml_, "//es:testSequence/es:test[@selected='yes']/@id" );
  bsem_.give();

  xdata::Vector<xdata::String> testIds;
  for ( vector< pair<string,string> >::const_iterator i = ids.begin(); i != ids.end(); ++i ) testIds.push_back( i->second );

  return testIds;
}


void emu::step::Configuration::setSelection( const multimap<string,string>& selection ){
  bsem_.take();
  // First deselect all tests and crates:
  xml_ = emu::utils::setSelectedNodesValues( xml_, "//@selected", "no" );
  // Transfer values to a simple map:
  map<string,string> s;
  for ( multimap<string,string>::const_iterator is = selection.begin(); is != selection.end(); ++ is ) s[is->first] = is->second;
  // Change unique id, too, as the configuration has been changed:
  modificationTime_ = emu::utils::getDateTime();
  s["/es:configuration/@modificationTime"] = modificationTime_;
  // Change values in XML:
  xml_ = emu::utils::setSelectedNodesValues( xml_, s );
  bsem_.give();
}

void emu::step::Configuration::setTestStatus( const string& testId, const string& status, const string& details ){
  bsem_.take();
  xml_ = emu::utils::setSelectedNodesValues( xml_, "//es:testSequence/es:test[@id='" + testId + "']/@status" , status  );
  xml_ = emu::utils::setSelectedNodesValues( xml_, "//es:testSequence/es:test[@id='" + testId + "']/@details", details );
  bsem_.give();
}

void emu::step::Configuration::setTestStatus( const string& status, const string& details ){
  bsem_.take();
  xml_ = emu::utils::setSelectedNodesValues( xml_, "//es:testSequence/es:test/@status" , status  );
  xml_ = emu::utils::setSelectedNodesValues( xml_, "//es:testSequence/es:test/@details", details );
  bsem_.give();
}

void emu::step::Configuration::setTestProgress( const map<string,pair<double,string> >& groupsProgress ){
  map<string,string> progressValues;
  for ( map<string,pair<double,string> >::const_iterator p = groupsProgress.begin(); p != groupsProgress.end(); ++p ){
    progressValues["//es:peripheralCrates[@group='" + p->first + "']/@progress" ] = emu::utils::stringFrom<int>( (int)p->second.first );
    bsem_.take();
    xml_ = emu::utils::removeSelectedNode( xml_, "//es:peripheralCrates[@group='" + p->first + "']/es:message" );
    xml_ = emu::utils::appendToSelectedNode( xml_, "//es:peripheralCrates[@group='" + p->first + "']", 
					     "<es:message>" + p->second.second + "</es:message>" );
    bsem_.give();
  }
  bsem_.take();
  xml_ = emu::utils::setSelectedNodesValues( xml_, progressValues );
  bsem_.give();
}
