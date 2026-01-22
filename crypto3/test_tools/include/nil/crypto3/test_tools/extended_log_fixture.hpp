//---------------------------------------------------------------------------//
// Copyright (c) 2026 Elena Tatuzova <elena@allocinit.xyz>
//
// MIT License
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//---------------------------------------------------------------------------//
#pragma once

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/utility/formatting_ostream.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/attributes/value_extraction.hpp>
#include <boost/log/utility/setup/console.hpp>


// Log-related classes
void file_formatter(boost::log::record_view const& rec, boost::log::formatting_ostream& strm){
    // Finally, put the record message to the stream
    if( rec[boost::log::trivial::severity] > boost::log::trivial::info) {
        strm  << "[" << rec[boost::log::trivial::severity] << "] " << rec[boost::log::expressions::smessage];
    } else {
        strm  << rec[boost::log::expressions::smessage];
    }
}


void colored_formatter(boost::log::record_view const& rec, boost::log::formatting_ostream& strm){
    // Colored output looks nice in terminal, but not in files.
    // Use --color-log to enable colored output in terminal.
    if( rec[boost::log::trivial::severity] == boost::log::trivial::fatal) {
        strm  << "[\x1B[91m" << rec[boost::log::trivial::severity] << "\x1B[0m] " << rec[boost::log::expressions::smessage];
    } else if( rec[boost::log::trivial::severity] == boost::log::trivial::error) {
        strm  << "[\x1B[38;2;255;165;0m" << rec[boost::log::trivial::severity] << "\x1B[0m] " << rec[boost::log::expressions::smessage];
    } else if( rec[boost::log::trivial::severity] == boost::log::trivial::warning) {
        strm  << "[\x1B[33m" << rec[boost::log::trivial::severity] << "\x1B[0m] " << rec[boost::log::expressions::smessage];
    } else if( rec[boost::log::trivial::severity] == boost::log::trivial::info) {
        strm  << "[\x1B[32m" << rec[boost::log::trivial::severity] << "\x1B[0m] " << rec[boost::log::expressions::smessage];
    } else {
        strm  << rec[boost::log::expressions::smessage];
    }
}


class ExtendedLogFixture {
public:
    ExtendedLogFixture() {
        // Initialize the logging system
        boost::log::trivial::severity_level log_level = boost::log::trivial::info;
        bool is_color = false;

        std::size_t argc = boost::unit_test::framework::master_test_suite().argc;
        auto &argv = boost::unit_test::framework::master_test_suite().argv;
        for( std::size_t i = 0; i < argc; i++ ){
            std::string arg(argv[i]);
            if( arg == "--log-level=trace"){
                log_level = boost::log::trivial::trace;
            }
            if( arg == "--log-level=debug"){
                log_level = boost::log::trivial::debug;
            }
            if( arg == "--log-level=info"){
                log_level = boost::log::trivial::info;
            }
            if( arg == "--log-level=warning"){
                log_level = boost::log::trivial::warning;
            }
            if( arg == "--log-level=error"){
                log_level = boost::log::trivial::error;
            }
            if( arg == "--no-log" ){
                log_level = boost::log::trivial::fatal;
            }
            if( arg == "--color-log" ){
                is_color = true;
            }
        }

        typedef boost::log::sinks::synchronous_sink< boost::log::sinks::text_ostream_backend > text_sink;
        boost::shared_ptr< text_sink > sink = boost::make_shared< text_sink >();

        sink->locked_backend()->add_stream(boost::shared_ptr< std::ostream >(&std::cout, boost::null_deleter()));
        if (is_color) {
            sink->set_formatter(&colored_formatter);
        } else {
            sink->set_formatter(&file_formatter);
        }
        sink->locked_backend()->auto_flush(true);
        boost::log::core::get()->add_sink(sink);

        sink->set_filter(
            boost::log::trivial::severity >= log_level
        );
    }
};