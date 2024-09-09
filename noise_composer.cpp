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

#include "noise_composer.h"

void ConstantNoise::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_value", "v"), &ConstantNoise::set_value);
	ClassDB::bind_method(D_METHOD("get_value"), &ConstantNoise::get_value);

	ADD_PROPERTY(
			PropertyInfo(Variant::FLOAT, "value", PROPERTY_HINT_RANGE, "-1,1,0.001"),
			"set_value", "get_value");
}

void NoiseCombinerOperator::set_first_noise(Ref<Noise> n) {
	NaryNoiseOperator<2>::set_operand(n, 0);
}
void NoiseCombinerOperator::set_second_noise(Ref<Noise> n) {
	NaryNoiseOperator<2>::set_operand(n, 1);
}

void NoiseCombinerOperator::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_first_noise", "n"),
			&NoiseCombinerOperator::set_first_noise);
	ClassDB::bind_method(D_METHOD("set_second_noise", "n"),
			&NoiseCombinerOperator::set_second_noise);
	ClassDB::bind_method(D_METHOD("get_first_noise"),
			&NoiseCombinerOperator::get_first_noise);
	ClassDB::bind_method(D_METHOD("get_second_noise"),
			&NoiseCombinerOperator::get_second_noise);

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "first",
						 PROPERTY_HINT_RESOURCE_TYPE, "Noise"),
			"set_first_noise", "get_first_noise");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "second",
						 PROPERTY_HINT_RESOURCE_TYPE, "Noise"),
			"set_second_noise", "get_second_noise");
}

void AbsoluteNoise::set_source(Ref<Noise> n) { set_operand(n, 0); }

void AbsoluteNoise::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_source", "n"), &AbsoluteNoise::set_source);
	ClassDB::bind_method(D_METHOD("get_source"), &AbsoluteNoise::get_source);

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "source",
						 PROPERTY_HINT_RESOURCE_TYPE, "Noise"),
			"set_source", "get_source");
}

void InvertNoise::set_source(Ref<Noise> n) { set_operand(n, 0); }

void InvertNoise::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_source", "n"), &InvertNoise::set_source);
	ClassDB::bind_method(D_METHOD("get_source"), &InvertNoise::get_source);

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "source",
						 PROPERTY_HINT_RESOURCE_TYPE, "Noise"),
			"set_source", "get_source");
}

void ClampNoise::set_source(Ref<Noise> n) { set_operand(n, 0); }

void ClampNoise::set_lower_bound(real_t v) {
	lower_bound = std::min(v, upper_bound);
	interval = upper_bound - lower_bound;
	emit_changed();
}

void ClampNoise::set_upper_bound(real_t v) {
	upper_bound = std::max(v, lower_bound);
	interval = upper_bound - lower_bound;
	emit_changed();
}

void ClampNoise::set_normalized(bool f) {
	normalize = f;
	emit_changed();
}

void ClampNoise::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_source", "n"), &ClampNoise::set_source);
	ClassDB::bind_method(D_METHOD("get_source"), &ClampNoise::get_source);
	ClassDB::bind_method(D_METHOD("set_lower_bound", "v"),
			&ClampNoise::set_lower_bound);
	ClassDB::bind_method(D_METHOD("get_lower_bound"),
			&ClampNoise::get_lower_bound);
	ClassDB::bind_method(D_METHOD("set_upper_bound", "v"),
			&ClampNoise::set_upper_bound);
	ClassDB::bind_method(D_METHOD("get_upper_bound"),
			&ClampNoise::get_upper_bound);
	ClassDB::bind_method(D_METHOD("set_normalized", "v"),
			&ClampNoise::set_normalized);
	ClassDB::bind_method(D_METHOD("is_normalized"), &ClampNoise::is_normalized);

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "source",
						 PROPERTY_HINT_RESOURCE_TYPE, "Noise"),
			"set_source", "get_source");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "lower_bound", PROPERTY_HINT_RANGE,
						 "-1,1,0.001"),
			"set_lower_bound", "get_lower_bound");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "upper_bound", PROPERTY_HINT_RANGE,
						 "-1,1,0.001"),
			"set_upper_bound", "get_upper_bound");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "normalized"), "set_normalized",
			"is_normalized");
}

void CurveNoise::set_source(Ref<Noise> n) { set_operand(n, 0); }

void CurveNoise::set_curve(Ref<Curve> c) {
	curve = c;
	emit_changed();
}

