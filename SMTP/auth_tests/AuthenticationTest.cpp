#include <gtest/gtest.h>
#include "Commands/AUTHCommand.h"
#include "Commands/MAILCommand.h"
#include "Commands/RCPTCommand.h"
#include "Commands/DATACommand.h"
#include "PLAINAuthHandler.h"
#include "LOGINAuthHandler.h"
#include "SMTPContext.h"
#include "../../utils/Base64.h"

class AuthenticationTest : public ::testing::Test {
protected:
    ISXSMTP::SMTPContext context;

    void SetUp() override {
        context.state = ISXSMTP::SMTPStates::POST_EHLO;
        context.is_authenticated = false;
    }
};

TEST_F(AuthenticationTest, PLAINAuthenticationSuccess) {
    auto plain_handler = std::make_shared<ISXSMTP::PLAINAuthHandler>();

    const char raw_creds[] = "\0testuser\0testpass";
    std::string creds(raw_creds, sizeof(raw_creds) - 1);
    auto encoded = Base64::Encode({creds.begin(), creds.end()});
    std::string encodedStr(encoded.begin(), encoded.end());

    ISXSMTP::SMTPCommandArguments args(context, {{"credentials", encodedStr}}, "smtp.test", nullptr, plain_handler);

    ISXSMTP::AUTHCommand auth_cmd;
    auto replies = auth_cmd.Invoke(args);

    ASSERT_EQ(replies.size(), 1);
    EXPECT_EQ(replies[0].GetCode(), 235);
    EXPECT_TRUE(context.is_authenticated);
}

TEST_F(AuthenticationTest, PLAINAuthenticationFailure) {
    auto plain_handler = std::make_shared<ISXSMTP::PLAINAuthHandler>();

    const char raw_creds[] = "\0wronguser\0wrongpass";
    std::string creds(raw_creds, sizeof(raw_creds) - 1);
    auto encoded = Base64::Encode({creds.begin(), creds.end()});
    std::string encodedStr(encoded.begin(), encoded.end());

    ISXSMTP::SMTPCommandArguments args(context, {{"credentials", encodedStr}}, "smtp.test", nullptr, plain_handler);

    ISXSMTP::AUTHCommand auth_cmd;
    auto replies = auth_cmd.Invoke(args);

    ASSERT_EQ(replies.size(), 1);
    EXPECT_EQ(replies[0].GetCode(), 535);
    EXPECT_FALSE(context.is_authenticated);
}

TEST_F(AuthenticationTest, LOGINAuthenticationUsernamePrompt) {
    auto login_handler = std::make_shared<ISXSMTP::LOGINAuthHandler>();

    ISXSMTP::SMTPCommandArguments args(context, {{"credentials", ""}}, "smtp.test", nullptr, login_handler);

    ISXSMTP::AUTHCommand auth_cmd;
    auto replies = auth_cmd.Invoke(args);

    ASSERT_EQ(replies.size(), 1);
    EXPECT_EQ(replies[0].GetCode(), 324);
    EXPECT_FALSE(context.is_authenticated);
}

TEST_F(AuthenticationTest, LOGINAuthenticationTwoStepSuccess) {
    auto login_handler = std::make_shared<ISXSMTP::LOGINAuthHandler>();
    ISXSMTP::AUTHCommand auth_cmd;

    // Send username
    std::string usr = "testuser";
    auto encoded_usr = Base64::Encode({usr.begin(), usr.end()});
    std::string encoded_usr_str(encoded_usr.begin(), encoded_usr.end());

    ISXSMTP::SMTPCommandArguments args1(context, {{"credentials", encoded_usr_str}}, "smtp.test", nullptr, login_handler);
    auto replies1 = auth_cmd.Invoke(args1);

    ASSERT_EQ(replies1.size(), 1);
    EXPECT_EQ(replies1[0].GetCode(), 324);
    EXPECT_FALSE(context.is_authenticated);

    // Send password
    std::string pass = "testpass";
    auto encoded_pass = Base64::Encode({pass.begin(), pass.end()});
    std::string encoded_pass_str(encoded_pass.begin(), encoded_pass.end());

    ISXSMTP::SMTPCommandArguments args2(context, {{"credentials", encoded_pass_str}}, "smtp.test", nullptr, login_handler);
    auto replies2 = auth_cmd.Invoke(args2);

    ASSERT_EQ(replies2.size(), 1);
    EXPECT_EQ(replies2[0].GetCode(), 235);
    EXPECT_TRUE(context.is_authenticated);
}

TEST_F(AuthenticationTest, AuthRequiredForMAIL) {
    context.is_authenticated = false;

    ISXSMTP::SMTPCommandArguments args(context, {}, "smtp.test", nullptr, nullptr);
    ISXSMTP::MAILCommand mail_cmd;
    auto replies = mail_cmd.Invoke(args);

    ASSERT_EQ(replies.size(), 1);
    EXPECT_EQ(replies[0].GetCode(), 530);
}

