/*******************************************************************************
 * CLI - A simple command line interface.
 * Copyright (C) 2016-2021 Daniele Pallastrelli
 *
 * Boost Software License - Version 1.0 - August 17th, 2003
 *
 * Permission is hereby granted, free of charge, to any person or organization
 * obtaining a copy of the software and accompanying documentation covered by
 * this license (the "Software") to use, reproduce, display, distribute,
 * execute, and transmit the Software, and to prepare derivative works of the
 * Software, and to permit third-parties to whom the Software is furnished to
 * do so, all subject to the following:
 *
 * The copyright notices in the Software and this entire statement, including
 * the above license grant, this restriction and the following disclaimer,
 * must be included in all copies of the Software, in whole or in part, and
 * all derivative works of the Software, unless such copies or derivative
 * works are solely in the form of machine-executable object code generated by
 * a source language processor.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
 * SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
 * FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 ******************************************************************************/

#if defined(WIN32) || defined(_WIN32) || defined(_WIN64)
    #error "AsyncSession only works on POSIX platforms."
#endif


#ifdef CLI_EXAMPLES_USE_STANDALONEASIO_SCHEDULER
    #include <cli/standaloneasioscheduler.h>
    #include <cli/standaloneasiocliasyncsession.h>
    namespace cli
    {
        using MainScheduler = StandaloneAsioScheduler;
        using CliAsyncSession = StandaloneAsioCliAsyncSession;
    } // namespace cli
#elif defined(CLI_EXAMPLES_USE_BOOSTASIO_SCHEDULER)
    #include <cli/boostasioscheduler.h>
    #include <cli/boostasiocliasyncsession.h>
    namespace cli
    {    
        using MainScheduler = BoostAsioScheduler;
        using CliAsyncSession = BoostAsioCliAsyncSession;
    }
#else
    #error either CLI_EXAMPLES_USE_STANDALONEASIO_SCHEDULER or CLI_EXAMPLES_USE_BOOSTASIO_SCHEDULER must be defined
#endif

#include <cli/cli.h>

using namespace cli;
using namespace std;

int main()
{
    try
    {
    
        // setup cli

        auto rootMenu = make_unique< Menu >( "cli" );
        rootMenu -> Insert(
                "hello",
                [](std::ostream& out){ out << "Hello, world\n"; },
                "Print hello world" );
        rootMenu -> Insert(
                "hello_everysession",
                [](std::ostream&){ Cli::cout() << "Hello, everybody" << std::endl; },
                "Print hello everybody on all open sessions" );
        rootMenu -> Insert(
                "answer",
                [](std::ostream& out, int x){ out << "The answer is: " << x << "\n"; },
                "Print the answer to Life, the Universe and Everything " );
        rootMenu -> Insert(
                "color",
                [](std::ostream& out){ out << "Colors ON\n"; SetColor(); },
                "Enable colors in the cli" );
        rootMenu -> Insert(
                "nocolor",
                [](std::ostream& out){ out << "Colors OFF\n"; SetNoColor(); },
                "Disable colors in the cli" );

        auto subMenu = make_unique< Menu >( "sub" );
        subMenu -> Insert(
                "hello",
                [](std::ostream& out){ out << "Hello, submenu world\n"; },
                "Print hello world in the submenu" );
        subMenu -> Insert(
                "demo",
                [](std::ostream& out){ out << "This is a sample!\n"; },
                "Print a demo string" );

        auto subSubMenu = make_unique< Menu >( "subsub" );
            subSubMenu -> Insert(
                "hello",
                [](std::ostream& out){ out << "Hello, subsubmenu world\n"; },
                "Print hello world in the sub-submenu" );
        subMenu -> Insert( std::move(subSubMenu));

        rootMenu -> Insert( std::move(subMenu) );


        Cli cli( std::move(rootMenu) );
        // global exit action
        cli.ExitAction( [](auto& out){ out << "Goodbye and thanks for all the fish.\n"; } );

        MainScheduler scheduler;
        CliAsyncSession session(scheduler, cli);
        session.ExitAction(
            [&scheduler](auto& out) // session exit action
            {
                out << "Closing App...\n";
                scheduler.Stop();
            }
        );    
        scheduler.Run();

        return 0;
    }
    catch (const std::exception& e)
    {
        cerr << "Exception caugth in main: " << e.what() << '\n';
    }
    catch (...)
    {
        cerr << "Unknown exception caugth in main.\n";
    }
    return -1;
}
