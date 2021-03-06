#ifndef OSMIUM_INDEX_MAP_SPARSE_MEM_MAP_HPP
#define OSMIUM_INDEX_MAP_SPARSE_MEM_MAP_HPP

/*

This file is part of Osmium (http://osmcode.org/libosmium).

Copyright 2013-2015 Jochen Topf <jochen@topf.org> and others (see README).

Boost Software License - Version 1.0 - August 17th, 2003

Permission is hereby granted, free of charge, to any person or organization
obtaining a copy of the software and accompanying documentation covered by
this license (the "Software") to use, reproduce, display, distribute,
execute, and transmit the Software, and to prepare derivative works of the
Software, and to permit third-parties to whom the Software is furnished to
do so, all subject to the following:

The copyright notices in the Software and this entire statement, including
the above license grant, this restriction and the following disclaimer,
must be included in all copies of the Software, in whole or in part, and
all derivative works of the Software, unless such copies or derivative
works are solely in the form of machine-executable object code generated by
a source language processor.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.

*/

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <map>
#include <stdexcept>
#include <vector>

#include <osmium/index/map.hpp>
#include <osmium/index/index.hpp>
#include <osmium/io/detail/read_write.hpp>

#define OSMIUM_HAS_INDEX_MAP_SPARSE_MEM_MAP

namespace osmium {

    namespace index {

        namespace map {

            /**
             * This implementation uses std::map internally. It uses rather a
             * lot of memory, but might make sense for small maps.
             */
            template <typename TId, typename TValue>
            class SparseMemMap : public osmium::index::map::Map<TId, TValue> {

                // This is a rough estimate for the memory needed for each
                // element in the map (id + value + pointers to left, right,
                // and parent plus some overhead for color of red-black-tree
                // or similar).
                static constexpr size_t element_size = sizeof(TId) + sizeof(TValue) + sizeof(void*) * 4;

                std::map<TId, TValue> m_elements;

            public:

                SparseMemMap() = default;

                ~SparseMemMap() override final = default;

                void set(const TId id, const TValue value) override final {
                    m_elements[id] = value;
                }

                const TValue get(const TId id) const override final {
                    try {
                        return m_elements.at(id);
                    } catch (std::out_of_range&) {
                        not_found_error(id);
                    }
                }

                size_t size() const override final {
                    return m_elements.size();
                }

                size_t used_memory() const override final {
                    return element_size * m_elements.size();
                }

                void clear() override final {
                    m_elements.clear();
                }

                void dump_as_list(const int fd) override final {
                    typedef typename std::map<TId, TValue>::value_type t;
                    std::vector<t> v;
                    std::copy(m_elements.begin(), m_elements.end(), std::back_inserter(v));
                    osmium::io::detail::reliable_write(fd, reinterpret_cast<const char*>(v.data()), sizeof(t) * v.size());
                }

            }; // class SparseMemMap

        } // namespace map

    } // namespace index

} // namespace osmium

#endif // OSMIUM_INDEX_MAP_SPARSE_MEM_MAP_HPP
