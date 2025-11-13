cmake_minimum_required(VERSION 3.30)
project(SMTP VERSION 0.1 DESCRIPTION "SMTP implementation")

set(CMAKE_CXX_STANDARD 23)

add_library(SMTP
	 "source/Commands/AUTHCommand.cpp"
	 "source/Commands/DATACommand.cpp" 
	 "source/Commands/EHLOCommand.cpp" 
	 "source/Commands/HELOCommand.cpp"
	 "source/Commands/NOOPCommand.cpp" 
	 "source/Commands/HELPCommand.cpp" 
	 "source/Commands/MAILCommand.cpp" 
	 "source/Commands/RCPTCommand.cpp" 
	 "source/Commands/QUITCommand.cpp" 
	 "source/Commands/VRFYCommand.cpp"
	 "source/Commands/RSETCommand.cpp"
	 "source/SMTPSession.cpp"  
	 "source/SMTPBuffer.cpp" 
	 "source/SMTPState.cpp" 
	 "source/SMTPReply.cpp" 
	 "source/SMTPCommandParser.cpp"
	 "source/PLAINAuthHandler.cpp"
	 "source/LOGINAuthHandler.cpp"
	 "source/SMTPConfig.cpp"
	 "source/SMTPConfigBuilder.cpp" "include/SMTPIMailbox.h")

target_include_directories(SMTP PUBLIC "include")