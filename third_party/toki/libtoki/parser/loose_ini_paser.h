/*
    Copyright (C) 2010 Tomasz Śniatowski, Adam Radziszewski
    Part of the libtoki project

    This program is free software; you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License as published by the Free
Software Foundation; either version 3 of the License, or (at your option)
any later version.

    This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. 

    See the LICENSE, COPYING.LESSER and COPYING files for more details.
*/

// Based on boost property_tree ini_parser, original copyright notice follows.
// ----------------------------------------------------------------------------
// Copyright (C) 2002-2006 Marcin Kalicinski
// Copyright (C) 2009 Sebastian Redl
//
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see www.boost.org
// ----------------------------------------------------------------------------
#ifndef LIBTOKI_LOOSE_INI_PARSER_H
#define LIBTOKI_LOOSE_INI_PARSER_H

#include <boost/property_tree/ini_parser.hpp>

namespace loose_ini_parser
{
	namespace property_tree = boost::property_tree;
	using boost::property_tree::ini_parser_error;
	using boost::throw_exception;
	using boost::property_tree::ini_parser::validate_flags;

    /**
     * Read INI from a the given stream and translate it to a property tree.
     * @note Clears existing contents of property tree. In case of error
     *       the property tree is not modified.
     * @throw ini_parser_error If a format violation is found.
     * @param stream Stream from which to read in the property tree.
     * @param[out] pt The property tree to populate.
     */
    template<class Ptree>
	void read_loose_ini(std::basic_istream<
                    typename Ptree::key_type::value_type> &stream,
                  Ptree &pt)
    {
        typedef typename Ptree::key_type::value_type Ch;
        typedef std::basic_string<Ch> Str;
        const Ch semicolon = stream.widen(';');
        const Ch lbracket = stream.widen('[');
        const Ch rbracket = stream.widen(']');

        Ptree local;
        unsigned long line_no = 0;
        Ptree *section = 0;
        Str line;

        // For all lines
        while (stream.good())
        {

            // Get line from stream
            ++line_no;
            std::getline(stream, line);
            if (!stream.good() && !stream.eof())
                BOOST_PROPERTY_TREE_THROW(ini_parser_error(
                    "read error", "", line_no));

            // If line is non-empty
            line = property_tree::detail::trim(line, stream.getloc());
            if (!line.empty())
            {
                // Comment, section or key?
                if (line[0] == semicolon)
                {
                    // Ignore comments
                }
                else if (line[0] == lbracket)
                {
                    typename Str::size_type end = line.find(rbracket);
                    if (end == Str::npos)
                        BOOST_PROPERTY_TREE_THROW(ini_parser_error(
                            "unmatched '['", "", line_no));
                    Str key = property_tree::detail::trim(
                        line.substr(1, end - 1), stream.getloc());
                    section = &local.push_back(
                        std::make_pair(key, Ptree()))->second;
                }
                else
                {
                    Ptree &container = section ? *section : local;
                    typename Str::size_type eqpos = line.find(Ch('='));
                    if (eqpos == Str::npos)
                        BOOST_PROPERTY_TREE_THROW(ini_parser_error(
                            "'=' character not found in line", "", line_no));
                    if (eqpos == 0)
                        BOOST_PROPERTY_TREE_THROW(ini_parser_error(
                            "key expected", "", line_no));
                    Str key = property_tree::detail::trim(
                        line.substr(0, eqpos), stream.getloc());
                    Str data = property_tree::detail::trim(
                        line.substr(eqpos + 1, Str::npos), stream.getloc());
                    container.push_back(std::make_pair(key, Ptree(data)));
                }
            }
        }

        // Swap local ptree with result ptree
        pt.swap(local);

    }

    /**
     * Read INI from a the given file and translate it to a property tree.
     * @note Clears existing contents of property tree.  In case of error the
     *       property tree unmodified.
     * @throw ini_parser_error In case of error deserializing the property tree.
     * @param filename Name of file from which to read in the property tree.
     * @param[out] pt The property tree to populate.
     * @param loc The locale to use when reading in the file contents.
     */
    template<class Ptree>
	void read_loose_ini(const std::string &filename,
                  Ptree &pt,
                  const std::locale &loc = std::locale())
    {
        std::basic_ifstream<typename Ptree::key_type::value_type>
            stream(filename.c_str());
        if (!stream)
            BOOST_PROPERTY_TREE_THROW(ini_parser_error(
                "cannot open file", filename, 0));
        stream.imbue(loc);
        try {
			read_loose_ini(stream, pt);
        }
        catch (ini_parser_error &e) {
            BOOST_PROPERTY_TREE_THROW(ini_parser_error(
                e.message(), filename, e.line()));
        }
    }