void CurveNoise::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_source", "n"), &CurveNoise::set_source);
	ClassDB::bind_method(D_METHOD("get_source"), &CurveNoise::get_source);

	ClassDB::bind_method(D_METHOD("set_curve", "c"), &CurveNoise::set_curve);
	ClassDB::bind_method(D_METHOD("get_curve"), &CurveNoise::get_curve);

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "source",
						 PROPERTY_HINT_RESOURCE_TYPE, "Noise"),
			"set_source", "get_source");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "curve",
						 PROPERTY_HINT_RESOURCE_TYPE, "Curve"),
			"set_curve", "get_curve");
}

void AffineNoise::set_source(Ref<Noise> n) { set_operand(n, 0); }

void AffineNoise::set_scale(real_t s) {
	scale = s;
	emit_changed();
}

real_t AffineNoise::get_scale() const { return scale; }

void AffineNoise::set_bias(real_t b) {
	bias = b;
	emit_changed();
}

real_t AffineNoise::get_bias() const { return bias; }

void AffineNoise::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_source", "n"), &AffineNoise::set_source);
	ClassDB::bind_method(D_METHOD("get_source"), &AffineNoise::get_source);

	ClassDB::bind_method(D_METHOD("set_scale", "v"), &AffineNoise::set_scale);
	ClassDB::bind_method(D_METHOD("get_scale"), &AffineNoise::get_scale);
	ClassDB::bind_method(D_METHOD("set_bias", "v"), &AffineNoise::set_bias);
	ClassDB::bind_method(D_METHOD("get_bias"), &AffineNoise::get_bias);

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "source",
						 PROPERTY_HINT_RESOURCE_TYPE, "Noise"),
			"set_source", "get_source");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "scale"), "set_scale", "get_scale");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "bias"), "set_bias", "get_bias");
}

void MixNoise::set_first_noise(Ref<Noise> n) { set_operand(n, 0); }

Ref<Noise> MixNoise::get_first_noise() const { return get_operand(0); }

void MixNoise::set_second_noise(Ref<Noise> n) { set_operand(n, 1); }

Ref<Noise> MixNoise::get_second_noise() const { return get_operand(1); }

void MixNoise::set_selector_noise(Ref<Noise> n) { set_operand(n, 2); }

Ref<Noise> MixNoise::get_selector_noise() const { return get_operand(2); }

void MixNoise::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_first_noise", "n"),
			&MixNoise::set_first_noise);
	ClassDB::bind_method(D_METHOD("set_second_noise", "n"),
			&MixNoise::set_second_noise);
	ClassDB::bind_method(D_METHOD("set_selector_noise", "n"),
			&MixNoise::set_selector_noise);
	ClassDB::bind_method(D_METHOD("get_first_noise"),
			&MixNoise::get_first_noise);
	ClassDB::bind_method(D_METHOD("get_second_noise"),
			&MixNoise::get_second_noise);
	ClassDB::bind_method(D_METHOD("get_selector_noise"),
			&MixNoise::get_selector_noise);

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "first",
						 PROPERTY_HINT_RESOURCE_TYPE, "Noise"),
			"set_first_noise", "get_first_noise");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "second",
						 PROPERTY_HINT_RESOURCE_TYPE, "Noise"),
			"set_second_noise", "get_second_noise");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "selector",
						 PROPERTY_HINT_RESOURCE_TYPE, "Noise"),
			"set_selector_noise", "get_selector_noise");
}

void SelectNoise::set_first_noise(Ref<Noise> n) { set_operand(n, 0); }

Ref<Noise> SelectNoise::get_first_noise() const { return get_operand(0); }

void SelectNoise::set_second_noise(Ref<Noise> n) { set_operand(n, 1); }

Ref<Noise> SelectNoise::get_second_noise() const { return get_operand(1); }

void SelectNoise::set_selector_noise(Ref<Noise> n) { set_operand(n, 2); }

Ref<Noise> SelectNoise::get_selector_noise() const { return get_operand(2); }

void SelectNoise::set_threshold(real_t t) {
	threshold = t;
	emit_changed();
}

real_t SelectNoise::get_threshold() const { return threshold; }

