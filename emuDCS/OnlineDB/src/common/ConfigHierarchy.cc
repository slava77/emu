// $Id: ConfigHierarchy.cc,v 1.1 2011/09/09 16:04:43 khotilov Exp $

#include "emu/db/ConfigHierarchy.h"

namespace emu { namespace db {

std::ostream& operator<<(std::ostream & os, const ConfigHierarchy & h)
{
  os<<"Hierarchy ( "<<h.prefix()<<" )";
  if (h.truncate_level_ < ConfigHierarchy::NOT_TRUNCATE) os<<" truncated after level "<<h.truncate_level_;
  os<<" :"<<std::endl;
  std::string type = h.typeOfHead();
  h.print(os, type, 0, 0);
  return os;
}


void ConfigHierarchy::print(std::ostream & os, const std::string &type, int padding, int verbosity) const
{
  std::string spaces(padding, '.');
  os<<spaces<<type<<std::endl;
  if (verbosity > 0 && def_)
  {
    xdata::Table * t = def_->tableDefinition(type);
    if (t)
    {
      os<<spaces;
      t->writeTo(os);
      os<<std::endl;
    }
  }

  std::vector<std::string> kids = typesOfChildern(type);
  for (std::vector<std::string>::iterator it = kids.begin(); it != kids.end(); it++)
  {
    print(os, *it, padding + 2, verbosity);
  }
}

}}