TEST_F(AuthenticationTest, AuthRequiredForRCPT) {
    context.is_authenticated = false;
    context.state = ISXSMTP::SMTPStates::POST_MAIL;

    ISXSMTP::SMTPCommandArguments args(context, {}, "smtp.test", nullptr, nullptr);
    ISXSMTP::RCPTCommand rcpt_cmd;
    auto replies = rcpt_cmd.Invoke(args);

    ASSERT_EQ(replies.size(), 1);
    EXPECT_EQ(replies[0].GetCode(), 530);
}

TEST_F(AuthenticationTest, AuthRequiredForDATA) {
    context.is_authenticated = false;
    context.state = ISXSMTP::SMTPStates::POST_RCPT;

    ISXSMTP::SMTPCommandArguments args(context, {}, "smtp.test", nullptr, nullptr);
    ISXSMTP::DATACommand data_cmd;
    auto replies = data_cmd.Invoke(args);

    ASSERT_EQ(replies.size(), 1);
    EXPECT_EQ(replies[0].GetCode(), 530);
}

TEST_F(AuthenticationTest, AuthenticationNotSupported) {
    ISXSMTP::SMTPCommandArguments args(context, {{"credentials", "test"}}, "smtp.test", nullptr, nullptr);
    ISXSMTP::AUTHCommand auth_cmd;
    auto replies = auth_cmd.Invoke(args);

    ASSERT_EQ(replies.size(), 1);
    EXPECT_EQ(replies[0].GetCode(), 502);
}

TEST_F(AuthenticationTest, AuthBeforeEHLO) {
    context.state = ISXSMTP::SMTPStates::INITIAL;
    auto plain_handler = std::make_shared<ISXSMTP::PLAINAuthHandler>();

    ISXSMTP::SMTPCommandArguments args(context, {{"credentials", "test"}}, "smtp.test", nullptr, plain_handler);
    ISXSMTP::AUTHCommand auth_cmd;
    auto replies = auth_cmd.Invoke(args);

    ASSERT_EQ(replies.size(), 1);
    EXPECT_EQ(replies[0].GetCode(), 503);
}

TEST_F(AuthenticationTest, LOGINAuthenticationFailureWrongPassword) {
    auto login_handler = std::make_shared<ISXSMTP::LOGINAuthHandler>();
    ISXSMTP::AUTHCommand auth_cmd;

    std::string usr = "testuser";
    auto encoded_usr = Base64::Encode({usr.begin(), usr.end()});
    std::string encoded_usr_str(encoded_usr.begin(), encoded_usr.end());
    ISXSMTP::SMTPCommandArguments args1(context, {{"credentials", encoded_usr_str}}, "smtp.test", nullptr, login_handler);
    auth_cmd.Invoke(args1);

    ASSERT_FALSE(context.is_authenticated);

    std::string pass = "wrongpass";
    auto encoded_pass = Base64::Encode({pass.begin(), pass.end()});
    std::string encoded_pass_str(encoded_pass.begin(), encoded_pass.end());
    ISXSMTP::SMTPCommandArguments args2(context, {{"credentials", encoded_pass_str}}, "smtp.test", nullptr, login_handler);
    auto replies2 = auth_cmd.Invoke(args2);

    ASSERT_EQ(replies2.size(), 1);
    EXPECT_EQ(replies2[0].GetCode(), 535);
    EXPECT_FALSE(context.is_authenticated);
}

TEST_F(AuthenticationTest, LOGINAuthenticationFailureWrongUsername) {
    auto login_handler = std::make_shared<ISXSMTP::LOGINAuthHandler>();
    ISXSMTP::AUTHCommand auth_cmd;

    std::string usr = "wronguser";
    auto encoded_usr = Base64::Encode({usr.begin(), usr.end()});
    std::string encoded_usr_str(encoded_usr.begin(), encoded_usr.end());
    ISXSMTP::SMTPCommandArguments args1(context, {{"credentials", encoded_usr_str}}, "smtp.test", nullptr, login_handler);
    auth_cmd.Invoke(args1);

    ASSERT_FALSE(context.is_authenticated);

    std::string pass = "testpass";
    auto encoded_pass = Base64::Encode({pass.begin(), pass.end()});
    std::string encoded_pass_str(encoded_pass.begin(), encoded_pass.end());
    ISXSMTP::SMTPCommandArguments args2(context, {{"credentials", encoded_pass_str}}, "smtp.test", nullptr, login_handler);
    auto replies2 = auth_cmd.Invoke(args2);

    ASSERT_EQ(replies2.size(), 1);
    EXPECT_EQ(replies2[0].GetCode(), 535);
    EXPECT_FALSE(context.is_authenticated);
}

TEST_F(AuthenticationTest, PLAINAuthenticationIncorrectFormat) {
    auto plain_handler = std::make_shared<ISXSMTP::PLAINAuthHandler>();

    std::string creds = "testusertestpass";
    auto encoded = Base64::Encode({creds.begin(), creds.end()});
    std::string encodedStr(encoded.begin(), encoded.end());

    ISXSMTP::SMTPCommandArguments args(context, {{"credentials", encodedStr}}, "smtp.test", nullptr, plain_handler);
    ISXSMTP::AUTHCommand auth_cmd;
    auto replies = auth_cmd.Invoke(args);

    ASSERT_EQ(replies.size(), 1);
    EXPECT_EQ(replies[0].GetCode(), 535);
    EXPECT_FALSE(context.is_authenticated);
}