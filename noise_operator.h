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

#ifndef NOISE_OPERATOR_H
#define NOISE_OPERATOR_H

#include <cstddef>
#include <functional>

#include "modules/noise/noise.h"

template <std::size_t N>
class NaryNoiseOperator : public Noise {
	GDCLASS(NaryNoiseOperator<N>, Noise);

private:
	std::array<Ref<Noise>, N> operands;
	std::function<real_t(const std::array<real_t, N> &)> function;

public:
	explicit NaryNoiseOperator(std::function<real_t(const std::array<real_t, N> &)> f) :
			function(f) {
	}

	virtual ~NaryNoiseOperator() {
		for (size_t i = 0; i < N; ++i) {
			if (operands[i].is_valid()) {
				operands[i]->disconnect_changed(callable_mp(this, &NaryNoiseOperator<N>::_changed));
			}
		}
	}

	size_t get_operator_count() const { return N; }

	real_t get_noise_1d(real_t p_x) const override {
		std::array<real_t, N> result;
		for (size_t i = 0; i < N; ++i) {
			result[i] = operands[i].is_valid() ? operands[i]->get_noise_1d(p_x) : 0.;
		}
		return function(result);
	}

	real_t get_noise_2dv(Vector2 p_v) const override {
		return get_noise_2d(p_v.x, p_v.y);
	}

	real_t get_noise_2d(real_t p_x, real_t p_y) const override {
		std::array<real_t, N> result;
		for (size_t i = 0; i < N; ++i) {
			result[i] = operands[i].is_valid() ? operands[i]->get_noise_2d(p_x, p_y) : 0.;
		}
		return function(result);
	}

	real_t get_noise_3dv(Vector3 p_v) const override {
		return get_noise_3d(p_v.x, p_v.y, p_v.z);
	}

	real_t get_noise_3d(real_t p_x, real_t p_y, real_t p_z) const override {
		std::array<real_t, N> result;
		for (size_t i = 0; i < N; ++i) {
			result[i] = operands[i].is_valid() ? operands[i]->get_noise_3d(p_x, p_y, p_z) : 0.;
		}
		return function(result);
	}

protected:
	void _changed() {
		emit_changed();
	}

	void set_operand(Ref<Noise> n, size_t index) {
		ERR_FAIL_COND_MSG(index < 0 || index >= N, "Invalid operand index");
		if (operands[index].is_valid()) {
			operands[index]->disconnect_changed(callable_mp(this, &NaryNoiseOperator<N>::_changed));
		}
		operands[index] = n;
		if (operands[index].is_valid()) {
			operands[index]->connect_changed(callable_mp(this, &NaryNoiseOperator<N>::_changed));
		}
	}

	Ref<Noise> get_operand(size_t index) const {
		return operands[index];
	}
};

#endif