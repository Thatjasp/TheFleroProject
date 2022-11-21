#include <cgicc/HTMLClasses.h>
#include <fleropp/HTMLStream.hpp>
#include <fleropp/HTMLLiterals.hpp>
#include <ranges>

#include "IframeExampleView.hpp"

// link to reference: https://www.w3schools.com/html/tryit.asp?filename=tryhtml_table_basic

extern "C" {
    IframeExampleView *allocator() {
        return new IframeExampleView();
    }

    void deleter(IframeExampleView *ptr) {
        delete ptr;
    }
}

void IframeExampleView::generate() {
    using namespace fleropp_literals;
    namespace htmls = fleropp_html_stream;

    htmls::gen_html_boiler_plate("text/html");
    "<html>"_h;
        "<body>"_h;
            "<iframe src='https://projects.baxterhuntington.xyz/modulon' width=600 height=600/>"_h;
        "</body>"_h;
    "</html>"_h;
}