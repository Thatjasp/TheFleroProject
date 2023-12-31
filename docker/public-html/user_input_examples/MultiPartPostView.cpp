#include <cstdlib>
#include <iterator>

#include "MultiPartPostView.hpp"

#include <fleropp/HTMLLiterals.hpp>
#include <fleropp/QueryString.hpp>
#include <fleropp/RequestData.hpp>
#include <fleropp/PostFile.hpp>

INIT_VIEW(MultiPartPostView)

void MultiPartPostView::get(const fleropp::io::RequestData& request) {
    using namespace fleropp::literals;
    "Content-type: text/html\r"_h;
    "\r"_h;
    "<html>"_h;
        "<head>"_h "<title>Hello!, World</title>"_h "</head>"_h;
        "<body>"_h;
            "<h1>Hello, World 2!</h1>"_h;
            "<h1>Hello, World&mdash;{}</h1>"_f("from me, in a template");
            "<h1>User-Agent: {}</h1>"_f(request.get_header("User-Agent"));
            "<h1>Request type: {}</h1>"_f(request.method());
            "<h1>Random number: {}</h1>"_f(rand());
            "<form action='multipart.elf' method='post' enctype='multipart/form-data' target='out_iframe'>"_h;
                "Name: <input type='text' name='person'><br>"_h;
                "File: <input type='file' name='secret'><br>"_h;
                "<input type='submit'>"_h;
            "</form>"_h;
            "<iframe name='out_iframe'></iframe>"_h;
        "</body>"_h;
    "</html>"_h;
}

void MultiPartPostView::post(const fleropp::io::RequestData& request) {
    using namespace fleropp::literals;
    std::vector<PostFile> dataVec = request.get_post_files();
    auto form1 = dataVec[0].get_data();
    auto form2 = dataVec[1].get_data();
    std::string result(form1.begin(),form1.end());
    std::string txtFileResult(form2.begin(),form2.end());
    "Content-type: text/html\r"_h;
    "\r"_h;
    "<h1>Number of things in the Post Data form: {}</h1>"_f(dataVec.size());
    "<h1>The {}'s name is: {}</h1>"_f(dataVec[0].get_name(),result);
    "<h1>The {}'s content is: {}</h1>"_f(dataVec[1].get_file_name(),txtFileResult);
    "<h1>Request type: {}</h1>"_f(request.method());
}