/*
 * =====================================================================================
 *
 *       Filename:  FunctionLauncher.h
 *
 *    Description:  FunctionLauncher application that does actual work of
 *    transformation. It parses transformation string and executes it.
 *    This class is being addresed within EmuMonitoringCanvas objects.
 *    FunctionLauncher is being created statically and stores the list of
 *    available functions.
 *
 *    If you have created a function (base class AbstractFunction) to
 *    register it here please do the following:
 *
 *    - add include line in the appropriate section, i.e.
 *
 *      #include "MyFunction.h"
 *
 *    - add registration line within the constructor, i.e.
 *
 *      functions.insert(make_pair("My", new MyFunction()));
 *
 *    Now it can be used within the canvas XML map, i.e.
 *
 *      <Pad1>My(histogram1, histogram2)</Pad1>
 *
 *    Syntax:
 *
 *      function([functin_params])
 *      where function_params = function_param [, function_params]
 *            function_param  = [-]histogram|function([function_params])
 *
 *
 *        Version:  1.0
 *        Created:  04/14/2008 11:40:30 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Valdas Rapsevicius (VR), Valdas.Rapsevicius@cern.ch
 *        Company:  CERN, CH
 *
 * =====================================================================================
 */

#ifndef FunctionLauncher_h
#define FunctionLauncher_h

#include <map>
#include <vector>
#include <stack>

#include "../EmuMonitoringObject.h"
#include "AbstractFunction.h"

/** Functions */
#include "SetFunction.h"
#include "DrawFunction.h"
#include "ChamberMapFunction.h"
#include "AddFunction.h"
#include "DivideFunction.h"

/** Register of functions (see Constructor if you wish to register one) */
typedef map<std::string, AbstractFunction*> FunctionMap;
typedef FunctionMap::iterator FunctionMapIterator;

/** Functions are being executed in stack
 * so this is an exceptional stack item*/
struct  ExecItem
{
  AbstractFunction*    func;
  EmuMonitoringObject* me_emu;
  MonitorElement*      me;
};

class FunctionLauncher
{

public:

  /**
   * @brief  Class Constructor
   * @param
   * @return
   */
  FunctionLauncher ()
  {

    // Initialization of build-in functions
    setFunction = new SetFunction();
    drawFunction = new DrawFunction();

    // Register of functions
    functions.insert(make_pair("Set", setFunction));
    functions.insert(make_pair("Draw", drawFunction));
    functions.insert(make_pair("Add", new AddFunction()));
    functions.insert(make_pair("Divide", new DivideFunction()));
    functions.insert(make_pair("DrawChamberMap", new ChamberMapFunction()));

  };

  /**
   * @brief  Class destructor that removes function registry!
   * @param
   * @return
   */
  ~FunctionLauncher()
  {
    FunctionMapIterator iter;
    for (iter = functions.begin(); iter != functions.end(); iter++)
      {
        delete iter->second;
        iter->second = NULL;
      }
  };

  /**
   * @brief  Entry point function
   * @param  command Command line (actualy it is everything what lies inside
   * <Padx/> element.
   * @param  MEs Monitoring elements of the apropriate scale, e.g. EMU level
   * @return
   */
  void execute(const std::string command, ME_List& MEs)
  {
    if (!command.empty() && !MEs.empty())
      {
        if (REMATCH("^.+$", command))
          {
            std::string l_command = command;
            if (REMATCH("^[-a-zA-Z0-9_]+$", l_command))
              {
                l_command = "Draw(" + l_command + ")";
              }
            executeFunction(l_command, MEs);
          }
        else
          {
            cerr << "Wrong command syntax [" << command << "]!\n";
          }
      }
  };

private:

  /** Register of functions... */
  FunctionMap functions;

  // Pointers to build-in functions
  SetFunction* setFunction;
  DrawFunction* drawFunction;


