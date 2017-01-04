/*
 * Copyright 2013 Pedro Larroy Tovar
 *
 * This file is subject to the terms and conditions
 * defined in file 'LICENSE.txt', which is part of this source
 * code package.
 */


/**
 * @addtogroup url 
 * @brief Handling of RFC 3986 urls
 *
 * @{
 */

/**
 * @class Url Url.hh
 * @brief handles URLs
 *
 * Utility to break them in its constituent parts according to RFC3986
 *
 * \verbatim
 * See RFC 3986.
 * http://user:password@host:port/path
 * foo://example.com:8042/over/there?name=ferret#nose
 * \_/   \______________/\_________/ \_________/ \__/
 *  |           |            |           |        |
 * scheme     authority     path      query     fragment
 *        \_______________________/
 *                  |
 *              hier_part
 *
 * Most important ABNF rules:
 *
 * authority     = [ userinfo "@" ] host [ ":" port ]
 * URI = scheme ":" hier-part [ "?" query ] [ "#" fragment ]
 *
 * \endverbatim
 *
 */

#pragma once

#include <string>
#include <stdexcept>
#include <boost/regex.hpp>

#include "Path.h"

namespace url
{

namespace utils
{

/**
 * Validating regexps
 */
/*
      IPv6address =                            6( h16 ":" ) ls32
                  /                       "::" 5( h16 ":" ) ls32
                  / [               h16 ] "::" 4( h16 ":" ) ls32
                  / [ *1( h16 ":" ) h16 ] "::" 3( h16 ":" ) ls32
                  / [ *2( h16 ":" ) h16 ] "::" 2( h16 ":" ) ls32
                  / [ *3( h16 ":" ) h16 ] "::"    h16 ":"   ls32
                  / [ *4( h16 ":" ) h16 ] "::"              ls32
                  / [ *5( h16 ":" ) h16 ] "::"              h16
                  / [ *6( h16 ":" ) h16 ] "::"

      ls32        = ( h16 ":" h16 ) / IPv4address
                  ; least-significant 32 bits of address

      h16         = 1*4HEXDIG
                  ; 16 bits of address represented in hexadecimal
      IPv4address = dec-octet "." dec-octet "." dec-octet "." dec-octet
      dec-octet   = DIGIT                 ; 0-9
                  / %x31-39 DIGIT         ; 10-99
                  / "1" 2DIGIT            ; 100-199
                  / "2" %x30-34 DIGIT     ; 200-249
                  / "25" %x30-35          ; 250-255

*/

// Although the RFC says ipv6 octects like 001 are not valid, it would be risky
// not to accept those
#define decoct "([01]?[0-9]?[0-9]|2[0-4][0-0]|25[0-5])"
#define ipv4 "(" decoct "\\." decoct "\\." decoct "\\." decoct ")"
#define h16 "([[:xdigit:]]{1,4})"
#define ls32 "(([[:xdigit:]]{1,4}:[[:xdigit:]]{1,4})|" ipv4 ")"
#define ipv6 "("                    "((" h16 ":){6}" ls32 ")"\
        "|("                       "::(" h16 ":){5}" ls32 ")"\
        "|(("                h16 ")?::(" h16 ":){4}" ls32 ")"\
        "|(((" h16 ":){0,1}" h16 ")?::(" h16 ":){3}" ls32 ")"\
        "|(((" h16 ":){0,2}" h16 ")?::(" h16 ":){2}" ls32 ")"\
        "|(((" h16 ":){0,3}" h16 ")?::(" h16 ":){1}" ls32 ")"\
        "|(((" h16 ":){0,4}" h16 ")?::" ls32 ")"\
        "|(((" h16 ":){0,5}" h16 ")?::" h16 ")"\
        "|(((" h16 ":){0,6}" h16 ")?::)"\
        ")"

/**
 * Url parsing regular expressions
 */
enum {
    RE_URL=0,
    RE_SCHEME,
    RE_HOST,
    RE_PORT,
    RE_IPVFUT,
    RE_IPV6,
    RE_IPV4
};

static const char* const url_regex[] = {
    /* RE_URL */
    "^(([^:/?#]+):)?(//([^/?#]*)|///)?([^?#]*)(\\?[^#]*)?(#.*)?",
    /* RE_SCHEME */
    "^[[:alpha:]][[:alnum:]+.-]*$",
    /* RE_HOST */
    "^[[:alnum:]]([[:alnum:].-]*[[:alnum:].])?$",
    /* RE_PORT */
    "^\\d{1,5}$",
    /* RE_IPVFUT*/
    "^v[[:xdigit:]]\\.1[[:alnum:]._~!$&'()*+,;=:-]*$",
    /* RE_IPV6 */
    ipv6,
    /* RE_IPV4 */
    ipv4
};

#undef ipv6
#undef ls32
#undef h16
#undef ipv4
#undef decoct

/**
 * Url escaping
 */
enum {
    URL_CHAR_RESERVED    = 0x1,
    URL_CHAR_UNSAFE        = 0x2,
    URL_CHAR_ESC        = 0x4,
    URL_CHAR_AUTH        = 0x8,
    URL_CHAR_PATH        = 0x10,
    URL_CHAR_QUERY        = 0x20,
    URL_CHAR_FRAGMENT    = 0x40
};

#define R URL_CHAR_RESERVED
#define U URL_CHAR_UNSAFE
#define E URL_CHAR_ESC
#define A URL_CHAR_AUTH
#define P URL_CHAR_PATH
#define Q URL_CHAR_QUERY
#define F URL_CHAR_FRAGMENT
#define RU R|U
#define X U|E|A|P|Q|F
/*
 * For authority we have to escape / ? #
 * For path: ?#
 * For query: #
 */
static const unsigned char url_char_table[256] =
{
  X,  X,  X,  X,   X,  X,  X,  X,     /* NUL SOH STX ETX  EOT ENQ ACK BEL */
  X,  X,  X,  X,   X,  X,  X,  X,     /* BS  HT  LF  VT   FF  CR  SO  SI  */
  X,  X,  X,  X,   X,  X,  X,  X,     /* DLE DC1 DC2 DC3  DC4 NAK SYN ETB */
  X,  X,  X,  X,   X,  X,  X,  X,     /* CAN EM  SUB ESC  FS  GS  RS  US  */
  X,  0,  X, RU|A|P|Q, R, X, R, 0,    /* SP  !   "   #    $   %   &   '   */
  0,  0,  0,  R,   R,  0,  0,  R|A,   /* (   )   *   +    ,   -   .   /   */
  0,  0,  0,  0,   0,  0,  0,  0,     /* 0   1   2   3    4   5   6   7   */
  0,  0, RU,  R,   X,  R,  X,  R|A|P, /* 8   9   :   ;    <   =   >   ?   */
 RU,  0,  0,  0,   0,  0,  0,  0,     /* @   A   B   C    D   E   F   G   */
  0,  0,  0,  0,   0,  0,  0,  0,     /* H   I   J   K    L   M   N   O   */
  0,  0,  0,  0,   0,  0,  0,  0,     /* P   Q   R   S    T   U   V   W   */
  0,  0,  0, RU,   X, RU,  X,  0,     /* X   Y   Z   [    \   ]   ^   _   */
  X,  0,  0,  0,   0,  0,  0,  0,     /* `   a   b   c    d   e   f   g   */
  0,  0,  0,  0,   0,  0,  0,  0,     /* h   i   j   k    l   m   n   o   */
  0,  0,  0,  0,   0,  0,  0,  0,     /* p   q   r   s    t   u   v   w   */
  0,  0,  0,  X,   X,  X,  0,  X,     /* x   y   z   {    |   }   ~   DEL */

  X, X, X, X,  X, X, X, X,  X, X, X, X,  X, X, X, X,
  X, X, X, X,  X, X, X, X,  X, X, X, X,  X, X, X, X,
  X, X, X, X,  X, X, X, X,  X, X, X, X,  X, X, X, X,
  X, X, X, X,  X, X, X, X,  X, X, X, X,  X, X, X, X,

  X, X, X, X,  X, X, X, X,  X, X, X, X,  X, X, X, X,
  X, X, X, X,  X, X, X, X,  X, X, X, X,  X, X, X, X,
  X, X, X, X,  X, X, X, X,  X, X, X, X,  X, X, X, X,
  X, X, X, X,  X, X, X, X,  X, X, X, X,  X, X, X, X,
};
#undef R
#undef U
#undef E
#undef A
#undef P
#undef Q
#undef F
#undef RU
#undef X

#define url_char_test(c, mask) (url_char_table[(unsigned char)(c)] & (mask))

} // end namespace

/**
 * @brief Exception thrown if the url doesn't look to be valid
 */
class BadUrl: public std::runtime_error {
    public:
        BadUrl(const std::string& reason): std::runtime_error(reason) { }
        BadUrl(): std::runtime_error("unspecified") { }
};

/**
 * @brief Exception thrown in case url doesn't parse correctly
 */
class UrlParseError: public BadUrl {
public:
    UrlParseError(const std::string& reason) : BadUrl(reason) { }
    UrlParseError(): BadUrl() { }
};



class Url {
public:
    /// @throw UrlParseError on url parsing error
    explicit Url(const std::string& s);
    explicit Url();

