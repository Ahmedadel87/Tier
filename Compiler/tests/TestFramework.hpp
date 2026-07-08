/*

WIP

TODO: Make sure of correctness
TODO: Then do cmake stuff (more modular cmake and a cmake to build the tests)

*/

#include <iostream>
#include <string_view>
#include <vector>
#include <fstream>

namespace TestVariables
{
    enum class Op
    {
        EQ,
        GT,
        LT,
        GE,
        LE,
        NE,
        TRUE,
        FALSE
    };

    constexpr std::string_view op_name(Op op)
    {
        switch(op)
        {
            case Op::EQ:
                return "==";
            case Op::GT:
                return ">";
            case Op::LT:
                return "<";
            case Op::GE:
                return ">=";
            case Op::LE:
                return "<=";
            case Op::NE:
                return "!=";
            case Op::TRUE:
                return "(expected true)";
            case Op::FALSE:
                return "(expected false)";
            default:
                return "how did you get here?!!?!?!? srsly tho";
        }
    }

    struct TestInstr
    {

        TestVariables::Op type;
        std::string_view lvalue;
        std::string_view rvalue;
        bool passed;
        size_t line;
        std::string_view file;
        bool is_assert;
    };

    struct DiagnosticResult
    {
        size_t total;
        size_t failed;

        DiagnosticResult& operator+=(const DiagnosticResult& rhs)
        {
            this->total += rhs.total;
            this->failed += rhs.failed;

            return *this;
        }
    };

    struct Test
    {
            std::vector<TestInstr> m_test_instructions;
            std::string m_test_suite_name;
            std::string m_test_name;

            Test(std::string test_suite_name, std::string test_name)
                : m_test_suite_name(test_suite_name) , m_test_name(test_name)
            {  
            }

            DiagnosticResult diagnostic() const
            {
                std::cerr << "\x1B[36m" << m_test_suite_name << ": " << m_test_name << ":\x1B[0m\n";
                bool failed = false;
                size_t amount_failed = 0;
                size_t amount_done = 0;
                for(const TestInstr& instr : m_test_instructions)
                {
                    amount_done++;
                    if(!instr.passed) 
                    {
                        amount_failed++;
                        std::cout << "\x1B[0m";
                        failed = true;
                        std::cerr << "    " << instr.file << ':' << instr.line << "\n\x1B[31m    " 
                        << ((instr.is_assert) ? "Assert " : "") << "Failed: "
                                << instr.lvalue << ' ' << op_name(instr.type) << ' ' << instr.rvalue << "\n\n";
                        if(instr.is_assert) break;
                    }
                }
                if(!failed) std::cout << "\x1B[32;1m[Passed]\n\n";
                std::cout << "\x1B[0m";

                return {amount_done, amount_failed};
            }

    };

    std::vector<Test> tests;
}