void SelectNoise::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_first_noise", "n"),
			&SelectNoise::set_first_noise);
	ClassDB::bind_method(D_METHOD("set_second_noise", "n"),
			&SelectNoise::set_second_noise);
	ClassDB::bind_method(D_METHOD("set_selector_noise", "n"),
			&SelectNoise::set_selector_noise);
	ClassDB::bind_method(D_METHOD("get_first_noise"),
			&SelectNoise::get_first_noise);
	ClassDB::bind_method(D_METHOD("get_second_noise"),
			&SelectNoise::get_second_noise);
	ClassDB::bind_method(D_METHOD("get_selector_noise"),
			&SelectNoise::get_selector_noise);

	ClassDB::bind_method(D_METHOD("set_threshold", "t"),
			&SelectNoise::set_threshold);
	ClassDB::bind_method(D_METHOD("get_treshold"),
			&SelectNoise::get_threshold);

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "first",
						 PROPERTY_HINT_RESOURCE_TYPE, "Noise"),
			"set_first_noise", "get_first_noise");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "second",
						 PROPERTY_HINT_RESOURCE_TYPE, "Noise"),
			"set_second_noise", "get_second_noise");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "selector",
						 PROPERTY_HINT_RESOURCE_TYPE, "Noise"),
			"set_selector_noise", "get_selector_noise");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "threshold"), "set_threshold", "get_threshold");
}

NoiseProxy::~NoiseProxy() {
	if (source.is_valid()) {
		source->disconnect_changed(callable_mp(this, &NoiseProxy::_changed));
	}
}

real_t NoiseProxy::get_noise_1d(real_t p_x) const {
	return const_cast<NoiseProxy *>(this)->_get_noise_1d(p_x);
}

real_t NoiseProxy::_get_noise_1d(real_t p_x) {
	cached_1d &= source.is_valid() && Math::is_equal_approx(p_x, coord_1d);
	if (!cached_1d) {
		value_1d = source->get_noise_1d(p_x);
		coord_1d = p_x;
		cached_1d = true;
	}
	return value_1d;
}

real_t NoiseProxy::get_noise_2dv(Vector2 p_v) const {
	return get_noise_2d(p_v.x, p_v.y);
}

real_t NoiseProxy::get_noise_2d(real_t p_x, real_t p_y) const {
	return const_cast<NoiseProxy *>(this)->_get_noise_2d(p_x, p_y);
}

real_t NoiseProxy::_get_noise_2d(real_t p_x, real_t p_y) {
	cached_2d &= source.is_valid() && Math::is_equal_approx(coord_2d.x, p_x) && Math::is_equal_approx(coord_2d.y, p_y);
	if (!cached_2d) {
		value_2d = source->get_noise_2d(p_x, p_y);
		coord_2d.x = p_x;
		coord_2d.y = p_y;
		cached_2d = true;
	}
	return value_2d;
}

real_t NoiseProxy::get_noise_3dv(Vector3 p_v) const {
	return get_noise_3d(p_v.x, p_v.y, p_v.z);
}

real_t NoiseProxy::get_noise_3d(real_t p_x, real_t p_y, real_t p_z) const {
	return const_cast<NoiseProxy *>(this)->_get_noise_3d(p_x, p_y, p_z);
}

real_t NoiseProxy::_get_noise_3d(real_t p_x, real_t p_y, real_t p_z) {
	cached_3d &= source.is_valid() && Math::is_equal_approx(coord_3d.x, p_x) && Math::is_equal_approx(coord_3d.y, p_y) && Math::is_equal_approx(coord_3d.z, p_z);
	if (!cached_3d) {
		value_3d = source->get_noise_3d(p_x, p_y, p_z);
		coord_3d.x = p_x;
		coord_3d.y = p_y;
		coord_3d.z = p_z;
		cached_3d = true;
	}
	return value_3d;
}

void NoiseProxy::set_source(Ref<Noise> n) {
	if (source.is_valid()) {
		source->disconnect_changed(callable_mp(this, &NoiseProxy::_changed));
	}
	if (n.is_valid()) {
		n->connect_changed(callable_mp(this, &NoiseProxy::_changed));
	}
	source = n;
	emit_changed();
}

Ref<Noise> NoiseProxy::get_source() const {
	return source;
}

void NoiseProxy::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_source", "n"), &NoiseProxy::set_source);
	ClassDB::bind_method(D_METHOD("get_source"), &NoiseProxy::get_source);

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "source",
						 PROPERTY_HINT_RESOURCE_TYPE, "Noise"),
			"set_source", "get_source");
}