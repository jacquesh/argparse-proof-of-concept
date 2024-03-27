#include <stdio.h>
#include <string>
#include <vector>

// Reference commands:
// * radosgw-admin
// * cargo?
// * go?
// * rg?
// * certbot?
// * git?
// * fzf?
// * croc?
// * jq?
// * docker?



// Reference libraries:
// * https://docs.python.org/3/library/argparse.html
// * https://github.com/pallets/click
// * https://github.com/tiangolo/typer
// * https://github.com/clap-rs/clap
// * https://docs.rs/getopts/latest/getopts/
// * https://github.com/alecthomas/kong (this one I just found by searching "golang argument parsing" though - I should try find somebody who works in- or writes a lot of golang, and ask them what the standard is there)
// * https://github.com/charmbracelet/bubbletea
// * https://tj.github.io/commander.js/



// Standards:
// * Always accept `mycmd --help` and have it print help and exit (if you have subcommands that aren't dashed then you should *also* support `mycmd help`, but still support the dashed version because that's what everybody will expect)
// * Don't block on input with no output, especially on the plain `mycmd` command with no flags
// * Consider failing without doing any work if unexpected args are passed

struct cli_args
{
    std::string subcommand;
    bool valid;

    const std::vector<std::string_view>& free();
    std::string_view operator[](std::string_view key);
};

struct cli_spec
{
    cli_spec(std::string_view description);

    cli_spec& subcommand(std::string_view cmd);
    cli_spec& param(std::string_view name);
    cli_spec& positional_param(std::string_view name);

    cli_spec& include_free_args(std::string_view free_arg_help_name); // TODO: Maybe this API is confusing, users might try to reference the free args the same way as they do the named args?

    cli_args parse(int argc, char** argv);

    std::string m_description;
};

uint64_t parse_int(std::string_view str);

int main(int argc, char** argv)
{
    // TODO: Add a --base flag that applies to all commands and sets the base of the operands (support at least 2, 10, 16?)
    // math add 5 10
    // math add 5 10 3 23
    // math add --mod 7 5 10
    // math add --mod 7 -- 5 10
    // math mul 8 5
    // math mul 4 2 5
    // math mul --mod 7 8 5
    // math mul --mod 7 -- 8 5
    // math div real 10 4
    // math div integer 10 4
    // math --help
    // math add --help
    // math div --help
    cli_spec cli("Mathematicli - A cli for doing arithmetic");

    cli.subcommand("add")
        .param("mod")
        .include_free_args("summand");
    cli.subcommand("mul")
        .param("mod")
        .include_free_args("value");

    // TODO: Can we add another command that demos the actual hierarchy here?
    //       The idea would be that if I add a named param to the higher-up subcommand
    //       (in this case `div`) then it will be available to all subcommands.
    auto div = cli.subcommand("div");
    div.subcommand("real")
        .positional_param("divisor")
        .positional_param("dividend");
    div.subcommand("integer")
        .positional_param("divisor")
        .positional_param("dividend");

    cli_args args = cli.parse(argc, argv);
    if(!args.valid)
    {
        return 1;
    }

    // TODO: How do we test subcommands?
    //       * We've used strings here - this doesn't help us get the flags/options passed in though, so we resort to subscript operators which don't nicely handle required-vs-optional params
    //       * We could call functions and pass in the arguments there (this is what Typer does - https://github.com/tiangolo/typer) although I don't know how you properly define descriptions for each argument then, or how you ensure consistency of arguments across subcommands...
    //       * We could pass in/return a std::variant of structs, each of which represents the arguments for a subcommand
    //          This is difficult to do in C++ because we don't have RTTI. We can achieve it with some macro bullshit though.
    //          We should try implement the same thing in rust (where we have fancy macros) and see if that produces cleaner results?
    if(args.subcommand == "add")
    {
        uint64_t result = 0;
        for(std::string_view arg : args.free())
        {
            result += parse_int(arg);
        }
        if(args["mod"].has_value())
        {
            result %= parse_int(args["mod"].value());
        }
        printf("%llu\n", result);
    }
    else if(args.subcommand == "mul")
    {
        uint64_t result = 1;
        for(std::string_view arg : args.free())
        {
            result *= parse_int(arg);
        }
        if(args["mod"].has_value())
        {
            result %= parse_int(args["mod"].value());
        }
        printf("%llu\n", result);
    }
    else if(args.subcommand == "div real")
    {
        uint64_t divisor = parse_int(args["divisor"]);
        uint64_t dividend = parse_int(args["dividend"]);
        printf("%.2f\n", double(divisor)/double(dividend));
    }
    else if(args.subcommand == "div integer")
    {
        uint64_t divisor = parse_int(args["divisor"]);
        uint64_t dividend = parse_int(args["dividend"]);
        printf("%llu\n", divisor/dividend);
    }
    return 0;
}




// ===== Implementation =====

cli_spec::cli_spec(std::string_view description)
    : m_description(description)
{
}

cli_args cli_spec::parse(int argc, char** argv)
{
    cli_args result = {};
    return result;
}
