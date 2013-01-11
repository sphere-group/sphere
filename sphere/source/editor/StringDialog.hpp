#ifndef STRING_DIALOG_HPP
#define STRING_DIALOG_HPP
#include <afxwin.h>
#include <string>
class CStringDialog : public CDialog
{
public:
  CStringDialog(const char* caption, const char* default_value);
  const char* GetValue() const;
private:
  virtual BOOL OnInitDialog();
  virtual void OnOK();
private:
  std::string m_caption;
  std::string m_value;
};
#endif
