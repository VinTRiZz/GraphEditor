#include "mainwindow.h"
#include <QApplication>

#include <Common/Logging.h>

#include <iostream> // For boost::program_options
#include <boost/program_options.hpp>

int main(int argc, char *argv[])
{
    boost::program_options::options_description descr;
    descr.add_options()
            ("help", "Print this info and exit")
    ;

    boost::program_options::variables_map vm;
    try {
        auto parsedOptions = boost::program_options::parse_command_line(argc, argv, descr);
        boost::program_options::store(parsedOptions, vm);
        vm.notify();

    } catch (const boost::program_options::error& ex) {
        LOG_ERROR_SYNC(ex.what());
        return 1;
    }

    if (vm.count("help")) {
        descr.print(std::cout);
        return 0;
    }

    LOG_INFO_SYNC("Started GraphEditor");
    QApplication a(argc, argv);
    MainWindow w;

    w.show();
    return a.exec();
}