    /// @throw UrlParseError on url parsing error
    void assign(const std::string& s);

    /**
     * @brief Merge with a relative reference
     * transform a relative referecence u into the target url see [RFC3986 Page 34]
     */
    Url& merge_ref(const Url& u);

    /*** OPERATORS ***/
    Url& operator=(const std::string& s);

    /**
     * This is used for merging two relative URIs or one absolute URI and a relative one. Since merging with an absolute URI just overwrites everything.
     * Urls with authority have absolute paths
     * @throws BadUrl
     */
    Url& operator+=(const Url& u);
    bool operator==(const Url& u);
    bool operator!=(const Url& u);

    // Safe bool idiom, operator bool causes unintended conversions to char, so we can use operator std::string
    // http://www.artima.com/cppsource/safebool2.html
    typedef void (Url::*safe_bool)() const;

    operator safe_bool() const
    {
        //std::cout << "safe bool" << std::endl;
        if(empty())
            return 0;
        else
            return &Url::safe_bool_aux;
    }
    void safe_bool_aux() const {}

    operator std::string()
    {
        return get();
    }

    friend std::ostream& operator<<(std::ostream& os, const Url& u);
    /**
     * scheme and host are case insensitive
     */
    void normalize_scheme() ;
    void normalize_host() ;
    /**
     * normalize pct-encoded escapes to uppercase as mandated by rfc
     */
    void normalize_escapes();
    static std::string normalize_escapes(const std::string&) ;
    /**
     * does all normalizations
     */
    void normalize();

