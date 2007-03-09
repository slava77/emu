/*
 * Copyright 1999-2004 The Apache Software Foundation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/**
 * 
 * @author David N. Bertoni (david_n_bertoni@lotus.com)
 * */



// Base class header file.
#include "EmuXalanTransformerProblemListener.h"



#if defined(XALAN_CLASSIC_IOSTREAMS)
#include <iostream.h>
#else
#include <ostream>
#endif



#include <xalanc/PlatformSupport/DOMStringHelper.hpp>
#include <xalanc/PlatformSupport/DOMStringPrintWriter.hpp>

#include <iostream>
#include <sstream>

#include <xalanc/XalanDOM/XalanNode.hpp>

#include "xcept/tools.h"

XALAN_CPP_NAMESPACE_BEGIN



EmuXalanTransformerProblemListener::EmuXalanTransformerProblemListener
( MemoryManagerType& theManager,
  log4cplus::Logger* thelogger   ) :
  ProblemListener(),
  m_problemListener(theManager, (PrintWriter*)0),
  m_logger(thelogger)
{
}



EmuXalanTransformerProblemListener::~EmuXalanTransformerProblemListener()
{
}



void
EmuXalanTransformerProblemListener::setPrintWriter(PrintWriter*	thePrintWriter)
{
  m_problemListener.setPrintWriter(thePrintWriter);
}



void
EmuXalanTransformerProblemListener::problem
(
 eProblemSource		     where,
 eClassification	     classification, 
 const XalanNode*	     sourceNode,
 const ElemTemplateElement*  styleNode,
 const XalanDOMString&	     msg,
 const XalanDOMChar*	     uri,
 int			     lineNo,
 int			     charOffset
 )
  throw (xdaq::exception::Exception)
{

  XALAN_USING_STD(cerr);
  XALAN_USING_STD(endl);
  XALAN_USING_STD(string);
  XALAN_USING_STD(stringstream);

  string origin;
  stringstream logMessage;

  switch( where ) 
    {
    case eXMLPARSER:
      origin = "XML parser";
      break;
    case eXSLPROCESSOR:
      origin = "XSL processor";
      break;
    case eXPATH:
      origin = "XPATH";
      break;
    default:
      origin = "unknown origin";
      break;
    }

  logMessage << "Xalan message from " << origin
	     << ": " << msg << endl;
  if ( 0 != sourceNode ) logMessage << " node: " << sourceNode->getNodeName() << endl;
  if ( 0 != uri        ) logMessage << " URI : " << uri << endl;
  if ( 0 <= lineNo     ) logMessage << " line: " << lineNo << endl;
  if ( 0 <= charOffset ) logMessage << " char: " << charOffset << endl;

  switch( classification )
    {
    case eERROR:
//       cerr << "FATAL: " << logMessage.str();
      XCEPT_RAISE(xdaq::exception::Exception, logMessage.str());
      LOG4CPLUS_FATAL( (*m_logger), logMessage.str() );
      break;
    case eWARNING:
//       cerr << "ERROR: " << logMessage.str();
      XCEPT_RAISE(xdaq::exception::Exception, logMessage.str());
      LOG4CPLUS_ERROR( (*m_logger), logMessage.str() );
      break;
    case eMESSAGE:
//       cerr << "WARN: "  << logMessage.str();
      LOG4CPLUS_WARN( (*m_logger), logMessage.str() );
      break;
    default:
//       cerr << "ERROR: " << logMessage.str();
      LOG4CPLUS_ERROR( (*m_logger), logMessage.str() );
      break;
    }
}



XALAN_CPP_NAMESPACE_END
