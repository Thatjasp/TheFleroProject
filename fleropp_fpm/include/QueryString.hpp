#ifndef QUERY_STRING_HPP
#define QUERY_STRING_HPP

#include <regex>
#include <string>
#include <unordered_map>

// fleropp IO namespace
namespace fleropp::io {
  /**
   * \namespace fleropp::io
   */

  /**
   * \brief Simple pattern that matches a query string.
   *
   * This will match query strings of the standard
   */
  static constexpr auto query_pattern = R"(([\w+%]+)=([^&]*))";

  /**
   * \brief The regular expression used to match a query string.
   *
   * Query strings should be of the form:
   *    key_1=value_1&key_2=value_2&...&key_n=value_n
   *
   * See RFC-3986 for more information.
   */
  static inline const std::regex query_regexp{query_pattern};

  /**
   * \class QueryString
   * \brief A class that represents a query string
   */
  class QueryString {
  public:
    /**
     * \brief Default QueryString Constructor
     */
    QueryString() = default;

    /**
     * \brief QueryString Constructor which takes the contents of a query string
     *
     * \param[in] contents The contents of a query string (exluding the leading '?').
     */
    QueryString(const std::string &contents);

    /**
     * \brief Parses the query string into a key-value pair format
     */
    void parse();

    /**
     * \brief Retrieves the value associated with `key`, or the empty string if no
     * corresponding value exists.
     *
     * \param[in] key The key to be searched
     * \return The value associated with `key`
     */
    std::string get(const std::string &key) const;

    /**
     * \brief Subscript operator. Retrieves a reference to the value associated with
     * `key`, or throws an exception if no corresponding value exists.
     * \param[in] key The key to be searched.
     * \return The value associated with `key`
     */
    const std::string &operator[](const std::string &key) const;

  private:
    std::string m_contents;
    std::unordered_map<std::string, std::string> m_qstring_map;

    static std::string url_decode(const std::string &url);
  };
}

#endif /* QUERY_STRING_HPP */