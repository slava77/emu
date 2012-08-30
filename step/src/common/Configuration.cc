#include "emu/step/Configuration.h"

#include "emu/utils/IO.h"
#include "emu/utils/DOM.h"
#include "emu/utils/Xalan.h"
#include "emu/utils/String.h"
#include "emu/utils/System.h"

#include "xcept/Exception.h"

using namespace emu::utils;

emu::step::Configuration::Configuration( const string& XMLnamespace, 
					 xdata::String& testParametersFileName,
					 const map<string,string>& pCrateSettingsFileNames )
  : bsem_( toolbox::BSem::FULL ) // unlocked
  , namespace_( XMLnamespace )
{
  bsem_.take();
  testParametersXML_ = emu::utils::readFile( emu::utils::performExpansions( testParametersFileName ) );
  for ( map<string,string>::const_iterator p = pCrateSettingsFileNames.begin(); p != pCrateSettingsFileNames.end(); ++p )
    {
      if ( p->first.size() > 0 && p->second.size() > 0 )
	{
	  pCrateSettingsXMLs_[p->first] = emu::utils::readFile( emu::utils::performExpansions( p->second ) );
	}
    }
  bsem_.give();

  createXML();
}

void emu::step::Configuration::createXML()
{
  string xslt( emu::utils::readFile( emu::utils::performExpansions( "${BUILD_HOME}/emu/step/xml/configuration.xsl" ) ) );

  stringstream source;
  stringstream xsl;
  stringstream crates;
  stringstream tests;

  bsem_.take();

  modificationTime_ = emu::utils::getDateTime();

  xml_ = "<es:configuration xmlns:es=\"" + namespace_ + "\" modificationTime=\"" + modificationTime_ + "\"/>";

  source.str( testParametersXML_ );
  xsl.str( xslt );
  emu::utils::transformStreams( source, xsl, tests );
  xml_ = emu::utils::appendToSelectedNode( xml_, "/es:configuration", tests.str() );

  for ( map<string,string>::const_iterator p = pCrateSettingsXMLs_.begin(); p != pCrateSettingsXMLs_.end(); ++p )
    {
      map<string,string> param;
      param["GROUP"] = "'" + p->first + "'";
      source.clear(); // reset control state to good after previous operation
      source.str( p->second ); // refill buffer
      xsl.clear(); // reset control state to good after previous operation
      xsl.str( xslt ); // refill buffer
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

void emu::step::Configuration::setTestStatus( const string& testId, const string& status ){
  bsem_.take();
  xml_ = emu::utils::setSelectedNodesValues( xml_, "//es:testSequence/es:test[@id='" + testId + "']/@status", status );
  bsem_.give();
}

void emu::step::Configuration::setTestStatus( const string& status ){
  bsem_.take();
  xml_ = emu::utils::setSelectedNodesValues( xml_, "//es:testSequence/es:test/@status", status );
  bsem_.give();
}

void emu::step::Configuration::setTestProgress( const map<string,double>& groupsProgress ){
  map<string,string> values;
  for ( map<string,double>::const_iterator p = groupsProgress.begin(); p != groupsProgress.end(); ++p )
    {
      values["//es:peripheralCrates[@group='" + p->first + "']/@progress"] = emu::utils::stringFrom<int>( (int)p->second );
    }
  bsem_.take();
  xml_ = emu::utils::setSelectedNodesValues( xml_, values );
  bsem_.give();
}
