#include <fleropp/HTMLStream.hpp>
#include <fleropp/HTMLLiterals.hpp>
#include <cgicc/HTMLClasses.h>
#include <string>

#include "JavascriptExample.hpp"

using namespace fleropp::literals;
using namespace cgicc;
using namespace std;
namespace htmls = fleropp::html_stream;

// link to reference: https://www.w3schools.com/html/tryit.asp?filename=tryhtml_head_script
// Note: Inline javascript seems to only work when it is not inside of a header.

INIT_VIEW(JavascriptExample)

namespace js_example_util {
    // Generates the Javascript logic that runs this page.
    void generateJS(htmls::HTMLStream<"<script type=\"text/javascript\">", "</script>"> &script) {
        script << "function myFunction() { document.getElementById(\"demo\").innerHTML = \"Hello JavaScript!\";} \n" << htmls::gen_end_tag();
    }
}

void JavascriptExample::get(const fleropp::io::RequestData& request) {
    using namespace js_example_util;
    htmls::HTMLStream<"<html>", "</html>", htmls::dump_on_end> html;
    htmls::HTMLStream<"<body>", "</body>"> body;
    htmls::HTMLStream<"<head>", "</head>"> head;

    htmls::HTMLStream<"<script type=\"text/javascript\">", "</script>"> script;
    generateJS(script);

    body << h1("My Web Page") 
        << p("A Paragraph").set("id","demo") 
        << button("Try it").set("type","button").set("onclick","myFunction()") << htmls::gen_end_tag();
    head << script << htmls::gen_end_tag();
    html << head << body << htmls::gen_end_tag();
}