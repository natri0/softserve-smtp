#ifndef AUTH_COMMAND_H
#define AUTH_COMMAND_H

#include "SMTPCommandBase.h"

namespace ISXSMTP {
  class AUTHCommand final : public SMTPCommandBase {
  public:
    std::vector<SMTPReply> Invoke(SMTPCommandArguments arguments) override;
    std::string GetName() override;
    std::string GetSyntax() override;
  };
}

#endif