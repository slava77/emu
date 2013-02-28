#ifndef __emu_me11dev_ActionValue_h__
#define __emu_me11dev_ActionValue_h__

namespace emu { namespace me11dev {


/** \class ActionValue
 * templated mixin class to store Action's data value for an Action with single textbox
 */
template<typename T>
class ActionValue
{
public:
  ActionValue(T def_val): value_(def_val) {}
  T value() const { return value_; }
  void value(T new_val) { value_ = new_val; }
private:
  T value_;
};

/** \class Action2Values
 * templated mixin class to store Action's data values for an Action with two textboxes
 */
template<typename T1, typename T2>
class Action2Values
{
public:
  Action2Values(T1 def_val1, T2 def_val2): value1_(def_val1), value2_(def_val2) {}
  T1 value1() const { return value1_; }
  T2 value2() const { return value2_; }
  void value1(T1 new_val) { value1_ = new_val; }
  void value2(T2 new_val) { value2_ = new_val; }
private:
  T1 value1_;
  T2 value2_;
};

}} // namespaces

#endif

