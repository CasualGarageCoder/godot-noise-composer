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

#include "core/object/object.h"
#include "core/object/ref_counted.h"
#include "noise_operator.h"
#include "scene/resources/curve.h"
#include <algorithm>
#include <iostream>

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
	real_t value;

public:
	ConstantNoise() :
			NaryNoiseOperator<0>([&](const std::array<real_t, 0> &) { return value; }), value(0.) {}
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
	real_t upper_bound;
	real_t lower_bound;
	real_t interval;
	bool normalize;

public:
	ClampNoise() :
			NaryNoiseOperator<1>(
					[&](const std::array<real_t, 1> &a) {
						real_t clamped = std::clamp(a[0], lower_bound, upper_bound);
						return (normalize && interval != 0.) ? (clamped - lower_bound) / interval : clamped;
					}),
			upper_bound(1.),
			lower_bound(-1.),
			interval(2.),
			normalize(false) {}

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
	real_t scale;
	real_t bias;

public:
	AffineNoise() :
			NaryNoiseOperator<1>([&](const std::array<real_t, 1> &a) { return (scale * a[0]) + bias; }), scale(1.), bias(0.) {}
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
	real_t threshold;

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
	bool cached_1d;
	bool cached_2d;
	bool cached_3d;
	real_t coord_1d;
	Vector2 coord_2d;
	Vector3 coord_3d;
	real_t value_1d;
	real_t value_2d;
	real_t value_3d;

public:
	NoiseProxy() :
			cached_1d(false), cached_2d(false), cached_3d(false) {}
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

#endif