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

#ifndef __EmuXalanTransformerProblemListener_h__
#define __EmuXalanTransformerProblemListener_h__

// Base include file.  Must be first.
#include "xalanc/XalanTransformer/XalanTransformerDefinitions.hpp"

#if defined(XALAN_CLASSIC_IOSTREAMS)
class ostream;
#else
#include <iosfwd>
#endif


#include "log4cplus/logger.h"
// #include "log4cplus/loggingmacros.h"
#include "xdaq/exception/Exception.h"

// Xalan header files.
#include <xalanc/XSLT/ProblemListenerDefault.hpp>


XALAN_CPP_NAMESPACE_BEGIN


/// \class EmuXalanTransformerProblemListener 
/// \brief Relay Xalan messages to XDAQ application's logger. 

///
/// Emu implementation of ProblemListener for XalanTransformer.
///
class XALAN_TRANSFORMER_EXPORT EmuXalanTransformerProblemListener : public ProblemListener
{
public:

#if defined(XALAN_NO_STD_NAMESPACE)
  typedef ostream			StreamType;
#else
  typedef std::ostream	StreamType;
#endif

  /// constructor

  ///
  /// @param theManager Xalan memory manager 
  /// @param theLogger logger
  ///
  EmuXalanTransformerProblemListener( MemoryManagerType&  theManager,
				      log4cplus::Logger*  theLogger );

  /// destructor
  virtual
    ~EmuXalanTransformerProblemListener();

  ///
  /// \defgroup inherited methods inherited from Xalan's ProblemListener
  ///
  /// @{

  /// method called by Xalan

  ///
  /// @param pw used by Xalan
  ///	
  virtual void
  setPrintWriter(PrintWriter*		pw);


  /// relay the Xalan error message to XDAQ application's logger or raise a XDAQ exception

  /// method called by Xalan if it encounters a problem
  ///
  /// @param where the Xalan module where the problem was encountered
  /// @param classification severity of problem
  /// @param sourceNode Xalan node
  /// @param styleNode ?
  /// @param msg message
  /// @param uri URI
  /// @param lineNo line in file
  /// @param charOffset character in line
  ///
  virtual void
  problem(
	  eProblemSource	        where,
	  eClassification		classification,
	  const XalanNode*		sourceNode,
	  const ElemTemplateElement*	styleNode,
	  const XalanDOMString&		msg,
	  const XalanDOMChar*		uri,
	  int				lineNo,
	  int				charOffset)
    throw (xdaq::exception::Exception);
/// @}

private:

  /// default constructor
  EmuXalanTransformerProblemListener();

  /// copy contructor (not implemented)
  EmuXalanTransformerProblemListener(const EmuXalanTransformerProblemListener&);

  /// problem listener
  ProblemListenerDefault	m_problemListener;

  /// logger to relay problem messages to
  log4cplus::Logger*            m_logger;
};



XALAN_CPP_NAMESPACE_END



#endif	// __EmuXalanTransformerProblemListener_h__