    namespace detail
    {
        template<class Ptree>
        void check_dupes(const Ptree &pt)
        {
            if(pt.size() <= 1)
                return;
            const typename Ptree::key_type *lastkey = 0;
            typename Ptree::const_assoc_iterator it = pt.ordered_begin(),
                                                 end = pt.not_found();
            lastkey = &it->first;
            for(++it; it != end; ++it) {
                if(*lastkey == it->first)
                    BOOST_PROPERTY_TREE_THROW(ini_parser_error(
                        "duplicate key", "", 0));
                lastkey = &it->first;
            }
        }
    }

    /**
     * Translates the property tree to INI and writes it the given output
     * stream.
     * @pre @e pt cannot have data in its root.
     * @pre @e pt cannot have keys both data and children.
     * @pre @e pt cannot be deeper than two levels.
     * @pre There cannot be duplicate keys on any given level of @e pt.
     * @throw ini_parser_error In case of error translating the property tree to
     *                         INI or writing to the output stream.
     * @param stream The stream to which to write the INI representation of the 
     *               property tree.
     * @param pt The property tree to tranlsate to INI and output.
     * @param flags The flags to use when writing the INI file.
     *              No flags are currently supported.
     */
    template<class Ptree>
	void write_loose_ini(std::basic_ostream<
                       typename Ptree::key_type::value_type
                   > &stream,
                   const Ptree &pt,
                   int flags = 0)
    {
        using detail::check_dupes;

        typedef typename Ptree::key_type::value_type Ch;

        BOOST_ASSERT(validate_flags(flags));
        (void)flags;

        if (!pt.data().empty())
            BOOST_PROPERTY_TREE_THROW(ini_parser_error(
                "ptree has data on root", "", 0));
        check_dupes(pt);

        for (typename Ptree::const_iterator it = pt.begin(), end = pt.end();
             it != end; ++it)
        {
            if (it->second.empty()) {
                stream << it->first << Ch('=')
                    << it->second.template get_value<
                        std::basic_string<Ch> >()
                    << Ch('\n');
            } else {
                if (!it->second.data().empty())
                    BOOST_PROPERTY_TREE_THROW(ini_parser_error(
                        "mixed data and children", "", 0));
                stream << Ch('[') << it->first << Ch(']') << Ch('\n');
                for (typename Ptree::const_iterator it2 = it->second.begin(),
                         end2 = it->second.end(); it2 != end2; ++it2)
                {
                    if (!it2->second.empty())
                        BOOST_PROPERTY_TREE_THROW(ini_parser_error(
                            "ptree is too deep", "", 0));
                    stream << it2->first << Ch('=')
                        << it2->second.template get_value<
                            std::basic_string<Ch> >()
                        << Ch('\n');
                }
            }
        }

    }

    /**
     * Translates the property tree to INI and writes it the given file.
     * @pre @e pt cannot have data in its root.
     * @pre @e pt cannot have keys both data and children.
     * @pre @e pt cannot be deeper than two levels.
     * @pre There cannot be duplicate keys on any given level of @e pt.
     * @throw info_parser_error In case of error translating the property tree
     *                          to INI or writing to the file.
     * @param filename The name of the file to which to write the INI
     *                 representation of the property tree.
     * @param pt The property tree to tranlsate to INI and output.
     * @param flags The flags to use when writing the INI file.
     *              The following flags are supported:
     * @li @c skip_ini_validity_check -- Skip check if ptree is a valid ini. The
     *     validity check covers the preconditions but takes <tt>O(n log n)</tt>
     *     time.
     * @param loc The locale to use when writing the file.
     */
    template<class Ptree>
	void write_loose_ini(const std::string &filename,
                   const Ptree &pt,
                   int flags = 0,
                   const std::locale &loc = std::locale())
    {
        std::basic_ofstream<typename Ptree::key_type::value_type>
            stream(filename.c_str());
        if (!stream)
            BOOST_PROPERTY_TREE_THROW(ini_parser_error(
                "cannot open file", filename, 0));
        stream.imbue(loc);
        try {
			write_loose_ini(stream, pt, flags);
        }
        catch (ini_parser_error &e) {
            BOOST_PROPERTY_TREE_THROW(ini_parser_error(
                e.message(), filename, e.line()));
        }
    }

}

#endif
