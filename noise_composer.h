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

#ifndef NOISE_COMPOSER_H
#define NOISE_COMPOSER_H

#include "core/math/transform_2d.h"
#include "core/object/object.h"
#include "core/object/ref_counted.h"
#include "core/os/mutex.h"
#include "core/os/thread.h"
#include "noise_operator.h"
#include "scene/resources/curve.h"
#include <algorithm>

#define DECLARE_NOISE_OPERAND(op_name, op_num)                   \
	void set_##op_name(Ref<Noise> n) { set_operand(n, op_num); } \
	Ref<Noise> get_##op_name() const { return get_operand(op_num); }

#define REGISTER_NOISE_OPERAND(class_name, op_name, property_name) \
	ClassDB::bind_method(D_METHOD("set_" #op_name, "n"),           \
			&class_name::set_##op_name);                           \
	ClassDB::bind_method(D_METHOD("get_" #op_name),                \
			&class_name::get_##op_name);                           \
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, #property_name,     \
						 PROPERTY_HINT_RESOURCE_TYPE, "Noise"),    \
			"set_" #op_name, "get_" #op_name);

class ConstantNoise : public NaryNoiseOperator<0> {
	GDCLASS(ConstantNoise, Noise);
	OBJ_SAVE_TYPE(ConstantNoise);

private:
	real_t value{ 0. };

public:
	ConstantNoise() :
			NaryNoiseOperator<0>([&](const std::array<real_t, 0> &) { return value; }) {}
	virtual ~ConstantNoise() {}

	void set_value(real_t v) {
		value = v;
		emit_changed();
	}
	real_t get_value() const { return value; }

protected:
	static void _bind_methods();
};

// Noise combiners

class NoiseCombinerOperator : public NaryNoiseOperator<2> {
	GDCLASS(NoiseCombinerOperator, Noise);

public:
	NoiseCombinerOperator(std::function<real_t(const std::array<real_t, 2> &)> f) :
			NaryNoiseOperator<2>(f) {}
	virtual ~NoiseCombinerOperator() {}

	DECLARE_NOISE_OPERAND(first_noise, 0)
	DECLARE_NOISE_OPERAND(second_noise, 1)

protected:
	static void _bind_methods();
};

class AddNoise : public NoiseCombinerOperator {
	GDCLASS(AddNoise, NoiseCombinerOperator);
	OBJ_SAVE_TYPE(AddNoise);

public:
	AddNoise() :
			NoiseCombinerOperator([](const std::array<real_t, 2> &a) { return a[0] + a[1]; }) {}
	virtual ~AddNoise() {}
};

class MultiplyNoise : public NoiseCombinerOperator {
	GDCLASS(MultiplyNoise, NoiseCombinerOperator);
	OBJ_SAVE_TYPE(MultiplyNoise);

public:
	MultiplyNoise() :
			NoiseCombinerOperator([](const std::array<real_t, 2> &a) { return a[0] * a[1]; }) {}
	virtual ~MultiplyNoise() {}
};

class MaxNoise : public NoiseCombinerOperator {
	GDCLASS(MaxNoise, NoiseCombinerOperator);
	OBJ_SAVE_TYPE(MaxNoise);

public:
	MaxNoise() :
			NoiseCombinerOperator([](const std::array<real_t, 2> &a) { return std::max(a[0], a[1]); }) {}
	virtual ~MaxNoise() {}
};

class MinNoise : public NoiseCombinerOperator {
	GDCLASS(MinNoise, NoiseCombinerOperator);
	OBJ_SAVE_TYPE(MinNoise);

public:
	MinNoise() :
			NoiseCombinerOperator([](const std::array<real_t, 2> &a) { return std::min(a[0], a[1]); }) {}
	virtual ~MinNoise() {}
};

class PowerNoise : public NoiseCombinerOperator {
	GDCLASS(PowerNoise, NoiseCombinerOperator);
	OBJ_SAVE_TYPE(PowerNoise);

public:
	PowerNoise() :
			NoiseCombinerOperator([](const std::array<real_t, 2> &a) { return std::pow(a[0], a[1]); }) {}
	virtual ~PowerNoise() {}
};

// Noise modifiers

class AbsoluteNoise : public NaryNoiseOperator<1> {
	GDCLASS(AbsoluteNoise, Noise);
	OBJ_SAVE_TYPE(AbsoluteNoise);

public:
	AbsoluteNoise() :
			NaryNoiseOperator<1>([](const std::array<real_t, 1> &a) { return std::abs(a[0]); }) {}
	virtual ~AbsoluteNoise() {}

	DECLARE_NOISE_OPERAND(source, 0)

protected:
	static void _bind_methods();
};

class InvertNoise : public NaryNoiseOperator<1> {
	GDCLASS(InvertNoise, Noise);
	OBJ_SAVE_TYPE(InvertNoise);

public:
	InvertNoise() :
			NaryNoiseOperator<1>([](const std::array<real_t, 1> &a) { return -a[0]; }) {}
	virtual ~InvertNoise() {}

	DECLARE_NOISE_OPERAND(source, 0)

protected:
	static void _bind_methods();
};

class ClampNoise : public NaryNoiseOperator<1> {
	GDCLASS(ClampNoise, Noise);
	OBJ_SAVE_TYPE(ClampNoise);

private:
	real_t upper_bound{ 1. };
	real_t lower_bound{ -1. };
	real_t interval{ 2. };
	bool normalize{ false };

public:
	ClampNoise() :
			NaryNoiseOperator<1>(
					[&](const std::array<real_t, 1> &a) {
						real_t clamped = std::clamp(a[0], lower_bound, upper_bound);
						return (normalize && interval != 0.) ? (clamped - lower_bound) / interval : clamped;
					}) {}

	virtual ~ClampNoise() {}

	DECLARE_NOISE_OPERAND(source, 0)

	void set_lower_bound(real_t v);
	real_t get_lower_bound() const { return lower_bound; }
	void set_upper_bound(real_t v);
	real_t get_upper_bound() const { return upper_bound; }
	void set_normalized(bool f);
	bool is_normalized() const { return normalize; }

protected:
	static void _bind_methods();
};

class CurveNoise : public NaryNoiseOperator<1> {
	GDCLASS(CurveNoise, Noise);
	OBJ_SAVE_TYPE(CurveNoise);

private:
	Ref<Curve> curve;

public:
	CurveNoise() :
			NaryNoiseOperator<1>([&](const std::array<real_t, 1> &a) { return curve.is_valid() ? curve->sample_baked((a[0] + 1.) / 2.) : 0.; }) {}
	virtual ~CurveNoise() {}

	DECLARE_NOISE_OPERAND(source, 0)

	void set_curve(Ref<Curve> c);
	Ref<Curve> get_curve() const { return curve; }

protected:
	void _curve_changed() {
#ifdef __NOT_GONNA_HAPPEN
		emit_changed();
#endif
	}

protected:
	static void _bind_methods();
};

class AffineNoise : public NaryNoiseOperator<1> {
	GDCLASS(AffineNoise, Noise);
	OBJ_SAVE_TYPE(AffineNoise);

private:
	real_t scale{ 1. };
	real_t bias{ 0. };

public:
	AffineNoise() :
			NaryNoiseOperator<1>([&](const std::array<real_t, 1> &a) { return (scale * a[0]) + bias; }) {}
	virtual ~AffineNoise() {}

	DECLARE_NOISE_OPERAND(source, 0)

	void set_scale(real_t s);
	real_t get_scale() const;

	void set_bias(real_t b);
	real_t get_bias() const;

protected:
	static void _bind_methods();
};

// Noise Selectors

class MixNoise : public NaryNoiseOperator<3> {
	GDCLASS(MixNoise, Noise);
	OBJ_SAVE_TYPE(MixNoise);

public:
	MixNoise() :
			NaryNoiseOperator<3>([&](const std::array<real_t, 3> &a) {
				real_t ratio = (a[2] + 1.) / 2.;
				return (ratio * a[1]) + ((1. - ratio) * a[0]);
			}) {}
	virtual ~MixNoise() {}

	DECLARE_NOISE_OPERAND(first_noise, 0)
	DECLARE_NOISE_OPERAND(second_noise, 1)
	DECLARE_NOISE_OPERAND(selector_noise, 2)

protected:
	static void _bind_methods();
};

class SelectNoise : public NaryNoiseOperator<3> {
	GDCLASS(SelectNoise, Noise);
	OBJ_SAVE_TYPE(SelectNoise)

private:
	real_t threshold{ 0. };

public:
	SelectNoise() :
			NaryNoiseOperator<3>([&](const std::array<real_t, 3> &a) {
				return (a[2] < threshold) ? a[0] : a[1];
			}) {}
	virtual ~SelectNoise() {}

	DECLARE_NOISE_OPERAND(first_noise, 0)
	DECLARE_NOISE_OPERAND(second_noise, 1)
	DECLARE_NOISE_OPERAND(selector_noise, 2)

	void set_threshold(real_t);
	real_t get_threshold() const;

protected:
	static void _bind_methods();
};

// Noise Proxy

class NoiseProxy : public Noise {
	GDCLASS(NoiseProxy, Noise);
	OBJ_SAVE_TYPE(NoiseProxy)

private:
	Ref<Noise> source;
	bool cached_1d{ false };
	bool cached_2d{ false };
	bool cached_3d{ false };
	real_t coord_1d{ 0. };
	Vector2 coord_2d{ 0., 0. };
	Vector3 coord_3d{ 0., 0., 0. };
	real_t value_1d{ 0. };
	real_t value_2d{ 0. };
	real_t value_3d{ 0. };

public:
	NoiseProxy() {}
	virtual ~NoiseProxy();

	virtual real_t get_noise_1d(real_t p_x) const override;

	virtual real_t get_noise_2dv(Vector2 p_v) const override;
	virtual real_t get_noise_2d(real_t p_x, real_t p_y) const override;

	virtual real_t get_noise_3dv(Vector3 p_v) const override;
	virtual real_t get_noise_3d(real_t p_x, real_t p_y, real_t p_z) const override;

	void set_source(Ref<Noise> n);
	Ref<Noise> get_source() const;

protected:
	void _changed() {
		emit_changed();
	}

	static void _bind_methods();

private:
	real_t _get_noise_1d(real_t p_x);
	real_t _get_noise_2d(real_t p_x, real_t p_y);
	real_t _get_noise_3d(real_t p_x, real_t p_y, real_t p_z);
};

class NoiseCoordinateRecompute : public Noise {
public:
	NoiseCoordinateRecompute() {}
	virtual ~NoiseCoordinateRecompute() {}

	virtual real_t get_noise_1d(real_t p_x) const override;

	virtual real_t get_noise_2dv(Vector2 p_v) const override;
	virtual real_t get_noise_2d(real_t p_x, real_t p_y) const override;

	virtual real_t get_noise_3dv(Vector3 p_v) const override;
	virtual real_t get_noise_3d(real_t p_x, real_t p_y, real_t p_z) const override;

	void set_inner_noise(Ref<Noise> n);
	Ref<Noise> get_inner_noise() const { return inner; }

protected:
	virtual real_t transform(real_t s) const = 0;
	virtual Vector2 transform(Vector2 s) const = 0;
	virtual Vector3 transform(Vector3 s) const = 0;

	void _changed() {
		emit_changed();
	}

private:
	Ref<Noise> inner;
};

class LinearTransformNoise : public NoiseCoordinateRecompute {
	GDCLASS(LinearTransformNoise, Noise)
	OBJ_SAVE_TYPE(LinearTransformNoise)
public:
	LinearTransformNoise() {}
	virtual ~LinearTransformNoise() {}

	void set_scale(real_t s);
	real_t get_scale() const { return scale; }

	void set_bias(real_t b);
	real_t get_bias() const { return bias; }

	void set_transform_2d(Transform2D t);
	const Transform2D &get_transform_2d() const { return transform_2d; }

	void set_transform_3d(Transform3D t);
	const Transform3D &get_transform_3d() const { return transform_3d; }

protected:
	static void _bind_methods();

	virtual real_t transform(real_t s) const override { return (s * scale) + bias; }
	virtual Vector2 transform(Vector2 s) const override { return transform_2d.xform(s); }
	virtual Vector3 transform(Vector3 s) const override { return transform_3d.xform(s); }

private:
	real_t scale{ 1. };
	real_t bias{ 0. };
	Transform2D transform_2d;
	Transform3D transform_3d;
};

class RescalerNoise : public Noise {
	GDCLASS(RescalerNoise, Noise)
	OBJ_SAVE_TYPE(RescalerNoise)

public:
	RescalerNoise() {}
	virtual ~RescalerNoise();

	void set_noise(Ref<Noise> n);
	Ref<Noise> get_noise() const { return noise; }

	bool is_working() const { return update_thread.is_started(); }

	real_t get_scale() const;
	real_t get_bias() const;

	void set_range(real_t r);
	real_t get_range() const { return range; }

	void set_step(real_t s);
	real_t get_step() const { return step; }

	virtual real_t get_noise_1d(real_t p_x) const override;

	virtual real_t get_noise_2dv(Vector2 p_v) const override;
	virtual real_t get_noise_2d(real_t p_x, real_t p_y) const override;

	virtual real_t get_noise_3dv(Vector3 p_v) const override;
	virtual real_t get_noise_3d(real_t p_x, real_t p_y, real_t p_z) const override;

	friend void compute_affine_transformation(void *);

protected:
	static void _bind_methods();

private:
	static void compute_affine_transformation(void *);
	void queue_update();

private:
	Ref<Noise> noise;
	real_t scale{ 0. };
	real_t bias{ 0. };
	real_t range{ 16. };
	real_t step{ 0.5 };
	bool update_queued{ false };
	Thread update_thread;
	Mutex queue_mutex;
};
#endif