#define TEST(test_suite_name, test_name) \
        bool has_assert_failed = true; \
        TestVariables::tests.push_back(TestVariables::Test(#test_suite_name, #test_name));

#define TEST_EQ(x, y) \
        do{ \
        if(!has_assert_failed) \
            TestVariables::tests.back().m_test_instructions.push_back(TestVariables::TestInstr{TestVariables::Op::EQ, #x, #y, (x) == (y), __LINE__, __FILE__, false}); \
        } while(0)

#define ASSERT_EQ(x, y) \
        do{ \
        if(!has_assert_failed) \
            TestVariables::tests.back().m_test_instructions.push_back(TestVariables::TestInstr{TestVariables::Op::EQ, #x, #y, (x) == (y), __LINE__, __FILE__, true}); \
        if(!TestVariables::tests.back().m_test_instructions.back().passed) has_assert_failed = true; \
        } while(0)

#define TEST_GT(x, y) \
        do{ \
        if(!has_assert_failed) \
            TestVariables::tests.back().m_test_instructions.push_back(TestVariables::TestInstr{TestVariables::Op::GT, #x, #y, (x) > (y), __LINE__, __FILE__, false}); \
        } while(0)

#define ASSERT_GT(x, y) \
        do{ \
        if(!has_assert_failed) \
            TestVariables::tests.back().m_test_instructions.push_back(TestVariables::TestInstr{TestVariables::Op::GT, #x, #y, (x) > (y), __LINE__, __FILE__, true}); \
        if(!TestVariables::tests.back().m_test_instructions.back().passed) has_assert_failed = true; \
        } while(0)

#define TEST_LT(x, y) \
        do{ \
        if(!has_assert_failed) \
            TestVariables::tests.back().m_test_instructions.push_back(TestVariables::TestInstr{TestVariables::Op::LT, #x, #y, (x) < (y), __LINE__, __FILE__, false}); \
        } while(0)

#define ASSERT_LT(x, y) \
        do{ \
        if(!has_assert_failed) \
            TestVariables::tests.back().m_test_instructions.push_back(TestVariables::TestInstr{TestVariables::Op::LT, #x, #y, (x) < (y), __LINE__, __FILE__, true}); \
        if(!TestVariables::tests.back().m_test_instructions.back().passed) has_assert_failed = true; \
        } while(0)

#define TEST_GE(x, y) \
        do{ \
        if(!has_assert_failed) \
            TestVariables::tests.back().m_test_instructions.push_back(TestVariables::TestInstr{TestVariables::Op::GE, #x, #y, (x) >= (y), __LINE__, __FILE__, false}); \
        } while(0)

#define ASSERT_GE(x, y) \
        do{ \
        if(!has_assert_failed) \
            TestVariables::tests.back().m_test_instructions.push_back(TestVariables::TestInstr{TestVariables::Op::GE, #x, #y, (x) >= (y), __LINE__, __FILE__, true}); \
        if(!TestVariables::tests.back().m_test_instructions.back().passed) has_assert_failed = true; \
        } while(0)

#define TEST_LE(x, y) \
        do{ \
        if(!has_assert_failed) \
            TestVariables::tests.back().m_test_instructions.push_back(TestVariables::TestInstr{TestVariables::Op::LE, #x, #y, (x) <= (y), __LINE__, __FILE__, false}); \
        } while(0)

#define ASSERT_LE(x, y) \
        do{ \
        if(!has_assert_failed) \
            TestVariables::tests.back().m_test_instructions.push_back(TestVariables::TestInstr{TestVariables::Op::LE, #x, #y, (x) <= (y), __LINE__, __FILE__, true}); \
        if(!TestVariables::tests.back().m_test_instructions.back().passed) has_assert_failed = true; \
        } while(0)

#define TEST_NE(x, y) \
        do{ \
        if(!has_assert_failed) \
            TestVariables::tests.back().m_test_instructions.push_back(TestVariables::TestInstr{TestVariables::Op::NE, #x, #y, (x) != (y), __LINE__, __FILE__, false}); \
        } while(0)

#define ASSERT_NE(x, y) \
        do{ \
        if(!has_assert_failed) \
            TestVariables::tests.back().m_test_instructions.push_back(TestVariables::TestInstr{TestVariables::Op::NE, #x, #y, (x) != (y), __LINE__, __FILE__, true}); \
        if(!TestVariables::tests.back().m_test_instructions.back().passed) has_assert_failed = true; \
        } while(0)
    
#define TEST_TRUE(x) \
        do{ \
        if(!has_assert_failed) \
            TestVariables::tests.back().m_test_instructions.push_back(TestVariables::TestInstr{TestVariables::Op::TRUE, #x, "", (x), __LINE__, __FILE__, false}); \
        } while(0)

#define ASSERT_TRUE(x) \
        do{ \
        if(!has_assert_failed) \
            TestVariables::tests.back().m_test_instructions.push_back(TestVariables::TestInstr{TestVariables::Op::TRUE, #x, "", (x), __LINE__, __FILE__, true}); \
        if(!TestVariables::tests.back().m_test_instructions.back().passed) has_assert_failed = true; \
        } while(0)

#define TEST_FALSE(x) \
        do{ \
        if(!has_assert_failed) \
            TestVariables::tests.back().m_test_instructions.push_back(TestVariables::TestInstr{TestVariables::Op::FALSE, #x, "", !(x), __LINE__, __FILE__, false}); \
        } while(0)

#define ASSERT_FALSE(x) \
        do{ \
        if(!has_assert_failed) \
            TestVariables::tests.back().m_test_instructions.push_back(TestVariables::TestInstr{TestVariables::Op::FALSE, #x, "", !(x), __LINE__, __FILE__, true}); \
        if(!TestVariables::tests.back().m_test_instructions.back().passed) has_assert_failed = true; \
        } while(0)

#define TEST_TOK(result, ...) \
        do{ \
        ASSERT_TRUE(result.has_value()); \
        auto&& _tok = *result; \
        Token::Token _expected_tok = {__VA_ARGS__}; \
        TEST_EQ(_tok.type, _expected_tok.type); \
        TEST_EQ(_tok.location.file_id, _expected_tok.location.file_id); \
        TEST_EQ(_tok.location.offset, _expected_tok.location.offset); \
        TEST_EQ(_tok.location.length, _expected_tok.location.length); \
        } while(0)

#define RUN_TESTS() \
    do{ \
    TestVariables::DiagnosticResult result = {}; \
    for(const TestVariables::Test& test : TestVariables::tests) \
    { \
        result += test.diagnostic(); \
    } \
    std::cout << "Completed Tests.\n\n================ Summary ================\n\n"; \
    std::cout << result.total - result.failed << " Passed.\n"; \
    std::cout << result.failed << " Failed.\n"; \
    } while(0)