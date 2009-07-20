#include "emu/dqm/tfanalyzer/RegEx.h"

std::map< std::string, std::vector<std::string> > RegExSearch(const char *pattern, const char *target)
{
//  stdlib++ interface to standard regex. Searches through the whole std::string
//  Result is a map array with matched patterns as keys and array of selected
//  subpatterns as a value
//  Example:
//	std::map< std::string, std::vector<std::string> > result = RegExSearch("(\\w+) (\\w+)", "This is a test");
//  result.begin()->first == "This is";
//  result.begin()->second[0] = "This";
//  result.begin()->second[1] = "is";
//  (--result.end())->first == "a test";
//  (--result.end())->second[0] = "a";
//  (--result.end())->second[1] = "test";

  std::map< std::string, std::vector<std::string> > retval;
  regex_t regsys;
  regsys.buffer    = 0;
  regsys.allocated = 0;
  regsys.fastmap   = 0;
  regsys.translate = 0;
  regsys.no_sub    = 0;
  regsys.used      = 0;
  re_syntax_options = RE_SYNTAX_AWK;
  re_syntax_options ^= RE_NO_GNU_OPS;

  if (re_compile_pattern(pattern, strlen(pattern), &regsys))
    {
      std::cout<<"Can't compile the pattern: "<<pattern<<std::endl;
      return retval;
    }
  re_registers regs;
  regs.num_regs = 0;

  while (1)
    {
      if (re_search(&regsys, target, strlen(target), 0, strlen(target), &regs)<0)
        {
          regfree(&regsys);
          return retval;
        }
#ifdef __DEBUG__
      std::cout<<regs.num_regs<<" matches found"<<std::endl;
#endif
      if (regs.num_regs)
        {
#ifdef __DEBUG__
          std::cout<<"First match range: "<<*regs.start<<" - "<<*regs.end<<std::endl;
#endif
          char match[*regs.end - *regs.start + 1];
          strncpy(match, &target[*regs.start], *regs.end - *regs.start);
          match[*regs.end - *regs.start] = '\0';
          retval[match] = std::vector<std::string>(0);
          std::vector<std::string> &submatches = retval.find(match)->second;

          for (unsigned i=1; i<regs.num_regs && regs.end[i]-regs.start[i]; i++)
            {
              char tmp[regs.end[i]-regs.start[i]+1];
              strncpy(tmp,&target[regs.start[i]],regs.end[i]-regs.start[i]);
              tmp[regs.end[i]-regs.start[i]] = '\0';
              submatches.push_back(tmp);
#ifdef __DEBUG__
              std::cout<<"Match "<<i<<": "<<tmp<<std::endl;
#endif
            }
          target += *regs.end;
        }
      else
        {
          regfree(&regsys);
          return retval;
        }
    }
  regfree(&regsys);
  return retval;
}

std::string RegExSubstitute(const char *pattern_from, const char *pattern_to, const char* target)
{
//  Perl's $var =~ s/(\\w)\\.(\\w)/$2.$1/gx can be written as
//  var = RegExSubstitude("(\\w)\\.(\\w)", "$2.$1", "first.second"); (var == "second.first")
  std::string retval;
  regex_t regsys;
  regsys.buffer    = 0;
  regsys.allocated = 0;
  regsys.fastmap   = 0;
  regsys.translate = 0;
  regsys.no_sub    = 0;
  regsys.used      = 0;
  re_syntax_options = RE_SYNTAX_AWK;
  re_syntax_options ^= RE_NO_GNU_OPS;

  if (re_compile_pattern(pattern_from, strlen(pattern_from), &regsys))
    {
      std::cout<<"Can't compilate the pattern: "<<pattern_from<<std::endl;
      return retval;
    }
  re_registers regs;
  regs.num_regs = 0;

  while (1)
    {
      if (re_search(&regsys, target, strlen(target), 0, strlen(target), &regs)<0)
        {
          retval.append(target,0,strlen(target));
          regfree(&regsys);
          return retval;
        }
#ifdef __DEBUG__
      std::cout<<regs.num_regs<<" matches found"<<std::endl;
#endif
      if (regs.num_regs)
        {
#ifdef __DEBUG__
          std::cout<<"First match range: "<<*regs.start<<" - "<<*regs.end<<std::endl;
#endif
          std::string subsitude = pattern_to;
          size_t var;
          while ( (var = subsitude.find("$")) != std::string::npos && subsitude[var-1] != '\\')
            {
              int k = atoi(&subsitude[var+1]);
              subsitude.replace(var,2,&target[regs.start[k]],regs.end[k]-regs.start[k]);
            }

          retval.append(target,0,*regs.start);
          retval.append(subsitude);
          target += *regs.end;
#ifdef __DEBUG__
          std::cout<<"Match "<<subsitude<<std::endl;
#endif
        }
      else
        {
          retval.append(target,0,strlen(target));
          regfree(&regsys);
          return retval;
        }
    }
  regfree(&regsys);
  return retval;
}

