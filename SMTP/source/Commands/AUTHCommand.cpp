#include "Commands/AUTHCommand.h"
#include "SMTPReply.h"
#include "SMTPState.h"
#include <iostream>

std::vector<ISXSMTP::SMTPReply> ISXSMTP::AUTHCommand::Invoke(SMTPCommandArguments arguments) {
  if (arguments.context.state != SMTPStates::POST_EHLO) {
    return { SMTPReply::BadSequenceOfCommands() };
  }

  if (!arguments.auth_handler) {
    return { {502, "Authentication not supported."} };
  }

  const auto& payload_it = arguments.arguments.find("credentials");
  std::string payload = payload_it != arguments.arguments.end() ? payload_it->second : "";

  if (arguments.auth_handler->Authenticate(payload)) {
    return { {235, "Authentication Succeeded"} };
  }

  if (arguments.auth_handler->GetName() == "LOGIN" && !isUsernameSet) {
    isUsernameSet = true;
    return { {324, "UGFzc3dvcmQ6"} }; // Decodes to: "Password:"
  }

  return {{535, "Authentication credentials invalid"}};
}

std::string ISXSMTP::AUTHCommand::GetName() {
  return "AUTH";
}

std::string ISXSMTP::AUTHCommand::GetSyntax() {
  return "AUTH [credentials]";
}