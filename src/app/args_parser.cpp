#include "args_parser.h"

namespace {

constexpr const char* FLAG_IN = "-i";
constexpr const char* FLAG_OUT = "-o";
constexpr const char* FLAG_FILTER = "-f";

/** @brief Проверяет, является ли токен одним из управляющих флагов. */
bool isFlag(const char* token)
{
    return std::strcmp(token, FLAG_IN) == 0 ||
           std::strcmp(token, FLAG_OUT) == 0 ||
           std::strcmp(token, FLAG_FILTER) == 0;
}

}  // namespace

void ArgsParser::reset()
{
    _hasInFileName = false;
    _hasOutFileName = false;
    _inFileName.clear();
    _outFileName.clear();
    _filterDescriptors.clear();
}

ArgsParser::Result ArgsParser::parse(int argc, char* argv[])
{
    reset();

    if(argc <= 1)
        return Result::noArgs;

    int i = 1;
    while(i < argc)
    {
        const char* token = argv[i];

        if(std::strcmp(token, FLAG_IN) == 0)
        {
            // За -i должно следовать имя файла, и -i не должен дублироваться
            if(i + 1 >= argc || isFlag(argv[i + 1]) || _hasInFileName)
                return Result::badArgs;
            _inFileName = argv[i + 1];
            _hasInFileName = true;
            i += 2;
        }
        else if(std::strcmp(token, FLAG_OUT) == 0)
        {
            if(i + 1 >= argc || isFlag(argv[i + 1]) || _hasOutFileName)
                return Result::badArgs;
            _outFileName = argv[i + 1];
            _hasOutFileName = true;
            i += 2;
        }
        else if(std::strcmp(token, FLAG_FILTER) == 0)
        {
            // За -f должно следовать название фильтра
            if(i + 1 >= argc || isFlag(argv[i + 1]))
                return Result::badArgs;

            FilterDescriptor descriptor;
            descriptor.name = argv[i + 1];
            i += 2;

            // Остальные токены до следующего флага — параметры фильтра.
            while(i < argc && !isFlag(argv[i]))
            {
                descriptor.params.emplace_back(argv[i]);
                ++i;
            }
            _filterDescriptors.push_back(std::move(descriptor));
        }
        else
        {
            // Любой токен вне групп -i / -o / -f некорректен.
            return Result::badArgs;
        }
    }

    return Result::ok;
}
