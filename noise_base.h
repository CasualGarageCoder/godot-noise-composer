#ifndef NOISE_COMPOSER_BASE_H
#define NOISE_COMPOSER_BASE_H

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

#endif