    void clear()
    {
        m_scheme.clear();
        clear_authority();

        m_path.clear();

        clear_query();
        clear_fragment();
    }


    bool empty() const;


    /**
     * Get the whole url as std::string
     */

    /**
     * Get the whole url as std::string
     */
    std::string get() const;
    std::string to_string() const { return get(); };
    std::string as_string() const { return get(); };

    /**
     * Size of the url when represented as an string
     */
    size_t size() const;

    static std::string escape_reserved_unsafe(const std::string& s);

    /**
     * escapes characters using the supplied mask
     */
    static std::string escape(const std::string& s, const unsigned char mask);

    /**
     * Unescapes characters that match the char table given the mask
     */
    static std::string unescape(const std::string& s, const unsigned char mask);

    /**
     * Unescapes those that don't match the mask
     */
    static std::string unescape_not(const std::string& s, const unsigned char mask);

    /**
     * Unescape those which are safe
     */
    static std::string unescape_safe(const std::string& s);

    /**
     * Unescape everything pct encoded
     */
    static std::string unescape(const std::string& s);

    /**
     * The rfc says: "If a URI does not contain an authority component, then the path cannot begin with two slash characters ("//")"
     * But then, there are the file:// URIs that can be file:///tmp/ or similar, so this apparently violates the rfc.
     * @throws boost::regex_error
     */
    bool syntax_ok() const ;
    bool valid_host() const;
    static bool valid_host(const std::string&);

    /***** ACCESSORS *****/
    void scheme(const std::string& s);
    std::string scheme() const
    {
        return m_scheme;
    }

    void authority(const std::string& s);
    std::string authority() const ;

    /**
     * authority     = [ userinfo "@" ] host [ ":" port ]
     */
    bool has_authority() const
    {
        return m_has_authority;
    }

    bool has_scheme() const
    {
        return (! m_scheme.empty());
    }

    void clear_authority()
    {
        m_host_ip_literal=false;
        m_has_authority = false;
        m_userinfo.clear();
        m_host.clear();
        m_port.clear();
    }

    bool has_query() const
    {
        return ! m_query.empty();
    }

    void clear_query()
    {
        m_query.clear();
    }

    bool has_fragment() const
    { 
        return ! m_fragment.empty();
    }

    void clear_fragment()
    {
        m_fragment.clear();
    }

    /**
     * absolute-URI  = scheme ":" hier-part [ "?" query ]
     * hier-part     = "//" authority path-abempty
     *                    / path-absolute
     *                    / path-rootless
     *                    / path-empty
     */
    bool absolute() const ;

    void userinfo(const std::string& s);
    std::string userinfo() const
    {
        return m_userinfo;
    }

    void host(const std::string& s);
    std::string host() const
    {
        return m_host;
    }

    void port(const std::string& s);
    /// @returns port
    std::string port() const;
    int port_int() const;

    void path(const std::string& s);
    /// @returns the path
    std::string path() const
    {
        return m_path.get();
    }

    /// normalize path, removing /./ and /../ segments
    void normalize_path()  { m_path.normalize(); };

    void query(const std::string& s);
    /// @return the query part
    std::string query() const  { return m_query; }

    void fragment(const std::string& s);
    /// @return the fragment part
    std::string fragment() const  { return m_fragment; }

    void set_def_port();

    Path        m_path;
    std::string m_scheme;
    bool m_has_authority;
    bool m_host_ip_literal;
    std::string m_userinfo;
    std::string m_host;
    std::string m_port;
    std::string m_query;
    std::string m_fragment;
};


} // end ns url

/** @} */

