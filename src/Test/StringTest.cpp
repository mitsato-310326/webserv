#include "TestScript.hpp"
#include "StringUtils.hpp"

bool DecodeStringTest(std::string str, const std::string& label,
                             const std::string& expected)
{
    bool ok = false;
    try
    {
        const std::string decoded = DecodeString(str);
        ok = decoded == expected;
    }
    catch (const std::exception&)
    {
        ok = false;
    }
    check(label, ok);
    return ok;
}

bool DecodeStringErrorTest(std::string str, const std::string& label)
{
    bool ok = false;
    try
    {
        DecodeString(str);
    }
    catch (const urlDecodeException&)
    {
        ok = true;
    }
    catch (const std::exception&)
    {
        ok = false;
    }
    check(label, ok);
    return ok;
}

bool csplitTest(std::string str)
{
    std::vector<std::string> vector = csplit(str, ':');
    size_t n = vector.size();
    for (size_t i = 0; i < n; i++)
    {
        std::cout << vector[i];
        if (i == n - 1)
            std::cout << std::endl;
        else
            std::cout << ", ";
    }
    return (true);
}

int StringTest(void)
{
    bool DecodeString = false;
    bool Csplit = false;

    if (DecodeString)
    {
        int failed = 0;
        title("DecodeString: valid inputs");
        failed += !DecodeStringTest("", "empty string", "");
        failed += !DecodeStringTest("plain/path", "no encoding", "plain/path");
        failed += !DecodeStringTest("66", "digits without percent stay unchanged", "66");
        failed += !DecodeStringTest("%41", "single encoded byte", "A");
        failed += !DecodeStringTest("%41%42%43", "adjacent encodings", "ABC");
        failed += !DecodeStringTest("hello%20world", "space in middle", "hello world");
        failed += !DecodeStringTest("%20hello%20", "encoding at both ends", " hello ");
        failed += !DecodeStringTest("%2f%2F", "lowercase and uppercase hex", "//");
        failed += !DecodeStringTest("/a%3Fb%23c", "encoded delimiters", "/a?b#c");
        failed += !DecodeStringTest("a+b%2Bc", "literal plus stays plus", "a+b+c");
        failed += !DecodeStringTest("%25", "encoded percent", "%");
        failed += !DecodeStringTest("%2541", "do not decode twice", "%41");
        failed += !DecodeStringTest("%2525", "nested percent stays encoded", "%25");
        failed += !DecodeStringTest("%25%41", "continue after decoded percent", "%A");
        failed += !DecodeStringTest("%25GG", "decoded percent is not parsed again", "%GG");
        failed += !DecodeStringTest("%00", "NUL byte", std::string(1, '\0'));
        failed += !DecodeStringTest("a%00b", "embedded NUL", std::string("a\0b", 3));
        failed += !DecodeStringTest(std::string("a\0%41", 5), "raw NUL before encoding", std::string("a\0A", 3));
        failed += !DecodeStringTest("%0D%0A", "CRLF bytes", "\r\n");
        failed += !DecodeStringTest("%7F%80%FF", "full byte range", std::string("\x7f\x80\xff", 3));
        failed += !DecodeStringTest("%E7%8C%AB", "UTF-8 bytes", std::string("\xe7\x8c\xab", 3));
        failed += !DecodeStringTest(std::string(1000, 'a') + "%41", "long plain prefix", std::string(1000, 'a') + "A");
        
        title("DecodeString: invalid inputs must throw urlDecodeException");
        const char* invalid[] = {
            "%", "%4", "abc%", "abc%4", "%GG", "%1G", "%G1",
            "%+1", "%-1", "% 1", "%1 ", "%\t1", "%1\n",
            "%%41", "%4%41", "%41%", "%41%GG"
        };
        for (unsigned int i = 0; i < sizeof(invalid) / sizeof(invalid[0]); ++i)
        failed += !DecodeStringErrorTest(invalid[i], std::string("reject: ") + invalid[i]);
        
        std::cout << "\n39 cases, " << failed << " failed\n";
        return failed == 0 ? 0 : 1;
    }
    if (Csplit == true)
    {
        csplitTest("a:b:c");
    }
    return (0);
}