  /**
   * @brief  Actual execution function
   * @param  command Command string
   * @param  MEs List of me's
   * @return
   */
  bool executeFunction(const std::string command, ME_List& MEs)
  {

    bool result = true;
    std::stack<ExecItem> execStack;
    ExecItem item;

    std::string lcom = command, name;

    //
    // remove spaces (if any)
    //
    REREPLACEM(" {1,}", lcom, "", "g");

    //
    // Lets eat lcom step by step ;)
    //
    while (!lcom.empty())
      {

        //
        // Get and remove starting name of something :)
        //
        name = lcom;
        REREPLACE("^([-a-zA-Z0-9_]+).*", name, "$1");
        REREPLACE("^[-a-zA-Z0-9_]+", lcom, "");

        //
        // Is it a  EmuMonitoringObject ?
        //
        if (lcom.empty() || REMATCH("^[\\),]", lcom))
          {

            bool mandatory = true;
            if (REMATCH("^-", name))
              {
                mandatory = false;
                REREPLACE("^-", name, "");
              }

            ME_List_iterator iemu = MEs.find(name);
            if (iemu != MEs.end())
              {
                item.func = NULL;
                item.me_emu = iemu->second;
                item.me = NULL;
                execStack.push(item);
              }
            else if (mandatory)
              {
                cerr << "EmuMonitoringObject [" << name << "] not found! Command [" << command << "]\n";
                result = false;
                break;
              }
            else
              {
                cerr << "Optional EmuMonitoringObject [" << name << "] not found in Command [" << command << "]. Ignoring...\n";
              }

          }
        else

          //
          // Next is function?
          //
          if (REMATCH("^\\(", lcom))
            {

              FunctionMapIterator ifunc = functions.find(name);
              if (ifunc != functions.end())
                {
                  item.func = ifunc->second;;
                  item.me_emu = NULL;
                  item.me = NULL;
                  execStack.push(item);
                }
              else
                {
                  cerr << "Function [" << name << "] not found! Command [" << command << "]\n";
                  result = false;
                  break;
                }
              if (REMATCH("^\\(\\)", lcom))
                {
                  REREPLACE("^.", lcom, "");
                }

            }

        //
        // Execute
        //
        while (REMATCH("^\\)", lcom))
          {

            REREPLACE("^.", lcom, "");

            //
            // Lets collect all the MEs from stack ;)
            //
            EmuMonitoringObject* lastEmu = NULL;
            FunctionParameters params;
            while (!execStack.empty() && execStack.top().func == NULL)
              {
                if (execStack.top().me_emu != NULL)
                  {
                    MonitorElement* me = dynamic_cast<MonitorElement*>(execStack.top().me_emu->getObject()->Clone());
                    params.insert(params.begin(), me);
                    execStack.top().me_emu->applyPadProperties();
                    if (lastEmu == NULL)
                      {
                        lastEmu = execStack.top().me_emu;
                      }
                  }
                else
                  {
                    params.push_back(execStack.top().me);
                    //lastEmu = NULL;
                  }
                execStack.pop();
              }

            // NULLify stuff
            item.func = NULL;
            item.me_emu = NULL;
            item.me = NULL;

            // Execute
            if (!execStack.empty() && execStack.top().func != NULL)
              {
                AbstractFunction* function = execStack.top().func;
                execStack.pop();

                item.me = function->calc(params);

                // Do something for built-in Draw function
                if (function == drawFunction && lastEmu != NULL)
                  {
                    std::string statOpt = lastEmu->getParameter("SetOptStat");
                    if (statOpt != "" )
                      {
                        gStyle->SetOptStat(statOpt.c_str());
                      }
                  }

                // Do an exception for built-in Set function
                if (function == setFunction && lastEmu != NULL)
                  {

                    std::string h_name  = lastEmu->getObject()->GetName();
                    std::string h_title = lastEmu->getObject()->GetTitle();

                    lastEmu->setObject(item.me);

                    lastEmu->getObject()->SetName(h_name.c_str());
                    lastEmu->getObject()->SetTitle(h_title.c_str());

                    item.me_emu = lastEmu;
                    item.me = NULL;
                  }

                execStack.push(item);

              }


            // Clear things ...
            for (unsigned int i = 0; i < params.size(); i++)
              {
                if (params[i] != NULL && params[i] != item.me)
                  {
                    delete params[i];
                  }
              }

          }

        //
        // Replace current command character...
        //
        if (!lcom.empty())
          {
            REREPLACE("^.", lcom, "");
          }

      }

    //
    // Execute remaining stuff on stack
    while (!execStack.empty())
      {

        /*
        if(execStack.top().me_emu != NULL){
          execStack.top().me_emu->applyPadProperties();
          execStack.top().me_emu->Draw();
          std::string statOpt = execStack.top().me_emu->getParameter("SetOptStat");
          if (statOpt != "" ) {
            gStyle->SetOptStat(statOpt.c_str());
          }
        }
        */

        if (execStack.top().me != NULL)
          {
            delete execStack.top().me;
          }

        execStack.pop();
      }

    return result;

  }

};

#endif
