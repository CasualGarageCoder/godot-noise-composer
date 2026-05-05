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

#include "noise_base.h"

class DerivativeNoise : public NoiseNode {
	GDCLASS(DerivativeNoise, NoiseNode);
	OBJ_SAVE_TYPE(DerivativeNoise);

public:
	DerivativeNoise() :
			NoiseNode(1) {}

	virtual ~DerivativeNoise() {}

	void set_source(Ref<Noise> n);
	Ref<Noise> get_source() const { return source; }

	virtual Ref<Noise> get_child(int) const override { return source; }

	virtual real_t get_noise_1d(real_t p_x) const override;

	virtual real_t get_noise_2dv(Vector2 p_v) const override;
	virtual real_t get_noise_2d(real_t p_x, real_t p_y) const override;

	virtual real_t get_noise_3dv(Vector3 p_v) const override;
	virtual real_t get_noise_3d(real_t p_x, real_t p_y, real_t p_z) const override;

	void set_distance(real_t v);
	_FORCE_INLINE_ real_t get_distance() const { return distance; }

protected:
	static void _bind_methods();

	void _changed() {
		emit_changed();
	}

private:
	/** Source noise. */
	Ref<Noise> source;

	/** Distance from the point. */
	double distance{ 1. };
};

class SmoothNoise : public NoiseNode {
	GDCLASS(SmoothNoise, NoiseNode);

public:
	SmoothNoise() :
			NoiseNode(1) {
		compute_kernels();
	}

	virtual ~SmoothNoise() {}

	void set_source(Ref<Noise> n);
	Ref<Noise> get_source() const { return source; }

	virtual Ref<Noise> get_child(int) const override { return source; }

	virtual real_t get_noise_1d(real_t p_x) const override;

	virtual real_t get_noise_2dv(Vector2 p_v) const override;
	virtual real_t get_noise_2d(real_t p_x, real_t p_y) const override;

	virtual real_t get_noise_3dv(Vector3 p_v) const override;
	virtual real_t get_noise_3d(real_t p_x, real_t p_y, real_t p_z) const override;

	void set_distance(real_t v);
	_FORCE_INLINE_ real_t get_distance() const { return distance; }

	void set_sample_count(int v);
	_FORCE_INLINE_ int get_sample_count() const { return sample_count; }

protected:
	static void _bind_methods();

	void _changed() {
		emit_changed();
	}

private:
	void compute_kernels();

private:
	Ref<Noise> source;

	// Maximum distance of evaluation
	real_t distance{ 1 };

	// Samples count
	int sample_count{ 16 };

	// Baked kernel for 1D noise
	Vector2 *kernel_1d{ nullptr };
	real_t weight_1d;

	// Baked kernel for 2D noise
	Vector3 *kernel_2d{ nullptr };
	real_t weight_2d;

	// Baked kernel for 3D noise
	Vector4 *kernel_3d{ nullptr };
	real_t weight_3d;

	// Baking guard
	std::shared_mutex bake_mutex;
};