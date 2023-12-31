#ifndef I_VIEW_HPP
#define I_VIEW_HPP

#include <ostream>
#include <string_view>

#ifdef FPM_BUILD
#include "ConstexprMap.hpp"
#include "FleroppIO.hpp"
#include "RequestData.hpp"
#include "util.hpp"
#else
#include <fleropp/ConstexprMap.hpp>
#include <fleropp/FleroppIO.hpp>
#include <fleropp/RequestData.hpp>
#include <fleropp/util.hpp>
#endif

#include <fmt/format.h>

#define INIT_VIEW(NAME)                       \
extern "C" {                                  \
    NAME* allocator() {                       \
        return new NAME();                    \
    }                                         \
    void deleter(NAME* ptr) {                 \
        delete ptr;                           \
    }                                         \
}                                             \

/**
 * \brief Wrapper for storing a heterogeneous collection of \code IView \endcode.
 * 
 * This abstract wrapper class serves as a base class for IView, allowing them to
 * be stored easily in a container. 
 */
struct IViewWrapper {
    virtual ~IViewWrapper() = default;
    virtual void get_dispatch(const fleropp::io::RequestData& request) = 0;
    virtual void post_dispatch(const fleropp::io::RequestData& request) = 0;
    virtual void put_dispatch(const fleropp::io::RequestData& request) = 0;
    virtual void del_dispatch(const fleropp::io::RequestData& request) = 0;
    virtual void get(const fleropp::io::RequestData& request) = 0;
    virtual void post(const fleropp::io::RequestData& request) = 0;
    virtual void put(const fleropp::io::RequestData& request) = 0;
    virtual void del(const fleropp::io::RequestData& request) = 0;
};

/**
 * \class IView
 * \brief Derived class from IViewWrapper
*/
template <class DerivedView>
class IView : public IViewWrapper {    
  public:
    /**
     * \brief Default IView destructor
    */
    virtual ~IView() = default;
    
    /**
     * \brief Function that handles GET requests
     * \param[in] request RequestData object containing GET request information
    */
    void get_dispatch(const fleropp::io::RequestData& request) final {
        static_cast<DerivedView*>(this)->get(request);
    }

    /**
     * \brief Function that handles POST requests
     * \param[in] request RequestData object containing POST request information
    */
    void post_dispatch(const fleropp::io::RequestData& request) final {
        static_cast<DerivedView*>(this)->post(request);
    }

    /**
     * \brief Function that handles PUT requests
     * \param[in] request RequestData object containing PUT request information
    */
    void put_dispatch(const fleropp::io::RequestData& request) final {
        static_cast<DerivedView*>(this)->put(request);
    }

    /**
     * \brief Function that handles DELETE requests
     * \param[in] request RequestData object containing DELETE request information
    */
    void del_dispatch(const fleropp::io::RequestData& request) final {
        static_cast<DerivedView*>(this)->del(request);
    }

    /**
     * \brief Default implementation of method for handling GET requests
     * \param[in] request RequestData object containing GET request information
    */
    void get(const fleropp::io::RequestData& request) { print_default_response(); }

    /**
     * \brief Default implementation of method that handles POST requests
     * \param[in] request RequestData object containing POST request information
    */
    void post(const fleropp::io::RequestData& request) { print_default_response(); }

    /**
     * \brief Default implementation of method that handles PUT requests
     * \param[in] request RequestData object containing PUT request information
    */
    void put(const fleropp::io::RequestData& request) { print_default_response(); }

    /**
     * \brief Default implementation of method that handles DELETE requests
     * \param[in] request RequestData object containing DELETE request information
    */
    void del(const fleropp::io::RequestData& request) { print_default_response(); }
  private:
    struct constants {
        using member_arg_t = const fleropp::io::RequestData&;
        using view_member_map_t = fleropp::util::member_map_t<IView, DerivedView, 4, member_arg_t>;

        static constexpr view_member_map_t members{{{&DerivedView::get, "GET"}, 
                                                    {&DerivedView::post, "POST"},
                                                    {&DerivedView::put, "PUT"},
                                                    {&DerivedView::del, "DELETE"}}};
        static constexpr std::string_view default_response = 
            "Status: 405 Method Not Allowed\r\n"
            "Content-type: text/html\r\n"
            "Allow: {}\r\n"
            "Content-length: 31\r\n\r\n"
            "<h1>405 Method Not Allowed</h1>";
        static constexpr auto n_supported = 
            fleropp::util::count_overridden<IView, DerivedView, members.size(), member_arg_t>(members);
        static constexpr auto supported = 
            fleropp::util::get_overridden<IView, DerivedView, members.size(), n_supported, member_arg_t>(members); 
    };

    // "The origin server MUST generate an allow header field in a 405
    //  response containing a list of the target resource's currently 
    //  supported methods."
    //  --RFC7231, Section 6.5.5
    static void print_default_response() {
        fleropp::io::fppout << fmt::format(constants::default_response, 
                                          fmt::join(constants::supported, ", "));
    }
};

#endif /* I_VIEW_HPP */