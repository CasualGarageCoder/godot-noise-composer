/**************************************************************************/
/* No Copyright, CC0                                                      */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/

#pragma once

#include "core/object/class_db.h"

#include "modules/noise/noise.h"

#include <cstddef>
#include <iterator>

class NoiseNode : public Noise {
	GDCLASS(NoiseNode, Noise);

public:
	NoiseNode(size_t c) :
			count{ c } {}

	int get_child_count() { return count; }

	virtual Ref<Noise> get_child(int n) const = 0;

protected:
	static void _bind_methods() {
		ClassDB::bind_method(D_METHOD("get_child", "n"), &NoiseNode::get_child);
		ClassDB::bind_method(D_METHOD("get_child_count"), &NoiseNode::get_child_count);
	}

private:
	size_t count;

public:
	struct Iterator {
		using iterator_category = std::input_iterator_tag;
		using difference_type = std::ptrdiff_t;
		using value_type = Ref<Noise>;
		using pointer = Ref<Noise>;
		using reference = Ref<Noise>;

		Iterator(NoiseNode *n, int pos) :
				node{ n }, index(pos) {}

		reference operator*() const { return node->get_child(index); }
		pointer operator->() const { return node->get_child(index); }

		Iterator &operator++() {
			++index;
			return *this;
		}
		Iterator operator++(int) {
			Iterator tmp = *this;
			++(*this);
			return tmp;
		}

		friend bool operator==(const Iterator &a, const Iterator &b) { return (a.node == b.node) && (a.index == b.index); }
		friend bool operator!=(const Iterator &a, const Iterator &b) { return (a.node != b.node) || (a.index != b.index); }

	private:
		NoiseNode *node;
		int index;
	};

public:
	Iterator begin() { return Iterator(this, 0); }
	Iterator end() { return Iterator(this, count); }
};